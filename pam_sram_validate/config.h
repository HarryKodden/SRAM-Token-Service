
#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *url;
    char *token;
    char *redis;
    char *port;
    char *ttl;
} CONFIG;

void free_config(CONFIG *);
CONFIG *parse_config(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif // _CONFIG_H