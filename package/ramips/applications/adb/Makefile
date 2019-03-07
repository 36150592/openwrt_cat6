#
# zhengda@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=adb
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/adb
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread +libopenssl +librt
  TITLE:=Tozed adb
  SUBMENU:=Applications
endef

define Package/adb/description
  tozed adb
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += \
	$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),-DCONFIG_$(c)=$(CONFIG_$c)))

#MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS)"

define Package/adb/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/adb $(1)/bin

endef


$(eval $(call BuildPackage,adb))

