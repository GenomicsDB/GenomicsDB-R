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

# find R home and set FLAGS
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
RBIN="${R_HOME}/bin/R"

CPPFLAGS=`"${RBIN}" CMD config CPPFLAGS`
CXX11STD=`"${RBIN}" CMD config CXX11STD`
CXXFLAGS=`"${RBIN}" CMD config CXX11FLAGS`

AC_LANG(C++)
AC_REQUIRE_CPP
AC_PROG_CXX

# Default values
AC_SUBST([GENOMICSDB_INSTALL_PATH], ${GENOMICSDB_HOME})
AC_SUBST([GENOMICSDB_LIBS], "-ltiledbgenomicsdb")

echo "Nalini"
echo "${GENOMICSDB_HOME}"
echo "Nalini 1"

# Allow for overrides
AC_ARG_WITH([genomicsdb],
            AC_HELP_STRING([--with-genomicsdb=[DIR]],
                           [GenomicsDB root install path [PREFIX]]),
            [GENOMICSDB_INSTALL_PATH="${with_genomicsdb}"],
            [])

GENOMICSDB_LIBS="${GENOMICSDB_LIBS} -Wl,-rpath,${GENOMICSDB_INSTALL_PATH}/lib"

# Check existence of GENOMICSDB_INSTALL_PATH
AS_IF([test "unset$GENOMICSDB_INSTALL_PATH" = "unset"],
      [AC_MSG_ERROR([GenomicsDB installation not found.
        Invoke with either \$GENOMICSDB_HOME env set to the installation path or
        by explicitly specifying the path with the --with-genomicsdb argument])])

# Sanity check installation
AC_CHECK_FILES([${GENOMICSDB_INSTALL_PATH}/include/genomicsdb.h
                ${GENOMICSDB_INSTALL_PATH}/include/genomicsdb_exception.h],
               [],
               [AC_MSG_ERROR([GenomicsDB include files not found])])

# Try compiling with GenomicsDB headers
CPPFLAGS="${CPPFLAGS} ${CXX11STD} ${CXXFLAGS} -I${GENOMICSDB_INSTALL_PATH}/include"
AC_TRY_COMPILE([#include <genomicsdb.h>],
               [],
               [],
               [AC_MSG_ERROR([unable to compile with genomicsdb.h])])

# Check GenomicsDB installation
LDFLAGS="${LD_FLAGS} -L${GENOMICSDB_INSTALL_PATH}/lib"
LIBS="${LIBS} -ltiledbgenomicsdb"
AC_MSG_CHECKING([whether genomicsdb works])
AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <genomicsdb.h>
int main() {
  if (genomicsdb_version().size() > 0)
    return 0;
  else
    return -1;
}]])],
               [AC_MSG_RESULT([yes])],
               [AC_MSG_ERROR([GenomicsDB installation at ${GENOMICSDB_INSTALL_PATH} is not functional])])

# create and report output
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT

echo
echo "Final src/Makevars"
cat src/Makevars
