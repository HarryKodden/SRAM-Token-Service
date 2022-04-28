## PAM_SRAM_VALIDATE

...

Harry Kodden

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
sudo apt-get install libcrypto++-dev
sudo apt-get install libssl-dev

~~~

On Fedora/Redhat/CentOS you will need:
~~~
sudo yum groupinstall 'Development Tools'
sudo yum install shtool
sudo yum install pam-devel
sudo yum install libcurl curl-devel
sudo yum install hiredis-devel
~~~

After cloning the repository, do the following:

~~~
cd pam_sram_validate
ln -s /usr/bin/shtool .
autoconf
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

The file /etc/pam.d/irods would then look like:

~~~
#%PAM-1.0
auth      sufficient     pam_unix.so
auth      sufficient     pam_sram_validate.so <...TBC...>
~~~

Configuration settings:

Config | Meaning | Example
--- | --- | ---
... | ... | ...

