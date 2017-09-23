@echo off
setlocal

pushd "%~dp1"

REM "%~1" -define icon:auto-resize="256,128,96,64,48,32,16" "%~n1.ico"
convert.exe "%~1" ( -clone 0 -resize 16x16 -extent 16x16 ) ( -clone 0 -resize 32x32 -extent 32x32 ) ( -clone 0 -resize 48x48 -extent 48x48 ) ( -clone 0 -resize 64x64 -extent 64x64 ) ( -clone 0 -resize 128x128 -extent 128x128 ) ( -clone 0 -resize 256x256 -extent 256x256 ) -delete 0 -alpha on -colors 256 app.ico

popd

@echo Done!
REM pause
exit