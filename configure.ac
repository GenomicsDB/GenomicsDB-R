AC_INIT([genomicsdb], 0.0.2)

# Only MacOS and Linux platforms are supported
AC_CHECK_PROG([UNAME], [uname], [yes], [no])
AS_IF([test "$UNAME" = "no"],
      [AC_MSG_ERROR([Only MacOS and Linux platforms are supported])])

AC_MSG_CHECKING([for OS platform])
OS_PLATFORM=`uname -s`
case "${OS_PLATFORM}" in
     Linux*)
         BUILD_PLATFORM=Linux
         ;;
     Darwin*)
         BUILD_PLATFORM=Darwin
         ;;
     *)
         AC_MSG_ERROR([Only MacOS and Linux platforms are supported])
         ;;
esac
AC_MSG_RESULT([$BUILD_PLATFORM])

AC_LANG(C++)
AC_REQUIRE_CPP
AC_PROG_CXX

# Default values
AC_SUBST([GENOMICSDB_INSTALL_PATH], ${GENOMICSDB_HOME})
AC_SUBST([GENOMICSDB_LIBS], "-ltiledbgenomicsdb")

# Allow for overrides
AC_ARG_WITH([genomicsdb],
            AC_HELP_STRING([--with-genomicsdb=[DIR]],
                           [GenomicsDB root install path [PREFIX]]),
            [
             GENOMICSDB_INSTALL_PATH="${with_genomicsdb}"
            ],
            [])

# Check GENOMICSDB_INSTALL_PATH passed as either $GENOMICSDB_HOME or --with-genomicsdb
AS_IF([test "unset$GENOMICSDB_INSTALL_PATH" = "unset"],
      [AC_MSG_ERROR([GenomicsDB installation not found.
        Invoke configure with either \$GENOMICSDB_HOME env set to the installation path or
        by explicitly specifying the path with the --with-genomicsdb argument])])

# Sanity check installation
AC_CHECK_FILES([${GENOMICSDB_INSTALL_PATH}/include/genomicsdb.h
                ${GENOMICSDB_INSTALL_PATH}/include/genomicsdb_exception.h],
               [],
               [AC_MSG_ERROR([GenomicsDB include files not found])])

GENOMICSDB_LIBS="${GENOMICSDB_LIBS} -Wl,-rpath,${GENOMICSDB_INSTALL_PATH}/lib"

# create and report output
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT

echo
echo "Final src/Makevars"
cat src/Makevars
