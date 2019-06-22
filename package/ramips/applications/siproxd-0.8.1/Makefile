include $(TOPDIR)/rules.mk

PKG_NAME:=siproxd-0.8.1
PKG_RELEASE:=1


PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

#TARGET_CFLAGS+=-I$(STAGING_DIR)/usr/sqn/include -DSQN_HOST -DSQN_NO_CVT -L$(STAGING_DIR)/usr/sqn/lib
#TARGET_CFLAGS += -I ${STAGING_DIR}/usr/include/libxml2

CONFIGURE_ARGS += -disable-shared \
				--prefix=/usr \
				--sysconfdir=/etc \
				--disable-doc \
				--enable-static-libosip2 \
				--with-libosip-prefix="$(STAGING_DIR)/usr"


define Package/siproxd-0.8.1
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libltdl +libpthread libosip2-4.1.0
  TITLE:=siproxd-0.8.1
  SUBMENU:=Applications
endef

define Package/siproxd-0.8.1/description
	check if system works well,if not,then fix it
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	cd $(PKG_BUILD_DIR)/ && ./autogen.sh
endef

define Package/siproxd-0.8.1/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/siproxd $(1)/bin

	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) src/siproxd.conf $(1)/etc/

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/siproxd.init $(1)/etc/init.d/siproxd

endef


$(eval $(call BuildPackage,siproxd-0.8.1))

