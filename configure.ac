AC_INIT([chirp2djmd5], [0.1], [ultrapod333@gmail.com])
AM_INIT_AUTOMAKE
AC_PROG_CXX
CPPFLAGS="-I./include"

AC_CHECK_HEADERS([csv.h],
        [found_csv_headers=yes; break;])


# Check for libcsv and its headers (apt-get install libcsv-dev)
AS_IF([test "x$found_csv_headers" != "xyes"],
        [AC_MSG_ERROR([*** Unable to find csv.h for libcsv])])

AC_CHECK_LIB([csv],
        [csv_parse],
        LIBS="$LIBS -lcsv",
        AC_MSG_ERROR([*** libcsv library not found!]))

AC_CHECK_LIB([jsoncpp],
        [_ZNK4Json5Value8isStringEv],
        LIBS="$LIBS -ljsoncpp",
        AC_MSG_ERROR([*** libjsoncpp library not found!]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
