if "%1"=="wincw" goto winscw
pushd test
call make_tests.bat
popd

:winscw
if "%1"=="arm" goto end
call abld build winscw udeb
pushd test
pushd gles_test
call abld build winscw udeb
popd
pushd test
call abld build winscw udeb
popd
pushd sos_test
call abld build winscw udeb
popd
pushd sos_test2
call abld build winscw udeb
popd
popd
:end
