#ifndef _VALIDATE_H
#define _VALIDATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "config.h"

bool validate(CONFIG *cfg, const char *username, const char *token);

#ifdef __cplusplus
}
#endif

#endif // _VALIDATE_H
