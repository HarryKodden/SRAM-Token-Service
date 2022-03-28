#!/usr/bin/python2

# Workaround for CVE-2019-16729
# https://sourceforge.net/p/pam-python/tickets/8/
import site
site.main()

import syslog
import traceback
import sys

sys.path.append('/usr/local/lib/python2.7/dist-packages')

import requests

DEFAULT_USER  = "nobody"
DEBUGGING = True

def logging(facility, message):
    if (facility != syslog.LOG_DEBUG) or DEBUGGING:
        syslog.openlog(facility=facility)
        syslog.syslog(facility, message)
        syslog.closelog()

import hashlib
import redis
import json

import datetime

class Cache(object):

    def __init__(self, config):
        try:
            self.redis = redis.Redis(
                host=config.get('REDIS_HOST', 'localhost'),
                port=int(config.get('REDIS_PORT', '6379')),
                db=0
            )
            for key in self.redis.scan_iter("*"):
                idle = self.redis.object("idletime", key)
                if idle > int(config.get('REDIS_MAX_IDLETIME', '3600')):
                    self.redis.delete(key)
        except Exception as e:
            logging(syslog.LOG_ERR, "Error initializing redis...{}".format(str(e)))
            self.redis = None

    def digest(self, data):
        h = hashlib.new('sha512')
        h.update(bytes(data))
        return h.hexdigest()

    def key(self, url, headers):
        return self.digest(
            json.dumps(
                {
                    'url': url,
                    'headers': headers
                }
            )
        )

    def val(self, payload):
        return self.digest(payload)

    def remember(self, payload, url, headers={}):
        if self.redis:
            self.redis.set(self.key(url, headers), self.val(payload))

    def validate(self, payload, url, headers={}):
        if not self.redis:
            return False

        key = self.key(url, headers)
        if not key:
            return False

        val = self.redis.get(key)
        if val and val == self.val(payload):
            return True

        self.redis.delete(key)


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

    start = datetime.datetime.now()

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

        cache = Cache(config)

        payload = "token={}".format(pamh.authtok)
        headers = {
            "Authorization": "Bearer {}".format(config.get('token','missing...')),
            "Content-Type": "application/x-www-form-urlencoded"
        }

        if cache.validate(payload, url, headers=headers):
            rval = pamh.PAM_SUCCESS
        else:
            response = requests.request("POST", url, data=payload, headers=headers, verify=sslverify)

            logging(syslog.LOG_DEBUG, response.text)

            if (response.status_code == 200):
                data = response.json()
                if data["username"].upper() == username.upper():
                    cache.remember(payload, url, headers=headers)
                    rval = pamh.PAM_SUCCESS
                else:
                    logging(syslog.LOG_ERR, "Username does not match.")
            else:
                logging(syslog.LOG_ERR, "{} returns: {}".format(url, response.status_code))

    except requests.exceptions.SSLError:
        logging(syslog.LOG_CRIT, "%s: SSL Validation error. Get a valid "
                                 "SSL certificate, For testing you can use the "
                                 "options 'nosslverify'." % __name__)

    except Exception as e:
        logging(syslog.LOG_ERR, traceback.format_exc())
        logging(syslog.LOG_ERR, "%s: %s" % (__name__, e))



    duration = datetime.datetime.now() - start
    logging(syslog.LOG_DEBUG, "Duration: {} microsecond".format(duration.microseconds))

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
