PATH=\UIQ3\epoc32\tools\;C:\Program\CSL Arm Toolchain\arm-none-symbianelf\bin;C:\Program\CSL Arm Toolchain\bin;\uiq3\epoc32\tools\;\uiq3\epoc32\gcc\bin\;C:\winnt\system32;C:\winnt;C:\winnt\System32\Wbem;C:\Program Files\ATI Technologies\ATI Control Panel;C:\Program Files\Common Files\Adaptec Shared\System;C:\Perl\bin;\uiq3\epoc32\tools;C:\MSVC6\VC98\Bin;e:\UIQ3\epoc32\tools\nokia_compiler\Symbian_Tools\Command_Line_Tools
Path=$(PATH)
COMPILER_PATH="\UIQ3\epoc32\tools\nokia_compiler\Symbian_Tools\Command_Line_Tools\"

# CWD \tremor_lowmem\epoc\
# MMPFile \tremor_lowmem\epoc\tremor.MMP
# Target libtremor.lib
# TargetType LIB
# BasicTargetType LIB
# MakefileType GNU

ERASE = @erase 2>>nul

# EPOC DEFINITIONS

EPOCBLD = \UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW
EPOCTRG = \UIQ3\EPOC32\RELEASE\WINSCW
EPOCLIB = \UIQ3\EPOC32\RELEASE\WINSCW
EPOCLINK = \UIQ3\EPOC32\RELEASE\WINSCW
EPOCSTATLINK = \UIQ3\EPOC32\RELEASE\WINSCW
EPOCASSPLINK = \UIQ3\EPOC32\RELEASE\WINSCW
EPOCDATA = \UIQ3\EPOC32\DATA
EPOCINC = \UIQ3\EPOC32\INCLUDE
TRGDIR = 
DATADIR = Z\System\Data

EPOCBLDUDEB = $(EPOCBLD)\UDEB
EPOCTRGUDEB = $(EPOCTRG)\UDEB
EPOCLIBUDEB = $(EPOCLIB)\UDEB
EPOCLINKUDEB = $(EPOCLINK)\UDEB
EPOCSTATLINKUDEB = $(EPOCSTATLINK)\UDEB
EPOCASSPLINKUDEB = $(EPOCASSPLINK)\UDEB

EPOCBLDUREL = $(EPOCBLD)\UREL
EPOCTRGUREL = $(EPOCTRG)\UREL
EPOCLIBUREL = $(EPOCLIB)\UDEB
EPOCLINKUREL = $(EPOCLINK)\UDEB
EPOCSTATLINKUREL = $(EPOCSTATLINK)\UREL
EPOCASSPLINKUREL = $(EPOCASSPLINK)\UDEB

# EPOC PSEUDOTARGETS

UDEB : MAKEWORKUDEB RESOURCEUDEB

UREL : MAKEWORKUREL RESOURCEUREL

ALL : UDEB UREL

CLEAN CLEANALL : CLEANBUILD CLEANRELEASE CLEANLIBRARY



WHAT WHATALL : WHATUDEB WHATUREL

RESOURCE RESOURCEALL : RESOURCEUDEB RESOURCEUREL

CLEANBUILD CLEANBUILDALL : CLEANBUILDUDEB CLEANBUILDUREL

CLEANRELEASE CLEANRELEASEALL : CLEANRELEASEUDEB CLEANRELEASEUREL

MAKEWORK MAKEWORKALL : MAKEWORKUDEB MAKEWORKUREL

LISTING LISTINGALL : LISTINGUDEB LISTINGUREL

MAKEWORK : MAKEWORKLIBRARY

RESOURCEUDEB RESOURCEUREL : GENERIC_RESOURCE


MWCIncludes:=$(MWCSym2Includes)
export MWCIncludes


MWLibraries:=+\UIQ3\epoc32\tools\nokia_compiler\Symbian_Support\Runtime\Runtime_x86\Runtime_Win32\Libs;\UIQ3\epoc32\tools\nokia_compiler\Symbian_Support\Win32-x86 Support\Libraries\Win32 SDK
export MWLibraries


MWLibraryFiles:=gdi32.lib;user32.lib;kernel32.lib;
export MWLibraryFiles

# EPOC DEFINITIONS

INCDIR  = -cwd source -i- \
 -i "\tremor_lowmem" \
 -i "\UIQ3\EPOC32\include" \
 -i "\UIQ3\EPOC32\include\libc" \
 -i "\UIQ3\epoc32\include\variant"\
 -i "\UIQ3\epoc32\include\variant\ " -include "UIQ_3.0.hrh"

