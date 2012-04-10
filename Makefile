BINDIR	= $(HOME)/bin/win
DEVENV	= "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 10.0/Common7/IDE/devenv.com"
DESKTOP	= /cygdrive/c/Users/$(USER)/Desktop

build:
	rm -f Release/xkeymacs.dll
	rm -f Release/xkeymacs.exe
	rm -f Release/xkeymacs64.dll
	rm -f Release/xkeymacs64.exe
	$(DEVENV) xkeymacs-vc10.sln /build "Release|Win32"
	$(DEVENV) xkeymacs-vc10.sln /build "Release|x64"

clean:
	git clean -fdx

install:
	cp -f Release/xkeymacs.dll $(BINDIR)
	cp -f Release/xkeymacs.exe $(BINDIR)
	cp -f Release/xkeymacs64.dll $(BINDIR)
	cp -f Release/xkeymacs64.exe $(BINDIR)
	cp -f XKeymacs.lnk $(DESKTOP)

