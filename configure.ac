AC_INIT([genomicsdb], 0.0.2)

AC_LANG(C++)
AC_REQUIRE_CPP
AC_PROG_CXX

# default values
if test "${GENOMICSDB_HOME+set}" != set; then
  echo "variable \$GENOMICSDB_HOME is not set"
  GENOMICSDB_HOME="/usr/local"
fi
AC_SUBST([GENOMICSDB_INSTALL_PATH], ${GENOMICSDB_HOME})
AC_SUBST([GENOMICSDB_LIBS], "-ltiledbgenomicsdb")

# allow for overrides
AC_ARG_WITH([genomicsdb],
            AC_HELP_STRING([--with-genomicsdb=[DIR]],
                           [GenomicsDB root install path [PREFIX]]),
            [
             GENOMICSDB_INSTALL_PATH="${with_genomicsdb}"
            ],
            [])

GENOMICSDB_LIBS="${GENOMICSDB_LIBS} -Wl,-rpath,${GENOMICSDB_INSTALL_PATH}/lib"

# create and report output
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT

echo
echo "Final src/Makevars"
cat src/Makevars