CWFLAGS = -wchar_t off -align 4 -warnings on -w nohidevirtual,nounusedexpr -enum int -str pool -exc ms  -nostdinc

CWDEFS  =  -d "__SYMBIAN32__" -d "__CW32__" -d "__WINS__" -d "__WINSCW__" -d "_LOW_ACCURACY_" -d "__SUPPORT_CPP_EXCEPTIONS__" $(USERDEFS)

CWUDEB = perl -S err_formatter.pl $(COMPILER_PATH)mwccsym2.exe -msgstyle parseable  -sym codeview -inline off $(CWFLAGS) -d _DEBUG -d _UNICODE $(CWDEFS) $(INCDIR)
CWUREL = perl -S err_formatter.pl $(COMPILER_PATH)mwccsym2.exe -msgstyle parseable  -O4,s $(CWFLAGS) -d NDEBUG -d _UNICODE $(CWDEFS) $(INCDIR)


UDEB : \
	$(EPOCTRGUDEB)\libtremor.lib

UREL : \
	$(EPOCTRGUREL)\libtremor.lib


RESOURCEUDEB : MAKEWORKUDEB

RESOURCEUREL : MAKEWORKUREL



# REAL TARGET - LIBRARY

LIBRARY : MAKEWORKLIBRARY UDEB UREL

FREEZE :

CLEANLIBRARY :

GENERIC_RESOURCE : GENERIC_MAKEWORK

# REAL TARGET - BUILD VARIANT UDEB

WHATUDEB : WHATGENERIC

CLEANUDEB : CLEANBUILDUDEB CLEANRELEASEUDEB

CLEANBUILDUDEB : 
	@perl -S ermdir.pl "$(EPOCBLDUDEB)"

CLEANRELEASEUDEB : CLEANGENERIC


UDEB_RELEASEABLES1= \
	$(EPOCTRGUDEB)\libtremor.lib

WHATUDEB:
	@echo $(UDEB_RELEASEABLES1)

CLEANRELEASEUDEB:
	-$(ERASE) $(UDEB_RELEASEABLES1)



LISTINGUDEB : MAKEWORKUDEB \
	LISTINGUDEBmdct \
	LISTINGUDEBdsp \
	LISTINGUDEBinfo \
	LISTINGUDEBmisc \
	LISTINGUDEBfloor1 \
	LISTINGUDEBfloor0 \
	LISTINGUDEBvorbisfile \
	LISTINGUDEBres012 \
	LISTINGUDEBmapping0 \
	LISTINGUDEBcodebook \
	LISTINGUDEBframing \
	LISTINGUDEBbitwise \
	LISTINGUDEBfloor_lookup \
	LISTINGUDEBalloca

LIBSUDEB=

LINK_OBJSUDEB= \
	$(EPOCBLDUDEB)\mdct.o \
	$(EPOCBLDUDEB)\dsp.o \
	$(EPOCBLDUDEB)\info.o \
	$(EPOCBLDUDEB)\misc.o \
	$(EPOCBLDUDEB)\floor1.o \
	$(EPOCBLDUDEB)\floor0.o \
	$(EPOCBLDUDEB)\vorbisfile.o \
	$(EPOCBLDUDEB)\res012.o \
	$(EPOCBLDUDEB)\mapping0.o \
	$(EPOCBLDUDEB)\codebook.o \
	$(EPOCBLDUDEB)\framing.o \
	$(EPOCBLDUDEB)\bitwise.o \
	$(EPOCBLDUDEB)\floor_lookup.o \
	$(EPOCBLDUDEB)\alloca.o

COMMON_LINK_FLAGSUDEB= -stdlib -subsystem windows


LINK_FLAGSUDEB= $(COMMON_LINK_FLAGSUDEB) $(LIBSUDEB) \
	 -o "$(EPOCTRGUDEB)\libtremor.lib" -noimplib

$(EPOCTRGUDEB)\libtremor.lib : $(LINK_OBJSUDEB)  $(LIBSUDEB)
	$(COMPILER_PATH)mwldsym2.exe -msgstyle gcc -library $(LINK_FLAGSUDEB) -l $(EPOCBLDUDEB) -search $(notdir $(LINK_OBJSUDEB))


# REAL TARGET - BUILD VARIANT UREL

WHATUREL : WHATGENERIC

CLEANUREL : CLEANBUILDUREL CLEANRELEASEUREL

CLEANBUILDUREL : 
	@perl -S ermdir.pl "$(EPOCBLDUREL)"

CLEANRELEASEUREL : CLEANGENERIC


