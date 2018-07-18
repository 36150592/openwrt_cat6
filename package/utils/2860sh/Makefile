#
# Copyright (C) 2010 segal.di.ubi.pt
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
	
include $(TOPDIR)/rules.mk
	
PKG_NAME:=2860sh
PKG_VERSION:=1
PKG_RELEASE:=6
	
PKG_MAINTAINER:=Nuno Goncalves <nunojpg@gmail.com>
	
include $(INCLUDE_DIR)/package.mk
	
define Package/2860sh
SECTION:=utils
CATEGORY:=Utilities
TITLE:=install rt2860v2.sh 
endef
	
define Package/2860sh/description
RestoreFactory handles the reset button event and issues a "firstboot" command.
Button and wait time can be configured with a UCI file.
endef

define Build/Compile
endef
define Package/2860sh/install
	$(INSTALL_DIR) $(1)/lib/wifi
#	$(INSTALL_BIN) ./files/rt2860v2.sh $(1)/lib/wifi/rt2860v2.sh
	$(INSTALL_DIR) $(1)/etc/modules.d
	$(INSTALL_BIN) ./files/50-rt2860v2 $(1)/etc/modules.d/50-rt2860v2
	 $(INSTALL_BIN) ./files/50-mt7603e $(1)/etc/modules.d/
	$(INSTALL_BIN) ./files/51-mt76x2e $(1)/etc/modules.d/

endef

$(eval $(call BuildPackage,2860sh))
