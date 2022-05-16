AC_PREREQ([2.71])
AC_INIT([pam_sram_validate],[0.1],[BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign]) 

# Check whether a C++ was found (AC_PROG_CXX sets $CXX to "g++" even when it
# doesn't exist)
AC_LANG_PUSH([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],[],[AC_MSG_ERROR([No C++ compiler found])])
AC_LANG_POP([C])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h syslog.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset pow strdup])

dnl Check for all required headers
AC_CHECK_HEADERS(security/pam_appl.h security/pam_modules.h hiredis/hiredis.h curl/curl.h openssl/sha.h,, [
        AC_MSG_WARN(Required header missing, compilation will likely fail.)
], [
#ifdef HAVE_SECURITY_PAM_APPL_H
#  include <security/pam_appl.h>
#endif
#ifdef HAVE_SECURITY_PAM_MODULES_H
#  include <security/pam_modules.h>
#endif
])

# Checks for libraries.

dnl Check for PAM libraries
AC_CHECK_LIB(pam, pam_get_user)

dnl Check for REDIS libraries
AC_CHECK_LIB(hiredis, redisCommand)

dnl Check for REDIS libraries
AC_CHECK_LIB(curl, curl_easy_getinfo)

dnl Check for MATH libraries
AC_CHECK_LIB(m, pow)

dnl Check for CRYPTO libraries
AC_CHECK_LIB(crypto, main, , [AC_MSG_ERROR("Linking against crypto library failed")])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
