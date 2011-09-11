# obshutdown

localedir       = $(datadir)/locale
maindir       = $(datadir)/obshutdown
themesdir        = $(DESTDIR)$(datadir)/obshutdown/themes

AUTOMAKE_OPTIONS = subdir-objects foreign
# ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = \
	src/obshutdown

src_obshutdown_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(CAIRO_CFLAGS) \
	-DLOCALEDIR=\"$(localedir)\" \
	-DMAINDIR=\"$(maindir)\" \
	-DTHEMEDIR=\"$(themedir)\"

src_obshutdown_LDADD = \
	$(GTK_LIBS) \
	$(CAIRO_LIBS)

src_obshutdown_SOURCES = \
	src/types.h \
	src/varibles.h \
	src/messages.c \
	src/messages.h \
	src/misc.c \
	src/misc.h \
	src/parsers.c \
	src/parsers.h \
	src/actions.c \
	src/actions.h \
	src/callbacks.c \
	src/callbacks.h \
	src/main.c \
	src/main.h

dist_main_DATA = \
	data/example.rc

EXTRA_DIST = \
	data/themes/* \
	data/themes/foom/* \
	data/themes/oxygen/* \
	data/themes/faenza/* \
	config.rpath \
	obshutdown.cbp \
	README \
	ChangeLog \
	COPYING \
	AUTHORS

# Packages themes...

install-data-hook:
	$(MKDIR_P) "$(themesdir)/foom"
	$(MKDIR_P) "$(themesdir)/oxygen"
	$(MKDIR_P) "$(themesdir)/faenza"
	$(INSTALL_DATA) data/themes/foom/* '$(themesdir)/foom'
	$(INSTALL_DATA) data/themes/faenza/* '$(themesdir)/faenza'
	$(INSTALL_DATA) data/themes/oxygen/* '$(themesdir)/oxygen'
	$(INSTALL_DATA) data/themes/minimal-bg.png '$(themesdir)'
	$(INSTALL_DATA) data/themes/spacefun-bg.png '$(themesdir)'
	$(INSTALL_DATA) data/themes/foom.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/foom-minimal.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/foom-spacefun.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/oxygen.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/oxygen-minimal.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/oxygen-spacefun.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/faenza.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/faenza-minimal.rc '$(themesdir)'
	$(INSTALL_DATA) data/themes/faenza-spacefun.rc '$(themesdir)'

uninstall-hook:
	rm -rf $(maindir)
