#ifndef _CACHE_H
#define _CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

extern void cache_remember(CONFIG *, const char *, const char *, unsigned long);
extern bool cache_validate(CONFIG *, const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif // _CACHE_H_