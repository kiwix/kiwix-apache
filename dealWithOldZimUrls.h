#ifndef DEAL_WITH_OLD_ZIM_URLS_H //dealWithOldZimUrls
#define DEAL_WITH_OLD_ZIM_URLS_H //dealWithOldZimUrls

#include <string>
void cleanUp(std::string const& mimeType, std::string const& prefix, zim::Article const& article, std::string &content);
#endif
