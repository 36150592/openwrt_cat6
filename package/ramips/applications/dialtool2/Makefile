#
# zhengda@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dialtool2
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/dialtool2
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread
  TITLE:=Dialtool2
  SUBMENU:=Applications
endef

define Package/dialtool2/description
  dialtool version 2
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += \
	$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),-DCONFIG_$(c)=$(CONFIG_$c)))

#MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS)"

define Package/dialtool2/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dialtool_new $(1)/bin

	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/dialogtool2_config $(1)/etc/dialogtool2_config
	$(INSTALL_DATA) ./files/dialogtool2_version $(1)/etc/dialogtool2_version
	$(INSTALL_BIN) ./files/udhcpc.script $(1)/etc/udhcpc.script
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dialtool2.init $(1)/etc/init.d/dialtool2
endef


$(eval $(call BuildPackage,dialtool2))

