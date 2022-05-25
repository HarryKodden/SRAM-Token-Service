#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include "logging.h"
#include "validate.h"
#include "api.h"
#include "cache.h"

#include "../json-parser/json.h"

static json_value *lookup(json_value* value, const char* name) {
	json_value * result = NULL;

	if (value == NULL) {
		return NULL;
	}

	char *remaining, *current = strdup(name);

	if ((remaining=strchr(current, '.')) != NULL) {
		*remaining++ = '\0';
	}

	for (unsigned int x = 0; x < value->u.object.length; x++) {
		if (!strcmp(value->u.object.values[x].name, current)) {

			result = (remaining ? lookup(value->u.object.values[x].value, remaining) : value->u.object.values[x].value);

			break;
		}
	}

	free(current);

	return result;
}

static char *str_printf(const char * fmt, ...) {
	char *buffer = NULL;
	va_list args;
	
	va_start(args, fmt);
	int rc = vasprintf(&buffer, fmt, args);
	va_end(args);

	if (rc == -1) {
		logging(LOG_ERR, "Error create string with fmt: %s", fmt);
		return NULL;
	}
	return buffer;
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
		! (url = str_printf("%s/api/tokens/introspect", cfg->url)) ||
		! (auth = str_printf("Authorization: Bearer %s", cfg->token)) ||
		! (data = str_printf("token=%s", token))
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
					result = (strcasecmp(username, user->u.string.ptr) == 0);

					if (!result) {
						logging(LOG_ERR, "Username %s != %s\n", username, user->u.string.ptr);
					}

					if (result && cfg->entitled) {
						json_value *entitledments = lookup(json, "user.eduperson_entitlement ");

						result = false;
						if (entitledments) {
							for (unsigned int x = 0; !result && x < entitledments->u.array.length; x++) {
								if (entitledments->u.array.values[x]) {
									char *entitledment = entitledments->u.array.values[x]->u.string.ptr;
									logging(LOG_DEBUG, "Inspecting: %s...\n", entitledment);
									result = (strcasecmp(cfg->entitled, entitledment) == 0);
									if (result) {
										logging(LOG_DEBUG, "Match: %s !\n", entitledment);
									}
								}
							}
						} else {
							logging(LOG_ERR, "Error inspecting entitlements\n");
						}
					}

					if (result) {
						long expiration = MIN(exp->u.integer, (time(NULL)+(atoi(cfg->ttl) * 60)));
						cache_remember(cfg, username, token, expiration);
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
