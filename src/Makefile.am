install: pam_sram_validate.so
	sudo cp pam_sram_validate.so /lib/x86_64-linux-gnu/security/

test: install
	echo "auth required pam_sram_validate.so debug url=https://sram.surf.nl token=$${TOKEN}" redis=redis | sudo tee "/etc/pam.d/test"
	pamtester test $${USERNAME} authenticate

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = pam_sram_validate.so
pam_sram_validate_so_SOURCES = pam_sram_validate.c validate.c api.c config.c logging.c cache.cpp digest.cpp ../json-parser/json.c
pam_sram_validate_so_CFLAGS = -fPIC -O0 -ggdb3 -std=c11 -Wall -Wextra -Wshadow -Wpedantic -Wuninitialized -Wformat=2 -Werror -fstack-protector-strong
pam_sram_validate_so_LDFLAGS= -shared -fPIC