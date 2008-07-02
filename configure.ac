
AC_INIT(Jigsawn, 0.0.1, fraggle@users.sourceforge.net, jigsawn)
AC_CONFIG_AUX_DIR(autotools)

AM_INIT_AUTOMAKE($PACKAGE_TARNAME, $PACKAGE_VERSION, no-define)

AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Support for running test cases using valgrind:

use_valgrind=false
AC_ARG_ENABLE(valgrind,
[  --enable-valgrind       Use valgrind when running unit tests. ],
[ use_valgrind=true ])

if [[ "$use_valgrind" = "true" ]]; then
        AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)

        if [[ "$HAVE_VALGRIND" = "no" ]]; then
                AC_MSG_ERROR([Valgrind not found in PATH. ])
        fi
fi

AM_CONDITIONAL(USE_VALGRIND, $use_valgrind)

AC_OUTPUT([
    Makefile
    src/Makefile
    src/include/Makefile
    src/include/jigsawn/Makefile
    test/Makefile
])

