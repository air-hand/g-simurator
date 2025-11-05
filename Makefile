SHELL=pwsh

!IFNDEF CONFIGURATION
CONFIGURATION=Debug
!ENDIF

!IFNDEF CLEAN
CLEAN=0
!ENDIF

all: test

setup: phony
	@echo "Please Open $(SHELL) as Administrator and exec [$(SHELL) $(MAKEDIR)\tools\setup_devenv.ps1] other terminal. cause nmake.exe depends on this script."

build: phony ## Build the project
	@echo "Building..."
	@$(SHELL) .\tools\build.ps1 -build_type $(CONFIGURATION) -clean $(CLEAN) || exit 1

test: phony ## Run tests (builds if needed / or requires prior build)
	@echo "Testing..."
	@$(SHELL) .\tools\test.ps1 -build_type $(CONFIGURATION) || exit 1

# pesudotarget
phony:

