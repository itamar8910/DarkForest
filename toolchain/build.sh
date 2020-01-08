#!/bin/bash

BINUTILS_VER="2.33.1"
BINUTILS_FTP="https://ftp.gnu.org/gnu/binutils"
GCC_VER="9.2.0"
GCC_FTP="https://ftp.gnu.org/gnu/gcc/gcc-"

download=true
extract=true
build_binutils=true
build_gcc=true

if [[ "$download" == true ]]; then
	curl "$BINUTILS_FTP/binutils-$BINUTILS_VER.tar.gz" -o binutils.tar.gz
	curl "$GCC_FTP$GCC_VER/gcc-$GCC_VER.tar.gz" -o gcc.tar.gz
fi
if [[ "$extract" == true ]]; then
tar -xzf binutils.tar.gz
tar -xzf gcc.tar.gz
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PREFIX="$DIR/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

if [[ "$build_binutils" == true ]]; then
	# build binutils
	mkdir $DIR/build-binutils
	cd $DIR/build-binutils
	../binutils-$BINUTILS_VER/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
	make
	make install
fi

# build gcc 
cd $DIR
if [[ "$build_gcc" == true ]]; then
	# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
	which -- $TARGET-as || echo $TARGET-as is not in the PATH

	mkdir build-gcc
	cd build-gcc
	../gcc-$GCC_VER/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
	make all-gcc
	make all-target-libgcc
	make install-gcc
	make install-target-libgcc
fi
