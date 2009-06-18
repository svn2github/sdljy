@echo off
if "%1"=="" goto ERROR
if /i "%1"=="openc" goto OPENC
if /i "%1"=="symbianc" goto SYMBIANC
:ERROR
echo "buildtest OPENC|SYMBIANC"
goto END
:OPENC
call maketest_openc.bat
goto NEXT
:SYMBIANC
call maketest_symbianc.bat
goto NEXT
:NEXT

perl ..\tools\subst_arm_gcce.pl test\sdl_test.pkg  _gcce \\armv5\\ \gcce\

pushd test
call bldmake bldfiles
call abld build winscw udeb
call abld build armv5 urel
call abld build gcce urel
rem call makekeys -cert -password yourpassword -len 2048 -dname "CN=SDL OR=Simple Media Library CO=FI" sdl_test.key sdl_test.cer
call makesis sdl_test.pkg
call signsis -v sdl_test.sis sdl_test.sisx \certificates\selfsigned.cer \certificates\selfsigned.key foobar
call makesis sdl_test_gcce.pkg
call signsis -v sdl_test_gcce.sis sdl_test_gcce.sisx \certificates\selfsigned.cer \certificates\selfsigned.key foobar
copy sdl_test.sisx ..\..\install
copy sdl_test_gcce.sisx ..\..\install
popd
:END
