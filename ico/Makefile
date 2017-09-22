icon:
	#./convert 256.png    -resize 128x128  128.png
	#./convert 256.png    -resize 64x64  64.png
	#./convert 256.png    -resize 48x48  48.png
	#./convert 256.png    -resize 32x32  32.png
	#./convert 256.png    -resize 22x22  22.png
	#./convert 256.png    -resize 16x16  16.png
	
	#./convert 256.png -define icon:auto-resize="256,128,96,64,48,32,16" app.ico

	./convert.exe 256.png \( -clone 0 -resize 16x16 -extent 16x16 \) \( -clone 0 -resize 32x32 -extent 32x32 \) \( -clone 0 -resize 48x48 -extent 48x48 \) \( -clone 0 -resize 64x64 -extent 64x64 \) \( -clone 0 -resize 128x128 -extent 128x128 \) \( -clone 0 -resize 256x256 -extent 256x256 \) -delete 0 -alpha on -colors 256 app.ico
	
	windres app.rc -O coff -o app.res