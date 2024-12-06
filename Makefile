SHELL=pwsh

!IFNDEF CONFIGURATION
CONFIGURATION=Debug
!ENDIF

!IFNDEF CLEAN
CLEAN=0
!ENDIF

all: build test

build: phony
	@echo "Building..."
	@$(SHELL) .\tools\build.ps1 -config $(CONFIGURATION) -clean $(CLEAN)

test: phony
	@echo "Testing..."
	@$(SHELL) .\tools\test.ps1

setup: phony
	@echo "Setting up..."
	@$(SHELL) .\tools\setup_devenv.ps1

# pesudotarget
phony:

