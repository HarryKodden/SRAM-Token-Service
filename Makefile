all: pam-module

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
ifndef URL
	$(error URL is required environment variable !)
endif
ifndef TOKEN
	$(error TOKEN is required environment variable !)
endif
ifndef ENTTILEMENT
	echo ENTTILEMENT is not specified, is optional environment variable
endif

	sudo -E $(MAKE) -C src install

# Test...
.PHONY: test
test: install
ifndef USERNAME
	$(error USERNAME is required environment variable !)
endif
	$(MAKE) -C src test
