include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=netcwmp
PKG_VERSION:=2019-02-20

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git@192.168.3.234:/home/git/code/netcwmp/.git
#PKG_SOURCE_URL:=https://github.com/netcwmp/netcwmp.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=babb479baf9cb758e94689812eb4b2fa9afe609e
#PKG_SOURCE_VERSION:=c0f7cb0d9ebf7178f96c705741ac137d6c9ba16f
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
PKG_LICENSE:=ISC


include $(INCLUDE_DIR)/package.mk




define Package/netcwmp
        SECTION:=MTK Properties
	CATEGORY:=MTK Properties
	TITLE:=Netcwmp(Tr069) FQa MTK7628 device
        DEPENDS:=+lua +luci-lib-json +libpthread
	SUBMENU:=Applications
endef


CONFIGURE_VARS += \
        BUILD_CC="$(TARGET_CC)" \
        HOSTCC="$(HOSTCC)" 

MAKE_FLAGS :=





define Package/netcwmp/conffiles
/etc/cwmp.conf
endef

define Package/netcwmp/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cwmpd/src/cwmpd $(1)/usr/sbin/cwmpd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/script/getinfacestatus $(1)/usr/bin/getinfacestatus
	$(INSTALL_BIN) ./files/script/getclientlist $(1)/usr/bin/getclientlist
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_CONF) ./files/conf/cwmp.conf $(1)/etc/
	$(INSTALL_CONF) ./files/conf/device.xml $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/netcwmp.init $(1)/etc/init.d/netcwmp
endef
$(eval $(call BuildPackage,netcwmp))
