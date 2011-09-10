AC_PREREQ([2.54])
AC_INIT([obshutdown], [0.1-rc1], [panjandrummail@gmail.com])
AM_INIT_AUTOMAKE([obshutdown], [0.1-rc1])

AC_CONFIG_SRCDIR([src/main.c])

AC_PREFIX_DEFAULT([/usr/local])
test "$prefix" = "NONE" && prefix=$ac_default_prefix

# AM_PROG_CC_C_O
AC_C_CONST
AC_C_INLINE

AC_PROG_INSTALL

CHECK_EXPORT_DYNAMIC
LDFLAGS="$LDFLAGS $EXPORT_DYNAMIC_FLAGS"

# ALL_LINGUAS=""
# AM_GNU_GETTEXT_VERSION(0.15)
# AM_GNU_GETTEXT([external])

PKG_CHECK_MODULES(GTK, [gtk+-2.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(CAIRO, [cairo])
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

AC_CHECK_PROG(TAR, [tar], [yes], [no])
if test "$TAR" = "no"; then
  AC_MSG_ERROR([Unable to find the tar command.])
fi

AC_CHECK_PROG(GZIP, [gzip], [yes], [no])
if test "$GZIP" = "no"; then
  AC_MSG_ERROR([Unable to find the gzip command.])
fi

AC_CHECK_PROG(GUNZIP, [gunzip], [yes], [no])
if test "$GUNZIP" = "no"; then
  AC_MSG_ERROR([Unable to find the gunzip command.])
fi

AC_CONFIG_FILES([
  Makefile
])
AC_OUTPUT

AC_MSG_RESULT
AC_MSG_RESULT([configure complete, now type \"make\"])
