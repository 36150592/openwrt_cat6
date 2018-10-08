include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=netcwmp-7621
PKG_RELEASE:=1.0.2

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/netcwmp-7621
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=Netcwmp(Tr069) FQa MTK7628 device
        DEPENDS:=+lua +luci-lib-json +libpthread
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/netcwmp-7621/conffiles
/etc/cwmp.conf
endef

define Package/netcwmp-7621/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cwmpd $(1)/usr/sbin/cwmpd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/script/getinfacestatus $(1)/usr/bin/getinfacestatus
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/script/getclientlist $(1)/usr/bin/getclientlist
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/conf/cwmp.conf $(1)/etc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/conf/device.xml $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netcwmp.init $(1)/etc/init.d/netcwmp
endef
$(eval $(call BuildPackage,netcwmp-7621))