UREL_RELEASEABLES1= \
	$(EPOCTRGUREL)\libtremor.lib

WHATUREL:
	@echo $(UREL_RELEASEABLES1)

CLEANRELEASEUREL:
	-$(ERASE) $(UREL_RELEASEABLES1)



LISTINGUREL : MAKEWORKUREL \
	LISTINGURELmdct \
	LISTINGURELdsp \
	LISTINGURELinfo \
	LISTINGURELmisc \
	LISTINGURELfloor1 \
	LISTINGURELfloor0 \
	LISTINGURELvorbisfile \
	LISTINGURELres012 \
	LISTINGURELmapping0 \
	LISTINGURELcodebook \
	LISTINGURELframing \
	LISTINGURELbitwise \
	LISTINGURELfloor_lookup \
	LISTINGURELalloca

LIBSUREL=

LINK_OBJSUREL= \
	$(EPOCBLDUREL)\mdct.o \
	$(EPOCBLDUREL)\dsp.o \
	$(EPOCBLDUREL)\info.o \
	$(EPOCBLDUREL)\misc.o \
	$(EPOCBLDUREL)\floor1.o \
	$(EPOCBLDUREL)\floor0.o \
	$(EPOCBLDUREL)\vorbisfile.o \
	$(EPOCBLDUREL)\res012.o \
	$(EPOCBLDUREL)\mapping0.o \
	$(EPOCBLDUREL)\codebook.o \
	$(EPOCBLDUREL)\framing.o \
	$(EPOCBLDUREL)\bitwise.o \
	$(EPOCBLDUREL)\floor_lookup.o \
	$(EPOCBLDUREL)\alloca.o

COMMON_LINK_FLAGSUREL= -stdlib -subsystem windows


LINK_FLAGSUREL= $(COMMON_LINK_FLAGSUREL) $(LIBSUREL) \
	 -o "$(EPOCTRGUREL)\libtremor.lib" -noimplib

$(EPOCTRGUREL)\libtremor.lib : $(LINK_OBJSUREL)  $(LIBSUREL)
	$(COMPILER_PATH)mwldsym2.exe -msgstyle gcc -library $(LINK_FLAGSUREL) -l $(EPOCBLDUREL) -search $(notdir $(LINK_OBJSUREL))


# SOURCES

# Source mdct.c

$(EPOCBLDUDEB)\mdct.o \
$(EPOCBLDUREL)\mdct.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\mdct.h \
	\tremor_lowmem\mdct_lookup.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\mdct.o : \tremor_lowmem\mdct.c
	echo mdct.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\mdct.c"

LISTINGUDEBmdct : $(EPOCBLDUDEB)\mdct.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\mdct.WINSCW.lst

$(EPOCBLDUREL)\mdct.o : \tremor_lowmem\mdct.c
	echo mdct.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\mdct.c"

LISTINGURELmdct : $(EPOCBLDUREL)\mdct.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\mdct.WINSCW.lst



# Source dsp.c

$(EPOCBLDUDEB)\dsp.o \
$(EPOCBLDUREL)\dsp.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\mdct.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os_types.h \
	\tremor_lowmem\window_lookup.h

$(EPOCBLDUDEB)\dsp.o : \tremor_lowmem\dsp.c
	echo dsp.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\dsp.c"

LISTINGUDEBdsp : $(EPOCBLDUDEB)\dsp.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\dsp.WINSCW.lst

$(EPOCBLDUREL)\dsp.o : \tremor_lowmem\dsp.c
	echo dsp.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\dsp.c"

LISTINGURELdsp : $(EPOCBLDUREL)\dsp.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\dsp.WINSCW.lst



# Source info.c

$(EPOCBLDUDEB)\info.o \
$(EPOCBLDUREL)\info.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\ctype.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\info.o : \tremor_lowmem\info.c
	echo info.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\info.c"

LISTINGUDEBinfo : $(EPOCBLDUDEB)\info.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\info.WINSCW.lst

$(EPOCBLDUREL)\info.o : \tremor_lowmem\info.c
	echo info.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\info.c"

LISTINGURELinfo : $(EPOCBLDUREL)\info.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\info.WINSCW.lst



# Source misc.c

$(EPOCBLDUDEB)\misc.o \
$(EPOCBLDUREL)\misc.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\stdarg_e.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdio.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\time.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\misc.o : \tremor_lowmem\misc.c
	echo misc.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\misc.c"

LISTINGUDEBmisc : $(EPOCBLDUDEB)\misc.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\misc.WINSCW.lst

$(EPOCBLDUREL)\misc.o : \tremor_lowmem\misc.c
	echo misc.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\misc.c"

LISTINGURELmisc : $(EPOCBLDUREL)\misc.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\misc.WINSCW.lst



# Source floor1.c

$(EPOCBLDUDEB)\floor1.o \
$(EPOCBLDUREL)\floor1.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\floor1.o : \tremor_lowmem\floor1.c
	echo floor1.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\floor1.c"

LISTINGUDEBfloor1 : $(EPOCBLDUDEB)\floor1.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\floor1.WINSCW.lst

$(EPOCBLDUREL)\floor1.o : \tremor_lowmem\floor1.c
	echo floor1.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\floor1.c"

LISTINGURELfloor1 : $(EPOCBLDUREL)\floor1.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\floor1.WINSCW.lst



# Source floor0.c

$(EPOCBLDUDEB)\floor0.o \
$(EPOCBLDUREL)\floor0.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\lsp_lookup.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\floor0.o : \tremor_lowmem\floor0.c
	echo floor0.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\floor0.c"

LISTINGUDEBfloor0 : $(EPOCBLDUDEB)\floor0.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\floor0.WINSCW.lst

$(EPOCBLDUREL)\floor0.o : \tremor_lowmem\floor0.c
	echo floor0.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\floor0.c"

LISTINGURELfloor0 : $(EPOCBLDUREL)\floor0.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\floor0.WINSCW.lst



# Source vorbisfile.c

$(EPOCBLDUDEB)\vorbisfile.o \
$(EPOCBLDUREL)\vorbisfile.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\errno.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stdarg_e.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdio.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\errno.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\ivorbisfile.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\vorbisfile.o : \tremor_lowmem\vorbisfile.c
	echo vorbisfile.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\vorbisfile.c"

LISTINGUDEBvorbisfile : $(EPOCBLDUDEB)\vorbisfile.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\vorbisfile.WINSCW.lst

$(EPOCBLDUREL)\vorbisfile.o : \tremor_lowmem\vorbisfile.c
	echo vorbisfile.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\vorbisfile.c"

LISTINGURELvorbisfile : $(EPOCBLDUREL)\vorbisfile.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\vorbisfile.WINSCW.lst



# Source res012.c

$(EPOCBLDUDEB)\res012.o \
$(EPOCBLDUREL)\res012.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\res012.o : \tremor_lowmem\res012.c
	echo res012.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\res012.c"

LISTINGUDEBres012 : $(EPOCBLDUDEB)\res012.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\res012.WINSCW.lst

$(EPOCBLDUREL)\res012.o : \tremor_lowmem\res012.c
	echo res012.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\res012.c"

LISTINGURELres012 : $(EPOCBLDUREL)\res012.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\res012.WINSCW.lst



# Source mapping0.c

$(EPOCBLDUDEB)\mapping0.o \
$(EPOCBLDUREL)\mapping0.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stdarg_e.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdio.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\codec_internal.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\mdct.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\mapping0.o : \tremor_lowmem\mapping0.c
	echo mapping0.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\mapping0.c"

LISTINGUDEBmapping0 : $(EPOCBLDUDEB)\mapping0.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\mapping0.WINSCW.lst

$(EPOCBLDUREL)\mapping0.o : \tremor_lowmem\mapping0.c
	echo mapping0.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\mapping0.c"

LISTINGURELmapping0 : $(EPOCBLDUREL)\mapping0.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\mapping0.WINSCW.lst



# Source codebook.c

$(EPOCBLDUDEB)\codebook.o \
$(EPOCBLDUREL)\codebook.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\codebook.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\codebook.o : \tremor_lowmem\codebook.c
	echo codebook.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\codebook.c"

LISTINGUDEBcodebook : $(EPOCBLDUDEB)\codebook.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\codebook.WINSCW.lst

$(EPOCBLDUREL)\codebook.o : \tremor_lowmem\codebook.c
	echo codebook.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\codebook.c"

LISTINGURELcodebook : $(EPOCBLDUREL)\codebook.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\codebook.WINSCW.lst



# Source framing.c

$(EPOCBLDUDEB)\framing.o \
$(EPOCBLDUREL)\framing.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\framing.o : \tremor_lowmem\framing.c
	echo framing.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\framing.c"

LISTINGUDEBframing : $(EPOCBLDUDEB)\framing.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\framing.WINSCW.lst

$(EPOCBLDUREL)\framing.o : \tremor_lowmem\framing.c
	echo framing.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\framing.c"

LISTINGURELframing : $(EPOCBLDUREL)\framing.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\framing.WINSCW.lst



# Source bitwise.c

$(EPOCBLDUDEB)\bitwise.o \
$(EPOCBLDUREL)\bitwise.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\stdlib.h \
	\UIQ3\EPOC32\include\libc\string.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\asm_arm.h \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\ivorbiscodec.h \
	\tremor_lowmem\misc.h \
	\tremor_lowmem\ogg.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\bitwise.o : \tremor_lowmem\bitwise.c
	echo bitwise.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\bitwise.c"

LISTINGUDEBbitwise : $(EPOCBLDUDEB)\bitwise.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\bitwise.WINSCW.lst

$(EPOCBLDUREL)\bitwise.o : \tremor_lowmem\bitwise.c
	echo bitwise.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\bitwise.c"

LISTINGURELbitwise : $(EPOCBLDUREL)\bitwise.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\bitwise.WINSCW.lst



# Source floor_lookup.c

$(EPOCBLDUDEB)\floor_lookup.o \
$(EPOCBLDUREL)\floor_lookup.o \
: \
	\UIQ3\EPOC32\include\libc\_ansi.h \
	\UIQ3\EPOC32\include\libc\machine\ieeefp.h \
	\UIQ3\EPOC32\include\libc\machine\types.h \
	\UIQ3\EPOC32\include\libc\math.h \
	\UIQ3\EPOC32\include\libc\stddef.h \
	\UIQ3\EPOC32\include\libc\sys\reent.h \
	\UIQ3\EPOC32\include\libc\sys\stdio_t.h \
	\UIQ3\EPOC32\include\libc\sys\types.h \
	\UIQ3\EPOC32\include\libc\time.h \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh \
	\tremor_lowmem\config_types.h \
	\tremor_lowmem\os.h \
	\tremor_lowmem\os_types.h

$(EPOCBLDUDEB)\floor_lookup.o : \tremor_lowmem\floor_lookup.c
	echo floor_lookup.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\floor_lookup.c"

LISTINGUDEBfloor_lookup : $(EPOCBLDUDEB)\floor_lookup.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\floor_lookup.WINSCW.lst

$(EPOCBLDUREL)\floor_lookup.o : \tremor_lowmem\floor_lookup.c
	echo floor_lookup.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\floor_lookup.c"

LISTINGURELfloor_lookup : $(EPOCBLDUREL)\floor_lookup.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\floor_lookup.WINSCW.lst



# Source alloca.c

$(EPOCBLDUDEB)\alloca.o \
$(EPOCBLDUREL)\alloca.o \
: \
	\UIQ3\epoc32\include\variant\Symbian_OS_v9.1.hrh \
	\UIQ3\epoc32\include\variant\UIQ_3.0.hrh

$(EPOCBLDUDEB)\alloca.o : \tremor_lowmem\alloca.c
	echo alloca.c
	$(CWUDEB) -o "$@" -c "\tremor_lowmem\alloca.c"

LISTINGUDEBalloca : $(EPOCBLDUDEB)\alloca.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\alloca.WINSCW.lst

$(EPOCBLDUREL)\alloca.o : \tremor_lowmem\alloca.c
	echo alloca.c
	$(CWUREL) -o "$@" -c "\tremor_lowmem\alloca.c"

LISTINGURELalloca : $(EPOCBLDUREL)\alloca.lis
	perl -S ecopyfile.pl $? \tremor_lowmem\alloca.WINSCW.lst



ROMFILE:

# Implicit rule for generating .lis files

.SUFFIXES : .lis .o

.o.lis:
	$(COMPILER_PATH)mwldsym2.exe -msgstyle gcc -S -show source,unmangled,comments $< -o $@




WHATGENERIC CLEANGENERIC :
	@rem none

# Rules to create all necessary directories

GENERIC_MAKEWORK : \
	\UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW
MAKEWORKLIBRARY : \
	\UIQ3\EPOC32\RELEASE\WINSCW\UDEB
MAKEWORKUDEB : \
	\UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW\UDEB \
	\UIQ3\EPOC32\RELEASE\WINSCW\UDEB
MAKEWORKUREL : \
	\UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW\UREL \
	\UIQ3\EPOC32\RELEASE\WINSCW\UREL

\UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW \
\UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW\UDEB \
\UIQ3\EPOC32\BUILD\tremor_lowmem\epoc\tremor\WINSCW\UREL \
\UIQ3\EPOC32\RELEASE\WINSCW\UDEB \
\UIQ3\EPOC32\RELEASE\WINSCW\UREL \
:
	perl -S emkdir.pl $@

