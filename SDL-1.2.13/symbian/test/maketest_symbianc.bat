mkdir 	test
del /q 	test\*.*
perl ..\tools\common_files.pl "test\bld.inf" "test\sdl_test.pkg"
perl ..\tools\make_parse.pl ..\..\test\Makefile.in all "perl ..\tools\mmpmake.pl" -uinc ..\..\..\include -uinc ..\..\..\test -srcdir ..\..\..\test -uid 0xE0001000 -pkgfile test\sdl_test.pkg -targetdir test -bldinffile test\bld.inf -sinc \epoc32\include\libc -sinc ..\..\..\include -lib estlib.lib -excl testcdrom;testdyngl;testgl;testiconv;testjoystick;testlock;testhread;testsem;torturethread -mathlib libm.lib




