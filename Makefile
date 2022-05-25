all: pam-module

# Make pam_sram_validate module
.PHONY: pam-module
pam-module: json-parser src/pam_sram_validate.so
src/pam_sram_validate.so: src/Makefile
	$(MAKE) -C src/
src/Makefile: src/Makefile.am src/configure.ac
	cd src && ./autogen.sh && ./configure

# Get / Update json-parser package...
.PHONY: json-parser
json-parser:
	git submodule init
	git submodule update

# Cleanup...
.PHONY: clean
clean:
	@-$(MAKE) -C src clean
	@-$(MAKE) -C src distclean

# Install...
.PHONY: install
install: pam-module
	$(MAKE) -C src install

# Test...
.PHONY: test
test: install
	$(MAKE) -C src test
