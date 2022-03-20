#!/usr/bin/python3

# Workaround for CVE-2019-16729
# https://sourceforge.net/p/pam-python/tickets/8/
import site
site.main()

import syslog
import traceback
import requests

DEFAULT_USER  = "nobody"
DEBUGGING = True

def logging(facility, message):
    if (facility != syslog.LOG_DEBUG) or DEBUGGING:
        syslog.openlog(facility=facility)
        syslog.syslog(facility, message)
        syslog.closelog()


def get_config(argv):
    """
    Read the parameters from the arguments. If the argument can be split with a
    "=", the parameter will get the given value.
    :param argv:
    :return: dictionary with the parameters
    """
    config = {}
    for arg in argv:
        argument = arg.split("=")
        if len(argument) == 1:
            config[argument[0]] = True
        elif len(argument) == 2:
            config[argument[0]] = argument[1]
    return config

def pam_sm_authenticate(pamh, flags, argv):

    config = get_config(argv)
    
    global DEBUGGING
    DEBUGGING = (config.get("debug", None) != None)

    logging(syslog.LOG_DEBUG, "Starting PAM authentication...{}".format(config))

    username = pamh.get_user(None) or DEFAULT_USER

    prompt = config.get("prompt", "SRAM Token")
    if prompt[-1] != ":":
        prompt += ":"

    url = "{}/api/tokens/introspect".format(
        config.get("url", "http://localhost:8080")
    )

    sslverify = not config.get("nosslverify", False)
    cacerts = config.get("cacerts")
    if sslverify and cacerts:
        sslverify = cacerts
    
    logging(syslog.LOG_DEBUG, "URL: {}".format(url))

    rval = pamh.PAM_AUTH_ERR

    try:
        if pamh.authtok is None:
            message = pamh.Message(pamh.PAM_PROMPT_ECHO_OFF, "%s " % prompt)
            response = pamh.conversation(message)
            pamh.authtok = response.resp

        payload = "token={}".format(pamh.authtok)
        headers = {
            "Authorization": "Bearer {}".format(config.get('token','missing...')),
            "Content-Type": "application/x-www-form-urlencoded"
        }

        response = requests.request("POST", url, data=payload, headers=headers, verify=sslverify)

        logging(syslog.LOG_DEBUG, response.text)

        if (response.status_code == 200):
            data = response.json()
            if data["username"].upper() == username.upper()
                rval = pamh.PAM_SUCCESS
            else:
                logging(syslog.LOG_ERR, "Username does not match.")
        else:
            logging(syslog.LOG_ERR, "{} returns: {}".format(url, response.status_code))

    except Exception as e:
        logging(syslog.LOG_ERR, traceback.format_exc())
        logging(syslog.LOG_ERR, "%s: %s" % (__name__, e))

    except requests.exceptions.SSLError:
        logging(syslog.LOG_CRIT, "%s: SSL Validation error. Get a valid "
                                 "SSL certificate, For testing you can use the "
                                 "options 'nosslverify'." % __name__)

    return rval

def pam_sm_setcred(pamh, flags, argv):
  return pamh.PAM_SUCCESS

def pam_sm_acct_mgmt(pamh, flags, argv):
  return pamh.PAM_SUCCESS

def pam_sm_open_session(pamh, flags, argv):
  return pamh.PAM_SUCCESS

def pam_sm_close_session(pamh, flags, argv):
  return pamh.PAM_SUCCESS

def pam_sm_chauthtok(pamh, flags, argv):
  return pamh.PAM_SUCCESS
