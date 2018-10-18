.PHONY: check clean really-clean compile

APYC = ../apyc

PYTHON = python3

check: check3

check%: 
	$(MAKE) -C .. compile
	@echo Testing correct programs...
	rm -f errflag
	-if [ -f runtime$*.py ]; then run="--runtime=runtime$*.py"; fi; \
	$(PYTHON) test-proj$* --compiler=$(APYC) $$run correct$*/*.py || touch errflag
	@echo
	@echo Testing erroneous programs...
	-$(PYTHON) test-proj$* --errors --compiler=$(APYC) error$*/*.py || touch errflag
	@if test -f errflag; then code=1; else code=0; fi; \
        rm -f errflag; exit $$code

compile:
	make -C .. -j 4 compile

check3: compile

clean:
	$(RM) errflag *~ */*~ 

really-clean: clean
