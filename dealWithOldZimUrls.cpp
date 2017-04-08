#include <string>

#include <unicode/regex.h>
#include <unicode/ucnv.h>
#include <string>
#include <map>

#include <zim/file.h>
#include <zim/fileiterator.h>
#include <kiwix/reader.h>

#include "dealWithOldZimUrls.h"

/*
Portions of the code used with permission of the original author @kelson
*/

std::map<std::string, RegexMatcher*> regexCache;

RegexMatcher *buildRegex(const std::string &regex) {
  RegexMatcher *matcher;
  std::map<std::string, RegexMatcher*>::iterator itr = regexCache.find(regex);
  
  /* Regex is in cache */
  if (itr != regexCache.end()) {
    matcher = itr->second;
  }

  /* Regex needs to be parsed (and cached) */
  else {
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString uregex = UnicodeString(regex.c_str());
    matcher = new RegexMatcher(uregex, UREGEX_CASE_INSENSITIVE, status);
    regexCache[regex] = matcher;
  }

  return matcher;
}

/* todo */
void freeRegexCache() {
}

std::string replaceRegex(const std::string &content, const std::string &replacement, const std::string &regex) {
  ucnv_setDefaultName("UTF-8");
  UnicodeString ucontent = UnicodeString(content.c_str());
  UnicodeString ureplacement = UnicodeString(replacement.c_str());
  RegexMatcher *matcher = buildRegex(regex);
  matcher->reset(ucontent);
  UErrorCode status = U_ZERO_ERROR;
  UnicodeString uresult = matcher->replaceAll(ureplacement, status);
  std::string tmp;
  uresult.toUTF8String(tmp);
  return tmp;
}

void cleanUp(std::string const& mimeType, std::string const& prefix, zim::Article const& article, std::string &content) {
    std::string baseUrl;

    if (mimeType.find("text/") != std::string::npos ||
            mimeType.find("application/javascript") != std::string::npos ||
            mimeType.find("application/json") != std::string::npos)
    {
	// Removed the following as they're not relvant in a standalone function
        // pthread_mutex_lock(&readerLock);
        // content = string(raw_content.data(), raw_content.size());
        // pthread_mutex_unlock(&readerLock);

        /* Special rewrite URL in case of ZIM file use intern *asbolute* url like /A/Kiwix */
        if (mimeType.find("text/html") != std::string::npos) {
            if (content.find("<body") == std::string::npos &&
                    content.find("<BODY") == std::string::npos) {
                content = "<html><head><title>" + article.getTitle() + "</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /></head><body>" + content + "</body></html>";
            }
            baseUrl = "/" + std::string(1, article.getNamespace()) + "/" + article.getUrl();
            content = replaceRegex(content, "$1$2" + prefix + "/$3/",
                                   "(href|src)(=[\"|\']{0,1}/)([A-Z|\\-])/");
            content = replaceRegex(content, "$1$2" + prefix + "/$3/",
                                   "(@import[ ]+)([\"|\']{0,1}/)([A-Z|\\-])/");
            content = replaceRegex(content,
                                   "<head><base href=\"/" + prefix + baseUrl + "\" />",
                                   "<head>");
        } else if (mimeType.find("text/css") != std::string::npos) {
            content = replaceRegex(content, "$1$2" + prefix + "/$3/",
                                   "(url|URL)(\\([\"|\']{0,1}/)([A-Z|\\-])/");
        }

    }

}
