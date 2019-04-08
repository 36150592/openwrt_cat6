#
# zhengda@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lanrecord
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/lanrecord
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread +iptables +libuci
  TITLE:=lanrecord
  SUBMENU:=Applications
endef

define Package/lanrecord/description
  lanrecord
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += \
	$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),-DCONFIG_$(c)=$(CONFIG_$c)))

#MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS)"

define Package/lanrecord/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lanrecord $(1)/bin

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/lanrecord.init $(1)/etc/init.d/lanrecord

endef


$(eval $(call BuildPackage,lanrecord))

