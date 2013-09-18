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

AC_ARG_WITH([fam],
			[  --with-fam=<path>      prefix of fam installation (eg /usr/local)],
		[case "${withval}" in
			  yes) with_fam="/usr";;
			  no)  with_fam="no";;
			  *)   with_fam="/usr";;
			 esac],
			 [with_fam="/usr"])

	CFLAGS="$CFLAGS -I $with_fam/include"
	LDFLAGS="$LDFLAGS -L $with_fam/lib"
	AC_CHECK_HEADERS(fam.h, AC_SEARCH_LIBS([FAMOpen], [fam]),
        AC_MSG_WARN([*** fam.h not found -- consider using --with-fam]))

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
