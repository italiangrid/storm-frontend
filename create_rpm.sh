autoreconf --install --force
./configure CFLAGS=-g CXXFLAGS=-g
make rpm
