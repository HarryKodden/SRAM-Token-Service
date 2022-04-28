install: pam_sram_validate.so
	sudo cp pam_sram_validate.so /lib/x86_64-linux-gnu/security/

test: install
	echo "auth required pam_sram_validate.so debug url=https://sram.surf.nl token=..." | sudo tee "/etc/pam.d/test"
	pamtester test hkodden5 authenticate

#	echo "auth required /lib/x86_64-linux-gnu/security/pam_sram_validate.so debug url=https://sram.surf.nl token=A3TV6wIt1AqSbn3HdCIgCyOP_38tY-6C1pGI2RwZ79gE" redis=redis | sudo tee "/etc/pam.d/test"

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = pam_sram_validate.so
pam_sram_validate_so_SOURCES = pam_sram_validate.c validate.c api.c config.c json.c logging.c cache.cpp digest.cpp
pam_sram_validate_so_CFLAGS=-fPIC
pam_sram_validate_so_CPPFLAGS=-fPIC
pam_sram_validate_so_CXXFLAGS=-fPIC
pam_sram_validate_so_LDFLAGS= -shared -fPIC
