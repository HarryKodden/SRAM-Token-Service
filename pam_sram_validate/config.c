#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "logging.h"

void free_config(CONFIG *cfg) {
    if (cfg) {
        free(cfg->url);
        free(cfg->token);
        free(cfg->redis);
        free(cfg->port);
        free(cfg->ttl);
        free(cfg);
    }
}

#define DEFAULT_REDIS_PORT "6379"
#define DEFAULT_TTL "5"

/**
 * Handles the basic parsing of a given option.
 * @arg buf is the buffer to be parsed
 * @arg opt_name_with_eq is the option name we are looking for (including equal sign)
 * Note that dst has to be freed by the caller in case of 0 return code
 * returns 0 if the option was not found
 * returns -1 if an error occured (duplicate option)
 * returns the position of the start of the value in the buffer otherwise
 */
int raw_parse_option(const char* buf, const char* opt_name_with_eq, char** dst) {
    size_t opt_len = strlen(opt_name_with_eq);
    if (0 == strncmp(buf, opt_name_with_eq, opt_len)) {
        if (dst && *dst) {
            logging(LOG_ERR,
                       "Duplicated option : %s. Only first one is taken into account",
                       opt_name_with_eq);
            return -1;
        } else {
            return (int)opt_len;
        }
    }
    return 0;
}

/// calls strdup and returns whether we had a memory error
int strdup_or_die(char** dst, const char* src) {
    *dst = strdup(src);
    return *dst ? 0 : -1;
}

/**
 * Handles the parsing of a given option.
 * @arg buf is the buffer to be parsed
 * @arg opt_name_with_eq is the option name we are looking for (including equal sign)
 * @arg dst is the destination buffer for the value found if any.
 * Note that dst has to be freed by the caller in case of 0 return code
 * returns 0 if the option was not found in the buffer
 * returns 1 if the option was found in buffer and parsed properly
 * returns -1 in case of error
 */
int parse_str_option(const char* buf, const char* opt_name_with_eq, char** dst) {
    int value_pos = raw_parse_option(buf, opt_name_with_eq, dst);
    if (value_pos > 0) {
        if (strdup_or_die(dst, buf + value_pos)) {
            return -1;
        }
        return 1;
    } else if (value_pos == -1) {
        // Don't crash on duplicate, ignore 2nd value
        return 1;
    }
    return value_pos;
}

CONFIG *parse_config(int argc, const char **argv) {
    int mem_error = 0;
    int i;

    CONFIG *cfg = (CONFIG *) calloc(1, sizeof(CONFIG));
    if (!cfg) {
        logging(LOG_CRIT, "Out of memory");
        return NULL;
    }

    cfg->url = NULL;
    cfg->token = NULL;
    cfg->ttl = NULL;
    cfg->redis = NULL;
    cfg->port = NULL;

    for (i = 0; i < argc; ++i) {
        int retval = 0;

        if (!strcmp(argv[i], "debug")) {
            logging_priority = LOG_DEBUG;
            continue;
        }

        if (retval == 0) retval = parse_str_option(argv[i], "url=", &cfg->url);
        if (retval == 0) retval = parse_str_option(argv[i], "token=", &cfg->token);
        if (retval == 0) retval = parse_str_option(argv[i], "ttl=", &cfg->ttl);
        if (retval == 0) retval = parse_str_option(argv[i], "redis=", &cfg->redis);
        if (retval == 0) retval = parse_str_option(argv[i], "port=", &cfg->port);

        if (0 == retval) {
            logging(LOG_ERR, "Invalid option: %s", argv[i]);
            free_config(cfg);
            return NULL;
        } else if (retval < 0) {
            mem_error = retval;
            break;
        }
    }

    // in case we got a memory error in the previous code, give up immediately
    if (mem_error) {
        logging(LOG_CRIT, "Out of memory");
        free_config(cfg);
        return NULL;
    }

    if (cfg->redis && ! cfg->port) {
       logging(LOG_DEBUG, "Setting default value for redis 'port' (%s)", DEFAULT_REDIS_PORT);
        cfg->port = strdup(DEFAULT_REDIS_PORT);
    }

    if (! cfg->ttl) {
       logging(LOG_DEBUG, "Setting default value for 'ttl' (%s)", DEFAULT_TTL);
       cfg->ttl = strdup(DEFAULT_TTL);
    }

    logging(LOG_DEBUG, "url => %s", cfg->url);
    logging(LOG_DEBUG, "token => %s", cfg->token);
    if (cfg->redis) {
        logging(LOG_DEBUG, "redis => %s:%s", cfg->redis, cfg->port);
    }
    logging(LOG_DEBUG, "ttl => %s", cfg->ttl);

    return cfg;
}
