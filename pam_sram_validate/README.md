## PAM_SRAM_VALIDATE

![CodeQL](https://github.com/harrykodden/SRAM-Token-Service/workflows/CodeQL/badge.svg)
[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://github.com/HarryKodden/SRAM-Token-Service/)

This module provides PAM authentication of a user offering a SRAM User ID and a SRAM User token.
The PAM module will initiate a token introspection request towards SRAM asking if that user supplied token is a valid token for usage with a given SRAM Service. The Service inspected has to be indicated with a Service Bearer token in the PAM Module configuration parameters. 

Since this PAM module might be used in combination with stateless applications, for example WebDAV server authentications, this PAM module might be invoked many times sequentially. For these use-cases a cache mechanism is offered as an optimization option. Using a 'redis' cache provides successfull authentication if subsequent invocations occur within a configurable sequences of seconds (ttl) following a previous successful authentication.

# Install

Requirement for building the pam module is that the basic development tools are installed on your system. This can be achieved by installing the following:

On Debian/Ubuntu:
~~~
sudo apt-get install build-essential
sudo apt-get install autoconf
sudo apt-get install shtool
sudo apt-get install libpam-dev
sudo apt-get install libcurl4-gnutls-dev
sudo apt-get install libhiredis-dev
sudo apt-get install libssl-dev

~~~

On Fedora/Redhat/CentOS you will need:
~~~
sudo yum groupinstall 'Development Tools'
sudo yum install shtool
sudo yum install pam-devel
sudo yum install libcurl curl-devel
sudo yum install hiredis-devel
sudo yum install openssl-devel
~~~

After cloning the repository, do the following:

~~~
cd pam_sram_validate
./autogen.sh
./configure
make
sudo make install
~~~

The pam module will be install under /usr/local/lib/security. In the PAM config file, you should explicity point to this path. Alternatively you can make a symbolic link to that default PAM location.

For example:

~~~
ln -s /usr/local/lib/security/pam_sram_validate.so /lib/security
~~~


# Sample Usage

Let:
- The pam_sram_validate.so module is installed in /usr/local/lib/security

The file /etc/pam.d/sample would then look like:

~~~
#%PAM-1.0
auth      sufficient     pam_unix.so
auth      sufficient     pam_sram_validate.so debug redis=redis token=... url=https://sram.surf.nl
~~~

Configuration settings:

Config | Meaning | Example
--- | --- | ---
debug | extra logging in /var/log/auth | ... debug
redis | hostname of redis server to cache successful authentications | redis=localhost
port | port of redis server, defaults to 6379 | port=1234
ttl | duration that cached results are remain valid for subsequent authentications, defaults to 5 seconds | ttl=5
url | fqdn of SRAM server | url=https://sram.surf.nl
token | the SRAM token for the SRAM Service to be uses for token introspection | token=...



