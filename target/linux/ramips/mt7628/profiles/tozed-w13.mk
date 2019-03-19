#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TZ-W13
	NAME:=Tozed w13
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci \
		kmod-ledtrig-usbdev
endef

define Profile/TZ-W13/Description
	Support for Tozed w13 routers.
endef
$(eval $(call Profile,TZ-W13))
