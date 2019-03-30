#!/bin/sh

echo "topdir=" $TOPDIR
echo "project=" $project
zip_pwd=tz18c6


cd $TOPDIR

SOFTWARE_VERSION=`cat project/s21/key-message | grep software_version | awk -F "=" '{print $2}'`
echo "Soft=${SOFTWARE_VERSION}"

mkdir -p image/${project}/update
rm -rf image/${project}/update/*

cp -f project/s21/bin/factory.bin image/${project}/factory.bin
cp -f project/s21/bin/old-all.bin image/${project}/old-all.bin

cp -f bin/ramips/openwrt-ramips-mt7621-sxx-u-boot.bin image/${project}/new_uboot.bin
cp -f bin/ramips/openwrt-ramips-mt7621-zbt-wg2626-squashfs-sysupgrade.bin image/${project}/new_firmware-kernel-fs.bin
cp -f project/s21/bin/factory.bin image/${project}/
cp -f project/s21/bin/old-all.bin image/${project}/

mkdir -p image/${project}/update/ModemUpdate
mkdir -p image/${project}/update/ConfigUpdateAll
mkdir -p image/${project}/update/ConfigUpdateSome

#new uboot new firmware
rm -f image/${project}/new_firmware_factory.bin
tr '\000' '\377' < /dev/zero | dd of=image/${project}/new_firmware_factory.bin bs=1024 count=16384 2>/dev/null
dd if=image/${project}/new_uboot.bin of=image/${project}/new_firmware_factory.bin conv=notrunc 2>/dev/null
dd if=image/${project}/factory.bin of=image/${project}/new_firmware_factory.bin conv=notrunc bs=64k seek=4 2>/dev/null
dd if=image/${project}/new_firmware-kernel-fs.bin of=image/${project}/new_firmware_factory.bin conv=notrunc bs=64k seek=5 2>/dev/null

#old uboot new firmware
cp -f image/${project}/old-all.bin image/${project}/olduboot_newfirmware.bin
tr '\000' '\377' < /dev/zero | dd of=imag/${project}/emptyfirmware.bin bs=1024 count=6144 2>/dev/null
dd if=image/${project}/emptyfirmware.bin of=image/${project}/olduboot_newfirmware.bin conv=notrunc bs=64k seek=5 2>/dev/null
dd if=image/${project}/new_firmware-kernel-fs.bin of=image/7621_cat6/olduboot_newfirmware.bin conv=notrunc bs=64k seek=5 2>/dev/null

#update package
cp image/${project}/new_firmware-kernel-fs.bin image/${project}/update/firmware-kernel-fs-v${SOFTWARE_VERSION}.bin
cp project/s21/system_updateit image/${project}/update/updateit
md5sum image/${project}/update/firmware-kernel-fs-v${SOFTWARE_VERSION}.bin >> image/${project}/update/md5sum.txt
cd image/${project}/update && zip -P ${zip_pwd} ${project}_v${SOFTWARE_VERSION}.bin * -r

