PWSH=pwsh

!IFNDEF CONFIGURATION
CONFIGURATION=Debug
!ENDIF

!IFNDEF CLEAN
CLEAN=0
!ENDIF

all: test

setup: phony
	@echo "Please Open $(PWSH) as Administrator and exec [$(PWSH) $(MAKEDIR)\tools\setup_devenv.ps1] other terminal. cause nmake.exe depends on this script."

build: phony ## Build the project
	@echo "Building..."
	@$(PWSH) -Command "& { .\tools\build.ps1 -build_type $(CONFIGURATION) -clean $(CLEAN) 2>&1 | Tee-Object -FilePath 'build_output.log'; exit $$LASTEXITCODE }"

test: phony ## Run tests (builds if needed / or requires prior build)
	@echo "Testing..."
	@$(PWSH) -Command "& { .\tools\test.ps1 -build_type $(CONFIGURATION); exit $$LASTEXITCODE }"

# pesudotarget
phony:
