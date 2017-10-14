# http://www.imagemagick.org/Usage/thumbnails/#favicon

CONVERT=./convert.exe

icon:
	@echo $(VERSION)
	$(CONVERT) 256.png -define icon:auto-resize=256,128,64,48,32,16 app.ico
	cat app.rc > app_generated.rc
	sed -e 's/1,0,0/$(VERSION)/g; s/\([0-9]\+\)\./\1,/g' version.rc > version_generated.rc
	sed -e 's/VERSION_SHORT/$(VERSION)/g' version_generated.rc >> app_generated.rc
	windres app_generated.rc -O coff -o app.res
	
	$(CONVERT) preferences-system-symbolic.png -define icon:auto-resize=256,128,64,48,32,16 preferences-system-symbolic.ico
	cat testcmd.rc > testcmd_generated.rc
	sed -e 's/VERSION_SHORT/$(VERSION)/g' version_generated.rc >> testcmd_generated.rc
	sed -i -e 's/ph\.exe/testcmd.exe/g' testcmd_generated.rc
	windres testcmd_generated.rc -O coff -o testcmd_generated.res

	sed -i -e 's/testcmd\.exe/testregex.exe/g' testcmd_generated.rc
	windres testcmd_generated.rc -O coff -o testregex_generated.res