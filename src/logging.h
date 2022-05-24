
#ifndef _LOGGING_H
#define _LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <syslog.h>
#include <security/pam_ext.h>

extern int logging_priority;
extern pam_handle_t *logging_pamh;

void logging(int priority, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // _LOGGING_H