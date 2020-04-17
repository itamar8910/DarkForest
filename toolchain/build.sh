#!/bin/bash

BINUTILS_VER="2.33.1"
BINUTILS_FTP="https://ftp.gnu.org/gnu/binutils"
GCC_VER="9.2.0"
GCC_FTP="https://ftp.gnu.org/gnu/gcc/gcc-"

download=false
extract=false
build_binutils=false
build_gcc=false

requested_operation=$1
num_of_cores=$2

case $requested_operation in
	full )
		download=true
		extract=true
		build_binutils=true
		build_gcc=true
		;;
	download )
		download=true
		;;
	extract )
		extract=true
		;;
	build_binutils )
		build_binutils=true
		;;
	build_gcc )
		build_gcc=true
		;;
	* )
		echo "Usage: $0 full/download/extract/build_binutils/build_gcc [num_of_cores_to_build_with]"
		exit 1
		;;
esac

if [[ "$2" == "" ]]; then
    echo "Number of cores to use for build not specified so defaulting to only 1 core"
    num_of_cores=1
fi

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
	make -j$num_of_cores
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
	make -j$num_of_cores all-gcc
	make -j$num_of_cores all-target-libgcc
	make install-gcc
	make install-target-libgcc
fi
