install-exec-local:
	sudo mkdir -p /lib/security
	sudo ln -sf /usr/local/bin/pam_sram_validate.so /lib/security/


install-data-local:
	@echo "auth required pam_sram_validate.so debug url=$${URL} token=$${TOKEN}" redis=redis entitled=$${ENTITLED} | sudo tee "/etc/pam.d/test" >/dev/null

test:
	pamtester test $${USERNAME} authenticate

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = pam_sram_validate.so
pam_sram_validate_so_SOURCES = pam_sram_validate.c validate.c api.c config.c logging.c cache.cpp digest.cpp ../json-parser/json.c
pam_sram_validate_so_CFLAGS = -fPIC -O0 -ggdb3 -std=c11 -Wall -Wextra -Wshadow -Wpedantic -Wuninitialized -Wformat=2 -Werror -fstack-protector-strong
pam_sram_validate_so_LDFLAGS = -shared -fPIC
