call subst_arm_gcce.bat ..\install\sdllib sdllib
call subst_arm_gcce.bat gles_test\sis\gles_test gles_test
call subst_arm_gcce.bat sos_test\sis\sdltest sdltest
call subst_arm_gcce.bat  sos_test2\sis\sdltest2 sdltest2



pushd ..
call bldmake bldfiles
call abld build armv5 urel
call abld build gcce urel
pushd install
call sdllib.bat
call sdllib_gcce.bat
del sdl-1.2.13-s60-2.3_armv5.sisx
del sdl-1.2.13-s60-2.3_gcce.sisx
ren sdllib.sisx sdl-1.2.13-s60-2.3_armv5.sisx
ren sdllib_gcce.sisx sdl-1.2.13-s60-2.3_gcce.sisx
popd
popd
call buildtest.bat SYMBIANC
copy test\sdl_test.six
copy test\sdl_test_gcce.six
pushd gles_test
call bldmake bldfiles
call abld build armv5 urel
call abld build gcce urel
pushd sis

call gles_test.bat
copy gles_test.sisx ..\..\..\install
call gles_test_gcce.bat
copy gles_test_gcce.sisx ..\..\..\install
popd
popd
pushd sos_test
call bldmake bldfiles
call abld build armv5 urel
call abld build gcce urel
pushd sis

call sdltest.bat
copy sdltest.sisx ..\..\..\install
call sdltest_gcce.bat
copy sdltest_gcce.sisx ..\..\..\install
popd
popd
pushd sos_test2
call bldmake bldfiles
call abld build armv5 urel
call abld build gcce urel
pushd sis

call sdltest2.bat
copy sdltest2.sisx ..\..\..\install
call sdltest2_gcce.bat
copy sdltest2_gcce.sisx ..\..\..\install
popd
popd

:exit