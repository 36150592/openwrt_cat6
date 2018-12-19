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
  SUBMENU:=Applications
endef

define Package/cpeconfer/description
  cpeconfer version 1
endef

define Build/Prepare
endef

define Build/Compile
	
endef


define Package/cpeconfer/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) ./files/cmd.lua $(1)/bin/cpeconfer
	$(INSTALL_DIR) $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/wifi.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/dhcp.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/util.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/firewall.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/modem.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/network.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/sim.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/system.lua $(1)/usr/lib/lua/tz
	$(INSTALL_DATA) ./files/device.lua $(1)/usr/lib/lua/tz
endef


$(eval $(call BuildPackage,cpeconfer))

