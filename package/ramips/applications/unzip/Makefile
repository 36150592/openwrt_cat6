#
# zhengda@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=unzip
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/unzip
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  TITLE:=unzip app
  SUBMENU:=Applications
endef

define Package/unzip/description
  unzip app
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += \
	$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),-DCONFIG_$(c)=$(CONFIG_$c)))

#MAKE_FLAGS += \
	"generic"

define Package/unzip/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/unzip $(1)/bin
endef


$(eval $(call BuildPackage,unzip))

