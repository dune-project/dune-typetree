# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.50)
DUNE_AC_INIT # gets module version from dune.module file
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([dune/typetree/nodetags.hh])
AC_CONFIG_HEADERS([config.h])

# add configure flags needed to create log files for dune-autobuild
DUNE_AUTOBUILD_FLAGS
#
# we need no more than the standard DE-stuff
# this module depends on dune-common
# this implies checking for [dune-common]
DUNE_CHECK_ALL

# implicitly set the Dune-flags everywhere
AC_SUBST(AM_CPPFLAGS, '$(DUNE_CPPFLAGS) -I$(top_srcdir)')
AC_SUBST(AM_LDFLAGS, $DUNE_LDFLAGS)
AC_SUBST([LDADD], ['$(top_builddir)/lib/libdunetypetree.la $(DUNE_LIBS)'])

AC_CONFIG_FILES([
  cmake/Makefile
  cmake/modules/Makefile
  cmake/pkg/Makefile
  doc/doxygen/Doxyfile
  doc/doxygen/Makefile
  doc/Makefile
  dune-typetree.pc
  dune/Makefile
  dune/typetree/Makefile
  test/Makefile
  lib/Makefile
  m4/Makefile
  Makefile
])
AC_OUTPUT
# finally print the summary information
DUNE_SUMMARY_ALL
