#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include "logging.h"
#include "validate.h"
#include "api.h"
#include "cache.h"
#include "json.h"

json_value *lookup(json_value* value, const char* name) {
	json_value * result = NULL;

	if (value == NULL) {
		return NULL;
	}

	char *remaining, *current = strdup(name);

	if ((remaining=strchr(current, '.')) != NULL) {
		*remaining++ = '\0';
	}

	for (int x = 0; x < value->u.object.length; x++) {
		if (!strcmp(value->u.object.values[x].name, current)) {

			result = (remaining ? lookup(value->u.object.values[x].value, remaining) : value->u.object.values[x].value);

			break;
		}
	}

	free(current);

	return result;
}

bool validate(CONFIG *cfg, const char *username, const char *token) {
	char *url =  NULL;
	char *auth = NULL;
	char *data = NULL;
	bool result = false;

	if (cache_validate(cfg, username, token)) {
		logging(LOG_INFO, "Matched from cache !\n");
		return true;
	}

	if (
			(asprintf(&url, "%s/api/tokens/introspect", cfg->url) == -1) ||
			(asprintf(&auth, "Authorization: Bearer %s", cfg->token) == -1) ||
			(asprintf(&data, "token=%s", token) == -1)
		) {

		logging(LOG_ERR, "Error allocating memory\n");
		goto finalize;
	}

	char *response = api(
		url, "POST", 
		(char *[]){ "Content-Type: application/x-www-form-urlencoded", auth, NULL}, 
		data
	);

	if (response) {
		json_value * json = json_parse(response, strnlen(response,BUFSIZ));
		free(response);

		if (json) {
			json_value *active = lookup(json, "active");

			if (active && active->u.boolean) {
				json_value *user = lookup(json, "username");
				json_value *exp = lookup(json, "exp");

				if (user && exp) {
					result = (strncasecmp(username, user->u.string.ptr, 32) == 0);
					
					long expiration = MIN(exp->u.integer, (time(NULL)+(atoi(cfg->ttl) * 60)));

					if (result) {
						cache_remember(cfg, username, token, expiration);
					} else {
						logging(LOG_ERR, "Username %s != %s\n", username, user->u.string.ptr);
					}
				} else {
					logging(LOG_ERR, "Username field not found\n");
				}
			} else {
				logging(LOG_ERR, "Token not active\n");
			}

			json_value_free(json);
		} else {
			logging(LOG_ERR, "Response could not parsed as (valid) json...\n", response);
		}
	} else {
		logging(LOG_ERR, "No response received...\n");
	}

finalize:
	free(url);
	free(auth);
	free(data);

	return result;
}
