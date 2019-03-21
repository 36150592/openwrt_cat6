#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tz_mgr
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

SOURCE_DIR:=./src
FILES_DIR:=./files


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=TZ_Config_Clients
  DEPENDS:=+libpthread +libuci
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/$(PKG_NAME)/install
	#install bin
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin
	#install shell script
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(FILES_DIR)/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	#install config
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) $(FILES_DIR)/$(PKG_NAME).config $(1)/etc/config/$(PKG_NAME)
	$(INSTALL_CONF) $(FILES_DIR)/$(PKG_NAME)_cfg.default $(1)/etc/config/.$(PKG_NAME)_cfg.default
	$(INSTALL_CONF) $(FILES_DIR)/.flag_factory_config $(1)/etc/.flag_factory_config
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
