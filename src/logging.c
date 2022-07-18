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
        #ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif
        pam_vsyslog(logging_pamh, priority, fmt, l);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        va_end(l);
    }
}
