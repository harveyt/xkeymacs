DEVENV	= "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 10.0/Common7/IDE/devenv.com"

build:
	rm Release/xkeymacs.dll	Release/xkeymacs64.dll
	$(DEVENV) xkeymacs-vc10.sln /build "Release|Win32"
	$(DEVENV) xkeymacs-vc10.sln /build "Release|x64"

clean:
	git clean -fdx

