
@rmdir /s/q epoc32

@mkdir epoc32\include\libpng-1.2.18\
@copy ..\..\png.h epoc32\include\libpng-1.2.18\
@copy ..\..\pngconf.h epoc32\include\libpng-1.2.18\

@mkdir epoc32\release\armv5\urel
@copy C:\Symbian\9.2\S60_3rd_FP1\Epoc32\release\armv5\urel\libpng.lib epoc32\release\armv5\urel\

@del libpng_sdk.zip
@..\..\..\sdks\tools\zip.exe libpng_sdk.zip -R *
