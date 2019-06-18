include $(TOPDIR)/rules.mk

PKG_NAME:=libosip2-4.1.0
PKG_RELEASE:=1


PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_DEPENDS :=
CMAKE_INSTALL:=0
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

#TARGET_LDFLAGS+=-L$(BUILD_DIR)/sequans-apps-r4.1.x/obj/host/openwrt/lib
#TARGET_CFLAGS+=-I$(BUILD_DIR)/sequans-apps-r4.1.x/obj/host/openwrt/include -DSQN_HOST -DSQN_NO_CVT -L$(BUILD_DIR)/sequans-apps-r4.1.x/obj/host/openwrt/lib
#TARGET_CFLAGS+=-I$(STAGING_DIR)/usr/sqn/include -DSQN_HOST -DSQN_NO_CVT -L$(STAGING_DIR)/usr/lib
#TARGET_CFLAGS+=-I$(STAGING_DIR)/usr/sqn/include -DSQN_HOST -DSQN_NO_CVT -L$(STAGING_DIR)/usr/sqn/lib
#TARGET_CFLAGS += -I ${STAGING_DIR}/usr/include/libxml2
#TARGET_CFLAGS += -I ${STAGING_DIR}/usr/include/cmdlib

#MAKE_FLAGS += EXTRA_LDFLAGS_FOR_LINK_CMDLIB="-lcmdlib"

CONFIGURE_ARGS += --prefix=/usr --disable-shared \
	--enable-static \
	--disable-debug \
	--disable-trace \
	--disable-mpatrol \
	--disable-gprof \
	--disable-mt \
	--enable-pthread \
	--enable-semaphore \
	--disable-sysv \
	--disable-gperf \
	--disable-test \
	--disable-minisize

define Package/libosip2-4.1.0
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=
  TITLE:=libosip2-4.1.0
  SUBMENU:=Applications
endef

define Package/libosip2-4.1.0/description
	check if system works well,if not,then fix it
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/libosip2-4.1.0/install
	cd ${PKG_BUILD_DIR} && make install DESTDIR=${STAGING_DIR}
endef


$(eval $(call BuildPackage,libosip2-4.1.0))

