#
# fengyuanzhao@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=led_process
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/led_process
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+lua +cpeconfer
  TITLE:=Led_process
  SUBMENU:=Applications
endef

define Package/led_process/description
  led_process version 1
endef

define Build/Prepare
endef

define Build/Compile
	
endef


define Package/led_process/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) ./files/led_process.lua $(1)/bin/led_process
endef


$(eval $(call BuildPackage,led_process))

