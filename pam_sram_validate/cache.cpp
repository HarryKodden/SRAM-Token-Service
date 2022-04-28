#include <cstdio>
#include <iostream>
#include <functional>
#include <string>
#include <stdbool.h>

#include "cache.h"
#include "logging.h"
#include "json.h"

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

        void remember(char *key, char *val) {
            if (! redis)
                return;

            logging(LOG_ERR, "Remembering: %s...\n", key);

            void *reply = redisCommand(redis, "SET %s %s", key, val);
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

            bool result = (strcmp(reply->str, val) == 0);
            freeReplyObject(reply);
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
    void cache_remember(CONFIG *cfg, char *key, char *val) {
        Cache *c = new Cache(cfg);
        c->remember(key, val);
        delete c;
    }

    bool cache_validate(CONFIG *cfg, char *key, char *val) {
        Cache *c = new Cache(cfg);
        bool result = c->validate(key, val);
        delete c;
        return result;
    }
}