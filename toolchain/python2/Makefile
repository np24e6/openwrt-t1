include $(TOPDIR)/rules.mk

PKG_NAME:=Python
PKG_VERSION:=2.7.18

PKG_SOURCE_URL:=https://www.python.org/ftp/python/${PKG_VERSION}
PKG_HASH:=b62c0e7937551d0cc02b8fd5cb0f544f9405bafc9a54d3808ed4594812edef43

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_CONFIGURE_ARGS = \
	--prefix= \
	--disable-shared \
	--with-threads \
	--enable-optimizations \
	--with-lto \
	--enable-ipv6 \
	--enable-unicode=ucs4 \
	--with-system-expat \
	--with-system-ffi \
	--with-dbmliborder=gdbm:ndbm \
	--without-ensurepip

# backported from AUR repo
_pybasever=2.7

define Host/Prepare
	$(call Host/Prepare/Default)

	# Temporary workaround for FS#22322
	# See http://bugs.python.org/issue10835 for upstream report
	$(SED) "/progname =/s/python/python$(_pybasever)/" \
		$(HOST_BUILD_DIR)/Python/pythonrun.c

	# Enable built-in SQLite module to load extensions (fix FS#22122)
	$(SED) "/SQLITE_OMIT_LOAD_EXTENSION/d" $(HOST_BUILD_DIR)/setup.py

	# FS#23997
	$(SED) "s|^#.* /usr/local/bin/python|#!/usr/bin/python2|" \
		$(HOST_BUILD_DIR)/Lib/cgi.py

	$(SED) "s/python2.3/python2/g" \
		$(HOST_BUILD_DIR)/Lib/distutils/tests/test_build_scripts.py \
		$(HOST_BUILD_DIR)/Lib/distutils/tests/test_install_scripts.py

	# Ensure that we are using the system copy of various libraries
	# (expat, zlib and libffi), rather than copies shipped in the tarball
	rm -r $(HOST_BUILD_DIR)/Modules/expat
	rm -r $(HOST_BUILD_DIR)/Modules/zlib
	rm -r $(HOST_BUILD_DIR)/Modules/_ctypes/{darwin,libffi}*

	# clean up #!s
	find $(HOST_BUILD_DIR) -name '*.py' | \
		xargs sed -i "s|#[ ]*![ ]*/usr/bin/env python$$$$|#!/usr/bin/env python2|"

	# Workaround asdl_c.py/makeopcodetargets.py errors after we touched the shebangs
	touch $(HOST_BUILD_DIR)/Include/Python-ast.h \
		$(HOST_BUILD_DIR)/Python/Python-ast.c \
		$(HOST_BUILD_DIR)/Python/opcode_targets.h
endef

define Host/Compile
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)
endef

define Host/Install
	mkdir -p $(TOOLCHAIN_DIR)/usr/local/py27
	$(MAKE) -C $(HOST_BUILD_DIR) \
		prefix=$(TOOLCHAIN_DIR)/usr/local/py27 \
		install
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
