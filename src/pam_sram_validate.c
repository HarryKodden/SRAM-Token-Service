/* Copyright (C) 2022 Harry Kodden
 */

#include "defs.h"

#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <security/pam_ext.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include "pam_sram_validate.h"

int pam_sm_open_session(UNUSED pam_handle_t *pamh, UNUSED int flags, UNUSED int argc, UNUSED const char **argv) {
    return (PAM_SUCCESS);
}

int pam_sm_close_session(UNUSED pam_handle_t *pamh, UNUSED int flags, UNUSED int argc, UNUSED const char **argv) {
    return (PAM_SUCCESS);
}

int pam_sm_acct_mgmt(UNUSED pam_handle_t *pamh, UNUSED int flags, UNUSED int argc, UNUSED const char **argv) {
    return (PAM_SUCCESS);
}

int pam_sm_authenticate(pam_handle_t *pamh, UNUSED int flags, int argc, const char **argv) {
    char *user = NULL;
    char *token = NULL;

    logging_pamh = pamh;

    CONFIG *cfg = parse_config(argc, argv);

    if (!cfg) {
        logging(LOG_ERR, "configuration invalid");
        return PAM_AUTH_ERR;
    }

    (void) pam_get_user(pamh, (const char **) &user, "User: ");

    if (!user) {
        free_config(cfg);
        return PAM_USER_UNKNOWN;
    }

    (void) pam_get_authtok(pamh, PAM_AUTHTOK, (const char **) &token, "Token: ");

    if (!token) {
        free_config(cfg);
        return PAM_AUTH_ERR;
    }

    int result = validate(cfg, user, token) ? PAM_SUCCESS : PAM_AUTH_ERR;

    free_config(cfg);
    
    return result;
}


int pam_sm_setcred(UNUSED pam_handle_t *pamh, UNUSED int flags, UNUSED int argc, UNUSED const char **argv) {
    return (PAM_SUCCESS);
}

int pam_sm_chauthtok(UNUSED pam_handle_t *pamh, UNUSED int flags,UNUSED  int argc, UNUSED const char **argv) {
    return (PAM_SUCCESS);
}
