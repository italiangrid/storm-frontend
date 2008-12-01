source frontend-setup.sh
autoreconf --install --force
./configure CFLAGS=-g CXXFLAGS=-g
make
