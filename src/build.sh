set -e

if [[ ! -v KIWIX_LIB ]]; then
  echo "KIWIX_LIB is not set. It needs to point to the INSTALL folder containing kiwix-lib. See ../README.md for info."
  exit 1
fi

export PKG_CONFIG_PATH=$KIWIX_LIB/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$KIWIX_LIB/lib/x86_64-linux-gnu
g++ --std=c++11  `pkg-config --cflags apr-1 kiwix` -fPIC -DPIC -I/usr/include/apache2/ -I$KIWIX_LIB/include/ -c mod_kiwix.cpp dealWithOldZimUrls.cpp
sudo apxs -i -n kiwix_module `pkg-config --libs apr-1 kiwix` -c mod_kiwix.o dealWithOldZimUrls.o
