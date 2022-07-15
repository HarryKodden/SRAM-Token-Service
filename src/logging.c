#include "defs.h"

#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

int logging_priority = LOG_ERR;
pam_handle_t *logging_pamh = NULL;

void logging(int priority, const char *fmt, ...) {

    if (logging_pamh && (logging_priority >= priority)) {
        va_list l;

        va_start(l, fmt);
        pam_vsyslog(logging_pamh, priority, fmt, l);
        va_end(l);
    }
}
