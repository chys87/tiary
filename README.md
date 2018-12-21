# BUILD INSTRUCTIONS #

## Get it ##

Clone it by git:

    git clone https://github.com/chys87/tiary.git

or download a snapshot.

## Generate configure script ##

You will need autoconf, automake and autoconf-archive:

    cd tiary
    autoreconf --install -v

## Configure, make and install ##

You will need ncurses, libxml2, re2, libbz2 and their development packages:

    ./configure --prefix=/where/you/want/to/install/it
    make
    make install
