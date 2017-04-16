#include "httpd.h"
#include "http_core.h"
#include "http_log.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_request.h"

#include "apr_strings.h"

#include <zim/file.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <kiwix/reader.h>

#include "dealWithOldZimUrls.h"

/*
Portions of the code used with permission of the original author @kelson
*/

extern "C" {
#ifdef APLOG_USE_MODULE
    APLOG_USE_MODULE(kiwix);
#endif
}

static void register_hooks(apr_pool_t *pool);
static int kiwix_handler(request_rec *r);

extern "C" {
    typedef struct {
        const char *path;
        const char *zimfilename;
    } kiwix_config;
}

static kiwix_config config;


/* Handler for the "examplePath" directive */
extern "C" const char *kiwix_set_path(cmd_parms *cmd, void *cfg, const char *arg)
{
    config.path = arg;
    return NULL;
}

extern "C" const char *kiwix_set_zimfilename(cmd_parms *cmd, void *cfg, const char *arg)
{
    config.zimfilename = arg;
    return NULL;
}

extern "C" {
    static const command_rec        kiwix_settings[] =
    {
        AP_INIT_TAKE1("zimFile", (const char* (*)())kiwix_set_zimfilename, NULL, RSRC_CONF, "The ZIM filename in full including the extension"),
        AP_INIT_TAKE1("zimPath", (const char* (*)())kiwix_set_path, NULL, RSRC_CONF, "The path to the ZIM file, including the trailing //"),
        { NULL }
    };
}

extern "C" module kiwix_module;

module AP_MODULE_DECLARE_DATA kiwix_module = {
    STANDARD20_MODULE_STUFF, NULL, NULL, NULL, NULL, kiwix_settings, register_hooks
};

static void register_hooks(apr_pool_t *pool) {
    ap_hook_handler(kiwix_handler, NULL, NULL, APR_HOOK_LAST);
    config.path = "/var/www/html";
    config.zimfilename = "wikipedia_for_schools.zim";
}

static int kiwix_handler(request_rec *r) {
    std::string const& prefix = "kiwix";
    std::string const& split_on = prefix + "/";

    if (!r->handler || strcmp(r->handler, prefix.c_str()))
        return (DECLINED);

    kiwix::Reader *reader = NULL;
    string zimpath = string(config.path) + string(config.zimfilename);
    ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE, 0, r->server, " ZIM file including path: %s", zimpath.c_str());
    string urlStr = "";

    bool found = false;

    zim::Article article;
    string fullUrl(r->uri);
    string url = fullUrl.substr(fullUrl.find(split_on) + string(split_on).length());
    ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE, 0, r->server,
                 "full url = %s, kiwix url = %s", fullUrl.c_str(), url.c_str());
    if (url.compare("status") == 0) {
        // The user is requesting the status page
        ap_set_content_type(r, "text/plain");
        ap_rputs("I'm OK, thank you, and you?\n", r);
        return OK;
    }
    if (url.compare("config") == 0) {
        // The user is requesting the configuration
        ap_set_content_type(r, "text/plain");
        ap_rputs("I wish I knew\n\nI'll ask\n", r);
        ap_rprintf(r, "Path: %s\n", config.path);
        ap_rprintf(r, "ZIM filename: %s\n", config.zimfilename);
        return OK;
    }
    try {
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
                apr_table_setn(r->headers_out, "Location", apr_pstrdup(r->pool, article.getLongUrl().c_str()));
                delete reader;
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
                if (contentType.find("text/") != std::string::npos ||
                        contentType.find("application/javascript") != std::string::npos ||
                        contentType.find("application/json") != std::string::npos) {

                    cleanUp(contentType, prefix, article, content);
                    ap_rputs(content.c_str(), r);
                } else {
                    ap_rwrite(article.getData().data(), article.getArticleSize(), r);
                }
            }
        }
        else {
            // This is useful for debugging and can probably be removed as the code matures.
            zim::File f(zimpath);
            ap_set_content_type(r, "text/plain");
            ap_rputs("Element not found, the ZIM file contains\n", r);
            for(zim::File::const_iterator it = f.begin(); it != f.end(); ++it) {
                ap_rputs((it->getMimeType() + " - " + it->getLongUrl() + " - " + it->getTitle() + "--\n").c_str(), r);
            }
        }
        delete reader; 
    }
    catch (const std::exception& e) {
        ap_rputs(e.what(), r);
    }

    return OK;
}

