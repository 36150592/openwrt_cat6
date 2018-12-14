include $(TOPDIR)/rules.mk

PKG_NAME:=alien
PKG_VERSION:=master
PKG_RELEASE=$(PKG_SOURCE_VERSION)



PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/autotools.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_GLIBC),-lrt)

define Package/alien
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libffi +lua +luci
  TITLE:=lua C lib helper
  SUBMENU:=Applications
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
	$(Build/Configure/Default)
endef

define Package/alien/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/src/.libs/alien_c.so   $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/src/alien.lua  $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,alien))
