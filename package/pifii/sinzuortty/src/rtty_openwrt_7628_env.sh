#!/bin/sh
EXTPATH=/home/jiang/work/7621/
export PATH=$PATH:$EXTPATH/staging_dir/host/bin:$EXTPATH/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
export  STAGING_DIR=$EXTPATH/staging_dir
export  DESTDIR=/tmp/rtty_install
rm -rf $DESTDIR
mkdir rtty
cd rtty
git clone https://github.com/enki/libev.git
git clone https://github.com/zhaojh329/libuwsc.git
git clone https://github.com/zhaojh329/rtty.git
cd libev
./configure --host=mipsel-openwrt-linux-uclibc
make install
cd ../libuwsc
cmake . -DCMAKE_C_COMPILER=mipsel-openwrt-linux-uclibc-gcc -DCMAKE_FIND_ROOT_PATH=$DESTDIR -DUWSC_SSL_SUPPORT=OFF
make install
cd ../rtty
cmake . -DCMAKE_C_COMPILER=mipsel-openwrt-linux-uclibc-gcc -DCMAKE_FIND_ROOT_PATH=$DESTDIR
make install
cp $DESTDIR/usr/local/bin/rtty $EXTPATH/package/pifii/sinzuortty/rootfs/usr/sbin/
cp $DESTDIR/usr/local/lib/libev.so.4.0.0 $EXTPATH/package/pifii/sinzuortty/rootfs/usr/lib/
cp $DESTDIR/usr/local/lib/libuwsc.so.3.2.0 $EXTPATH/package/pifii/sinzuortty/rootfs/usr/lib/

