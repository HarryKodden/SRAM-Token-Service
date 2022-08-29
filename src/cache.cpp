#include <cstdio>
#include <iostream>
#include <functional>
#include <string>
#include <cstring>

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "cache.h"
#include "logging.h"
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
			long port = strtol(cfg->port, NULL, 10);
			
			redis = redisConnectWithTimeout(cfg->redis, port, timeout);

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

		void remember(CONFIG *cfg, const char *username, const char *secret, long expiration) {
			if (! redis)
				return;

			char *key = key_digest(cfg, username);
			char *val = val_digest(secret);

			logging(LOG_ERR, "Remembering: %s...\n", key);

			void *reply = redisCommand(redis, "SET %s %ld:%s", key, expiration, val);
			freeReplyObject(reply);

			free(key);
			free(val);
		}

		bool validate(CONFIG *cfg, const char *username, const char *secret) {
			if (! redis)
				return false;

			char *key = key_digest(cfg, username);

			redisReply *reply = (redisReply *)redisCommand(redis ,"GET %s", key);
			if (!reply) {
				free(key);
				return false;
			}

			if (!reply->str) {
				logging(LOG_INFO, "Cache key not found: %s\n", key);
				freeReplyObject(reply);
				free(key);
				return false;
			}

			char *cached_val, *expiration = strdup(reply->str);
			freeReplyObject(reply);
			
			if ((cached_val = strchr(expiration, ':')) != NULL)
				*cached_val++ = 0;
			else {
				free(key);
				free(expiration);
				return false;
			}

			char *val = val_digest(secret);
			bool result = ((time(NULL) < atoi(expiration)) && (strcmp(cached_val, val) == 0));
			free(val);
			free(expiration);
			
			if (result) {
				logging(LOG_INFO, "Cache Hit for: %s\n", key);
			} else {
				logging(LOG_INFO, "Dropping cache for: %s\n", key);
				void *r = redisCommand(redis, "DEL %s", key);
				freeReplyObject(r);
			}

			free(key);

			return result;
		}

	private:
		redisContext *redis = NULL;
		
		char *key_digest(CONFIG *cfg, const char *username) {
			const char *items[] = { cfg->url, cfg->token, username, cfg->entitled, NULL };
			return digest(items);
		}

		char *val_digest(const char *secret) {
			const char *items[] = { secret, NULL };
			return digest(items);
		}
};

extern "C"
{
	void cache_remember(CONFIG *cfg, const char *username, const char *secret, unsigned long expiration) {
		Cache *c = new Cache(cfg);

		c->remember(cfg, username, secret, expiration);
	
		delete c;
	}

	bool cache_validate(CONFIG *cfg, const char *username, const char *secret) {
		Cache *c = new Cache(cfg);

		bool result = c->validate(cfg, username, secret);
		delete c;
		
		return result;
	}
}