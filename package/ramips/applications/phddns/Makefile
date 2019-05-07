include $(TOPDIR)/rules.mk

PKG_NAME:=phddns
PKG_VERSION:=2.0.6
PKG_RELEASE=$(PKG_SOURCE_VERSION)


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/autotools.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_GLIBC),-lrt)

define Package/phddns
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread
  TITLE:=ph ddns
  SUBMENU:=Applications
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
	$(Build/Configure/Default)
endef

define Package/phddns/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/phddns $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/phddns.conf $(1)/etc/phddns.conf
endef

$(eval $(call BuildPackage,phddns))
