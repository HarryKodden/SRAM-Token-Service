
#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include <curl/curl.h>

#include "logging.h"
#include "api.h"

typedef struct {
	char* payload;
	size_t size;
} CURL_FETCH;

size_t curl_callback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;         /* calculate buffer size */
	CURL_FETCH *p = (CURL_FETCH *) userp;   /* cast pointer to fetch struct */

	/* expand buffer */
	p->payload = (char*)realloc(p->payload, p->size + realsize + 1);

	/* check buffer */
	if (p->payload == NULL) {
		/* this isn't good */
		logging(LOG_ERR, "Failed to expand buffer in curl_callback");

		/* free buffer */
		free(p->payload);

		/* return */
		return (size_t) -1;
	}

	/* copy contents to buffer */
	memcpy(p->payload+p->size, contents, realsize);

	/* set new buffer size */
	p->size += realsize;

	/* ensure null termination */
	p->payload[p->size] = 0;

	/* return size */
	return realsize;
}

/*
 * API to URL url and returns the result
 */
char *api(const char* url, const char *method, char *headers[], const char* data) {
	CURL* curl;
	CURL_FETCH fetcher;

	// Prepare Headers...
	struct curl_slist* request_headers = NULL;

	for (int i=0; headers[i]; i++) {
		request_headers = curl_slist_append(request_headers, headers[i]);
	}
	
	// Prepare payload for response...
	fetcher.payload = (char*)calloc(1, sizeof(fetcher.payload));
	fetcher.size = 0;

	// Prepare API request...
	curl = curl_easy_init();
	if (curl) {
		long response_code;

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&fetcher);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, PACKAGE_NAME);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data ? strnlen(data,1024) : 0);

		// Perform the request
		if (curl_easy_perform(curl) != CURLE_OK) {
			return NULL;
		}

		// Check response
		if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code) != CURLE_OK || response_code != 200) {
			logging(LOG_ERR, "Request to %s --> %d", url, response_code);
			return NULL;
		}

		// always cleanup
		curl_easy_cleanup(curl);
	}

	return fetcher.payload;
}
