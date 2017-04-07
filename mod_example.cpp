#include "httpd.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

#include "apr_strings.h"

#include <zim/file.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <kiwix/reader.h>


extern "C" {
#ifdef APLOG_USE_MODULE
APLOG_USE_MODULE(kiwix);
#endif
}

static void register_hooks(apr_pool_t *pool);
static int kiwix_handler(request_rec *r);

extern "C" module kiwix_module;

module AP_MODULE_DECLARE_DATA kiwix_module = {
	STANDARD20_MODULE_STUFF, NULL, NULL, NULL, NULL, NULL, register_hooks
};

static void register_hooks(apr_pool_t *pool) {
	ap_hook_handler(kiwix_handler, NULL, NULL, APR_HOOK_LAST);
}

static int kiwix_handler(request_rec *r) {
	if (!r->handler || strcmp(r->handler, "kiwix"))
		return (DECLINED);
	
	kiwix::Reader *reader = NULL;
	string zimpath = "/var/www/html/wikipedia.zim";
	// TODO: parse the URL to obtain the requested object

	// Typical name of an object e.g. of a default page: 
	// http://localhost/kiwix/localcontent/A/index.html
	string urlStr = "";

	
	// TODO: if the requested object is the default, redirect the caller to
	// the default (home) page of the ZIM

	bool found = false;

	zim::Article article;
	string fullUrl(r->uri);
	string url = fullUrl.substr(fullUrl.find("kiwix/") + string("kiwix/").length()); 
	ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE, 0, r->server,
        	"full url = %s, kiwix url = %s",
        	fullUrl.c_str(), url.c_str());
	char& back = url.back();
	// url = "I/m/Africa_HIV-AIDS_300px.png";	
	try {
		zim::File f(zimpath);
		reader = new kiwix::Reader(zimpath);
		found = reader->getArticleObjectByDecodedUrl(url, article);
		if (found) {
			// Crude hack until I parse the URL correctly. 
			if (fullUrl.back() == '/') {
				/* If redirect */
				unsigned int loopCounter = 0;
                                 ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE, 0, r->server,
                                                   "---- good:%d, redirect:%d, url:%s",
                                                     article.good(), article.isRedirect(), article.getLongUrl().c_str());
                                
				while (article.isRedirect() && loopCounter++<421) {
                                        ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE, 0, r->server,
                                                     "good:%d, redirect:%d, url:%s",
                                                     article.good(), article.isRedirect(), article.getLongUrl().c_str());
					article = article.getRedirectArticle();
				}
				// apr_psprintf(r->pool, "%s", article.getLongUrl().c_str());
				// ap_set_content_type(r, "text/html");
				// apr_table_setn(r->headers_out, "Location", "/kiwix/A/Joliba.html"); //article.getLongUrl().c_str());
				apr_table_setn(r->headers_out, "Location", apr_pstrdup(r->pool, article.getLongUrl().c_str()));
 // article.getLongUrl().c_str());
				return HTTP_MOVED_TEMPORARILY;
			} else {
				/* If redirect */
				unsigned int loopCounter = 0;
				while (article.isRedirect() && loopCounter++<42) {
					article = article.getRedirectArticle();
				}
				
				string contentType = string(article.getMimeType().data(), article.getMimeType().size());
				ap_set_content_type(r, contentType.c_str());
				string content = string(article.getData().data(), article.getArticleSize()).c_str();
				if (contentType == "text/html" && std::string::npos == content.find("<body>")) {
					content = "<html><head><title>" + article.getTitle() 
						+ "</title><!--" + article.getLongUrl() + "--></head><body>" 
						+ content + "</body></html>";
				} 
				ap_rwrite(article.getData().data(), article.getArticleSize(), r);  //ap_rputs(content.c_str(), r);
			}
		}
		else {
			ap_set_content_type(r, "text/plain");
			for(zim::File::const_iterator it = f.begin(); it != f.end(); ++it) {
				ap_rputs((it->getMimeType() + " - " + it->getLongUrl() + " - " + it->getTitle() + "--\n").c_str(), r);
			}
		}
		reader = NULL;
	}
	catch (const std::exception& e) {
		ap_rputs(e.what(), r);
	}

	return OK;
}

