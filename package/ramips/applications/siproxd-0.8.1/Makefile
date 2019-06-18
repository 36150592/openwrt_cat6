include $(TOPDIR)/rules.mk

PKG_NAME:=siproxd-0.8.1
PKG_RELEASE:=1


PKG_BUILD_DIR := $(BUILD_DIR)/customer/$(PKG_NAME)
PKG_BUILD_DEPENDS :=
CMAKE_INSTALL:=0

include $(INCLUDE_DIR)/package.mk

#TARGET_LDFLAGS+=-L$(BUILD_DIR)/sequans-apps-r4.1.x/obj/host/openwrt/lib
#TARGET_CFLAGS+=-I$(BUILD_DIR)/sequans-apps-r4.1.x/obj/host/openwrt/include -DSQN_HOST -DSQN_NO_CVT -L$(BUILD_DIR)/sequans-apps-r4.1.x/obj/host/openwrt/lib
#TARGET_CFLAGS+=-I$(STAGING_DIR)/usr/sqn/include -DSQN_HOST -DSQN_NO_CVT -L$(STAGING_DIR)/usr/lib
TARGET_CFLAGS+=-I$(STAGING_DIR)/usr/sqn/include -DSQN_HOST -DSQN_NO_CVT -L$(STAGING_DIR)/usr/sqn/lib
TARGET_CFLAGS += -I ${STAGING_DIR}/usr/include/libxml2
TARGET_CFLAGS += -I ${STAGING_DIR}/usr/include/cmdlib

MAKE_FLAGS += EXTRA_LDFLAGS_FOR_LINK_CMDLIB="-lcmdlib"

CONFIGURE_ARGS += -disable-shared \
				--prefix=/usr \
				--sysconfdir=/etc \
				--disable-doc \
				--enable-static-libosip2 \
				--with-libosip-prefix="$(STAGING_DIR)/usr"


define Package/siproxd-0.8.1
  SECTION:=cpe
  CATEGORY:=CPE
  DEPENDS:=+libltdl +libpthread libosip2-4.1.0
  TITLE:=siproxd-0.8.1
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
	cd ${PKG_BUILD_DIR} && make install DESTDIR=${1}
	rm -rf ${1}/etc/*
	rm -rf ${1}/usr/lib/siproxd/*
endef


$(eval $(call BuildPackage,siproxd-0.8.1))

