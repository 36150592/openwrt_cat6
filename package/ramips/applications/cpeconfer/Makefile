#
# fengyuanzhao@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cpeconfer
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/cpeconfer
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  TITLE:=Cpeconfer
  DEPENDS:=+libpthread +lua
  SUBMENU:=Applications
endef

define Package/cpeconfer/description
  cpeconfer version 1
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

#define Build/Compile
#	make		
#endef


define Package/cpeconfer/install
	$(INSTALL_DIR) $(1)/bin/
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_BIN) ./files/cmd.lua $(1)/bin/cpeconfer
	$(INSTALL_DIR) $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/*.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/*.sh $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/firewall.tz $(1)/etc
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/luatzlib.so $(1)/usr/lib/lua

endef


$(eval $(call BuildPackage,cpeconfer))

