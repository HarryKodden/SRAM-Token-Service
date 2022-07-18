all: pam-module

# Environment variables, with not specified, these defaults will apply...
URL ?= http://sram.surf.nl
TOKEN ?= "sram-service-API-token-here"
USERNAME ?= $(shell whoami)
ENTITLED ?= *
export

# Make pam_sram_validate module
.PHONY: pam-module
pam-module: json-parser-package src/pam_sram_validate.so
	$(MAKE) -C src/
src/pam_sram_validate.so: src/Makefile
src/Makefile: src/Makefile.am src/configure.ac
	cd src && ./autogen.sh && ./configure

# Get / Update json-parser package...
.PHONY: json-parser-package
json-parser-package: json-parser
	git submodule init
	git submodule update

# Get json-parser package...
json-parser: 
	git clone https://github.com/json-parser/json-parser.git

# Cleanup...
.PHONY: clean
clean:
	@-$(MAKE) -C src clean
	@-$(MAKE) -C src distclean

# Install...
.PHONY: install
install: pam-module
	sudo -E $(MAKE) -C src install

# Test...
.PHONY: test
test: install
	$(MAKE) -C src test
