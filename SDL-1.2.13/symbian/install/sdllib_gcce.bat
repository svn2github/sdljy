if exist \certificates goto pack
pushd \
mkdir certificates
pushd certificates
call makekeys -cert -password foobar -len 2048 -dname "CN=SDL OR=Simple Media Library CO=FI" selfsigned.key selfsigned.cer
popd
popd
:pack
call makesis sdllib_gcce.pkg
call signsis -v sdllib_gcce.sis sdllib_gcce.sisx \certificates\selfsigned.cer \certificates\selfsigned.key foobar
rem copy sdllib_gcce.sisx ..\..\..\
