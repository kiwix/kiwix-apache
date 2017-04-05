set -e
g++ `pkg-config --cflags apr-1` -fPIC -DPIC -I/usr/include/apache2/ -c mod_example.cpp
sudo apxs -i -n example_module `pkg-config --libs apr-1` -lzim -c mod_example.o
