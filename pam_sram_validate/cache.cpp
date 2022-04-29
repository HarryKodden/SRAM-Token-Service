#include <cstdio>
#include <iostream>
#include <functional>
#include <string>
#include <stdbool.h>
#include <time.h>

#include "cache.h"
#include "logging.h"
#include "json.h"
#include "digest.h"

#include <hiredis/hiredis.h>

class Cache {
	public:
		Cache(CONFIG *cfg) {

			if (! cfg->redis) {
				logging(LOG_ERR, "Redis not configured !\n");
				return;
			}

			struct timeval timeout = { 1, 500000 }; // 1.5 seconds
			redis = redisConnectWithTimeout(cfg->redis, atoi(cfg->port), timeout);

			if (redis == NULL || redis->err) {
				if (redis) {
					logging(LOG_ERR, "Redis Connection error: %s\n", redis->errstr);
					redisFree(redis);
				} else {
					logging(LOG_ERR, "Redis Connection error: can't allocate redis context\n");
				}
			}
		}

		~Cache() {
			if (redis)
				redisFree(redis);
		}

		void remember(char *key, char *val, long exp) {
			if (! redis)
				return;

			logging(LOG_ERR, "Remembering: %s...\n", key);

			void *reply = redisCommand(redis, "SET %s %ld:%s", key, exp, val);
			freeReplyObject(reply);
		}

		bool validate(char *key, char *val) {
			if (! redis)
				return false;

			redisReply *reply = (redisReply *)redisCommand(redis ,"GET %s", key);
			if (!reply) {
				return false;
			}

			if (!reply->str) {
				logging(LOG_INFO, "Cache key not found: %s\n", key);
				freeReplyObject(reply);
				return false;
			}

			char *cached_val, *expiration = strdup(reply->str);
			freeReplyObject(reply);
			
			if ((cached_val = strchr(expiration, ':')) != NULL)
				*cached_val++ = 0;
			else {
				free(expiration);
				return false;
			}

			bool result = ((time(NULL) < atoi(expiration)) && (strcmp(cached_val, val) == 0));

			free(expiration);
			
			if (result) {
				logging(LOG_INFO, "Cache Hit for: %s\n", key);
			} else {
				logging(LOG_INFO, "Dropping cache for: %s\n", key);
				void *reply = redisCommand(redis, "DEL %s", key);
				freeReplyObject(reply);
			}

			return result;
		}

	private:
		redisContext *redis = NULL;
};

extern "C"
{
	static char *key_digest(CONFIG *cfg) {
		const char *items[3] = { cfg->url, cfg->token, NULL };

		return digest(items);
	}

	static char *val_digest(const char *secret) {
		const char *items[2] = { secret, NULL };

		return digest(items);
	}

	void cache_remember(CONFIG *cfg, const char *secret, long exp) {
		Cache *c = new Cache(cfg);

		char *key = key_digest(cfg);
		char *val = val_digest(secret);

		c->remember(key, val, exp);
		
		free(key);
		free(val);

		delete c;
	}

	bool cache_validate(CONFIG *cfg, const char *secret) {
		Cache *c = new Cache(cfg);

		char *key = key_digest(cfg);
		char *val = val_digest(secret);

		bool result = c->validate(key, val);

		free(key);
		free(val);

		delete c;
		
		return result;
	}
}