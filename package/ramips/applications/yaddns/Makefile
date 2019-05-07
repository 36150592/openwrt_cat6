include $(TOPDIR)/rules.mk

PKG_NAME:=yaddns
PKG_VERSION:=master
PKG_RELEASE=$(PKG_SOURCE_VERSION)


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/autotools.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_GLIBC),-lrt)

define Package/yaddns
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread
  TITLE:=no ip ddns
  SUBMENU:=Applications
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
	$(Build/Configure/Default)
endef

define Package/yaddns/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/yaddns $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/yaddns.conf $(1)/etc/yaddns.conf
endef

$(eval $(call BuildPackage,yaddns))
