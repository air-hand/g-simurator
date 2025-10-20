SHELL=pwsh

!IFNDEF CONFIGURATION
CONFIGURATION=Debug
!ENDIF

!IFNDEF CLEAN
CLEAN=0
!ENDIF

all: test

build: phony
	@echo "Building..."
	@$(SHELL) .\tools\build.ps1 -build_type $(CONFIGURATION) -clean $(CLEAN) || exit 1

test: build phony
	@echo "Testing..."
	@$(SHELL) .\tools\test.ps1 || exit 1

# pesudotarget
phony:

