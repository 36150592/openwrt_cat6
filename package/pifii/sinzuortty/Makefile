#
# hua.shao@mediatek.com
#
# MTK Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sinzuortty
PKG_RELEASE:=1.1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
#PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
#PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/sinzuortty
  SECTION:=pifii
  CATEGORY:=PiFii
  TITLE:=rootfs rtty
  DEPENDS:= +libevent
endef

define Package/sinzuortty/description
  An program to config eth mac.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += \
	-I$(LINUX_DIR)/include \
	-I$(LINUX_DIR)/drivers/net/raeth \
	$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),-DCONFIG_$(c)))

define Build/Configure
endef

define Package/sinzuortty/install
	$(CP) ./rootfs/* $(1)/
endef


$(eval $(call BuildPackage,sinzuortty))

