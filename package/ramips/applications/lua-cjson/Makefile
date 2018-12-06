#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lua-cjson
PKG_VERSION:=2.1.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.kyne.com.au/~mark/software/download/
PKG_MD5SUM:=24f270663e9f6ca8ba2a02cef19f7963

include $(INCLUDE_DIR)/package.mk

define Package/lua-cjson
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=lua-cjson
  URL:=http://www.kyne.com.au/~mark/software/lua-cjson.php
  DEPENDS:=+lua
endef

define Package/lua-cjson/description
  Lua CJSON provides JSON support for Lua. It has been tested under Linux, FreeBSD, OSX, Solaris and Windows.Make For Openwrt By MeXia,Captain sen @ Openwrt bar.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
          CC="$(TARGET_CC) $(TARGET_CFLAGS) \
          -I"$(STAGING_DIR)/usr/include" \
          -std=gnu99 -fPIC" \
          all
endef


define Package/lua-cjson/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	cp -r $(PKG_BUILD_DIR)/lua/* $(1)/usr/lib/lua
	$(INSTALL_DIR)  $(1)/usr/lib/lua
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/cjson.so  $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,lua-cjson))
