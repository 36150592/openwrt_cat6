#!/bin/sh
EXTPATH=/home/jiang/work/lede-openwrt7621/
#export PATH=$PATH:$EXTPATH/staging_dir/host/bin:$EXTPATH/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin
export  STAGING_DIR=$EXTPATH/staging_dir
export  DESTDIR=/tmp/rtty_install
mkdir rtty
cd rtty
git clone https://github.com/enki/libev.git
git clone https://github.com/zhaojh329/libuwsc.git
git clone https://github.com/zhaojh329/rtty.git
cd libev
./configure
make install
cd ../libuwsc
cmake . -DCMAKE_C_COMPILER=gcc -DCMAKE_FIND_ROOT_PATH=$DESTDIR -DUWSC_SSL_SUPPORT=OFF
make install
cd ../rtty
cmake . -DCMAKE_C_COMPILER=gcc -DCMAKE_FIND_ROOT_PATH=$DESTDIR
make install
cp $DESTDIR/usr/local/bin/rtty $EXTPATH/package/sinzuo/sinzuortty/rootfs-x86/usr/sbin/
cp $DESTDIR/usr/local/lib/libev.so.4.0.0 $EXTPATH/package/sinzuo/sinzuortty/rootfs-x86/usr/lib/
cp $DESTDIR/usr/local/lib/libuwsc.so.3.2.0 $EXTPATH/package/sinzuo/sinzuortty/rootfs-x86/usr/lib/

