rem call makekeys -cert -password yourpassword -len 2048 -dname "CN=Vendor OR=Vendor CO=FI" sdltest.key sdltest.cer
call makesis sdltest2.pkg
call signsis -v sdltest2.sis sdltest2.sisx \certificates\selfsigned.cer \certificates\selfsigned.key foobar