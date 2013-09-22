AC_PREREQ(2.61)
AC_INIT([fs_mon], [0.1], [paul.ionkin@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h]) 

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([DEPS], [glib-2.0 >= 2.22 gobject-2.0 gio-2.0 >= 0.9])



# check if we should enable verbose debugging 
AC_ARG_ENABLE(debug-mode,
     AS_HELP_STRING(--enable-debug-mode, enable support for running in debug mode),
        [], [enable_debug_mode=no])
if test x$enable_debug_mode = xyes; then
    CFLAGS="$CFLAGS -Wall -Wextra -O0 -Wdeclaration-after-statement -Wredundant-decls -Wmissing-noreturn -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Winline -Wformat-nonliteral -Wformat-security -Wswitch-default -Winit-self -Wmissing-include-dirs -Wundef -Waggregate-return -Wmissing-format-attribute -Wnested-externs -Wstrict-overflow=5 -Wformat=2 -Wunreachable-code -Wfloat-equal -ffloat-store -g -ggdb3  -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
else
    CFLAGS="-O2"
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
