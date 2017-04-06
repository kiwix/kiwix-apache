#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

#include <zim/file.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <kiwix/reader.h>

static void register_hooks(apr_pool_t *pool);
static int example_handler(request_rec *r);

extern "C" module example_module;

module AP_MODULE_DECLARE_DATA example_module = {
	STANDARD20_MODULE_STUFF, NULL, NULL, NULL, NULL, NULL, register_hooks
};

static void register_hooks(apr_pool_t *pool) {
	ap_hook_handler(example_handler, NULL, NULL, APR_HOOK_LAST);
}

static int example_handler(request_rec *r) {
	if (!r->handler || strcmp(r->handler, "example"))
		return (DECLINED);
	
	kiwix::Reader *reader = NULL;
	string zimpath = "/var/www/html/wikipedia.zim";
	string urlStr = "";
	bool found = false;

	zim::Article article;
	
	try {
		zim::File f(zimpath);
		reader = new kiwix::Reader(zimpath);
		found = reader->getArticleObjectByDecodedUrl(urlStr, article);
		if (found) {
			ap_set_content_type(r, (article.getMimeType()).c_str());
			string content = string(article.getData().data(), article.getArticleSize()).c_str();
			content = "<html><head><title>" + article.getTitle() + "</title></head><body>" + content + "</body></html>"; 
			ap_rputs(content.c_str(), r);
		}
		else {
			ap_set_content_type(r, "text/plain");
			for(zim::File::const_iterator it = f.begin(); it != f.end(); ++it) {
				ap_rputs((it->getTitle() + "--\n").c_str(), r);
			}
		}
		reader = NULL;
	}
	catch (const std::exception& e) {
		ap_rputs(e.what(), r);
	}

	return OK;
}

