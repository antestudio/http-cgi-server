meson setup build
cd build
meson compile
if [ $? -eq 0 ]; then
  cp lang/lang ../www/cgi-bin/lang
  cd ../www
  clear
  echo Build successful. To start server, run:
  echo '$ ./run.sh'
  echo
else
  echo Build failed
fi