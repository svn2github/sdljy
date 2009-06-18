ZDIR=\epoc32\data\z
TARGETDIR=$(ZDIR)\Resource\apps
SOURCE=data\pointer
FNAME=sdlexe
RESOURCE :
	bmconv /h\epoc32\include\$(FNAME).mbg $(TARGETDIR)\$(FNAME).mbm \
	    	/c16$(SOURCE).bmp /8$(SOURCE)-alpha.bmp /c16$(SOURCE)-2.bmp
	copy $(TARGETDIR)\$(FNAME).mbm \epoc32\release\winscw\udeb\z\resource\apps\ /Y
	-md \epoc32\include\SDL
	
	-copy ..\include\*.* \epoc32\include\SDL  /Y
	-copy .\inc\SDL_*.* \epoc32\include\SDL  /Y

	-perl tools\gendefdata.pl include\internal\gles_armv5_def.txt include\internal\gles_armv5_def.h
	-perl tools\gendefdata.pl include\internal\gles_winscw_def.txt include\internal\gles_wincw_def.h
	    	
do_nothing :
	@rem do_nothing

EXPORT:
	

MAKMAKE : do_nothing
	
BLD :  do_nothing


CLEAN : 
	-del /Q \epoc32\include\SDL\*.* 
	-rd /S /Q \epoc32\include\SDL
	
LIB : do_nothing

CLEANLIB : do_nothing

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)
FINAL : do_nothing
	 