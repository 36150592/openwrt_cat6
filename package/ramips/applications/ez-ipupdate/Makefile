include $(TOPDIR)/rules.mk

PKG_NAME:=ez-ipupdate
PKG_VERSION:=3.0.11
PKG_RELEASE=$(PKG_SOURCE_VERSION)


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/autotools.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_GLIBC),-lrt)

define Package/ez-ipupdate
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread
  TITLE:=3322 ddns
  SUBMENU:=Applications
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
	$(Build/Configure/Default)
endef

define Package/ez-ipupdate/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ez-ipupdate	 $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/ez-ipupdate.conf $(1)/etc/ez-ipdate.conf
endef

$(eval $(call BuildPackage,ez-ipupdate))
