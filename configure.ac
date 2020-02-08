AC_INIT([genomicsdb], 0.0.2)

AC_LANG(C++)
AC_REQUIRE_CPP
AC_PROG_CXX

# default values
echo "Using -I${HOME}/GenomicsDB/src/main/cpp/include for CPPFLAGS. Set env GENOMICSDB_HOME to override"
AC_SUBST([GENOMICSDB_INSTALL_PATH], "")
AC_SUBST([GENOMICSDB_LIBS], "-ltiledbgenomicsdb")

# allow for overrides
AC_ARG_WITH([genomicsdb],
            AC_HELP_STRING([--with-genomicsdb=[DIR]],
                           [GenomicsDB root install path [PREFIX]]),
            [
             GENOMICSDB_INSTALL_PATH="${with_genomicsdb}"
	     GENOMICSDB_LIBS="${GENOMICSDB_LIBS} -Wl,-rpath,${with_genomicsdb}/lib"
            ],
            [])

# create and report output
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT

CPPFLAGS="${CPPFLAGS} ${GENOMICSDB_INSTALL_PATH}/include"
LDFLAGS="${LDFLAGS} ${GENOMICSDB_INSTALL_PATH}/lib ${GENOMICSDB_LIBS}"

echo
echo "Final src/Makevars"
cat src/Makevars
