#
# zhengda@sztozed.net
#
# Tozed Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tz_web
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/tz_web
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libpthread
  TITLE:=TZ_WEB
  SUBMENU:=Applications
endef

define Package/tz_web/description
  Tozed WEB
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += \
	$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),-DCONFIG_$(c)=$(CONFIG_$c)))

#MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS)"

define Package/tz_web/install
	$(INSTALL_DIR) $(1)/tz_www
	$(INSTALL_DIR) $(1)/tz_www/cgi-bin/
	$(INSTALL_DIR) $(1)/tz_www/js
	$(INSTALL_DIR) $(1)/tz_www/css
	$(INSTALL_DIR) $(1)/tz_www/images
	$(INSTALL_DIR) $(1)/tz_www/js/language
	$(INSTALL_DIR) $(1)/tz_www/images/anhui
	$(INSTALL_DIR) $(1)/tz_www/images/orange
	$(INSTALL_DIR) $(1)/tz_www/images/shatel

	$(INSTALL_DIR) $(1)/tz_www/html
	$(INSTALL_DIR) $(1)/tz_www/html/config
	$(INSTALL_DIR) $(1)/tz_www/html/info
	$(INSTALL_DIR) $(1)/tz_www/html/sys
	$(INSTALL_DIR) $(1)/tz_www/html/update


	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/cgi-bin/* $(1)/tz_www/cgi-bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/mini_httpd.conf $(1)/tz_www
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/*.html $(1)/tz_www
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/js/*.js $(1)/tz_www/js
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/js/language/* $(1)/tz_www/js/language
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/css/* $(1)/tz_www/css
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/images/*.gif $(1)/tz_www/images
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/images/anhui/* $(1)/tz_www/images/anhui
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/images/orange/* $(1)/tz_www/images/orange
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/images/shatel/* $(1)/tz_www/images/shatel

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/*.json $(1)/tz_www
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/html/*.html $(1)/tz_www/html
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/html/config/* $(1)/tz_www/html/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/html/info/* $(1)/tz_www/html/info
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/html/sys/* $(1)/tz_www/html/sys
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/html/update/* $(1)/tz_www/html/update
       
        
endef


$(eval $(call BuildPackage,tz_web))

