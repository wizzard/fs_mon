AC_PREREQ(2.61)
AC_INIT([fs_mon], [0.1], [paul.ionkin@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define foreign subdir-objects])
AC_USE_SYSTEM_EXTENSIONS
AC_DEFINE(_GNU_SOURCE)
AC_PROG_MAKE_SET

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_SIZEOF(long int)
AC_C_CONST
AC_C_INLINE
AC_LANG(C)

# Checks for header files.
AC_CHECK_HEADER([sys/inotify.h], 
                [AC_CHECK_FUNC([inotify_init],[have_inotify="yes"],[have_inotify="no"])], 
                [have_inotify="no"]) 

if test "x$have_inotify" = "no"; then 
  AC_MSG_ERROR("inotify not found!") 
fi 

# check if we should enable verbose debugging 
AC_ARG_ENABLE(debug-mode,
     AS_HELP_STRING(--enable-debug-mode, enable support for running in debug mode),
        [], [enable_debug_mode=no])
if test x$enable_debug_mode = xyes; then
    CFLAGS="$CFLAGS -Wall -Wextra -O0 -Wdeclaration-after-statement -Wredundant-decls -Wmissing-noreturn -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Winline -Wformat-nonliteral -Wformat-security -Wswitch-default -Winit-self -Wmissing-include-dirs -Wundef -Waggregate-return -Wmissing-format-attribute -Wnested-externs -Wstrict-overflow=5 -Wformat=2 -Wunreachable-code -Wfloat-equal -ffloat-store -g -ggdb3  -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
else
    CFLAGS="-O2"
fi

AC_CONFIG_HEADERS([config.h]) 
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
