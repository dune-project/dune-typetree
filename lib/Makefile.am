#the dune-typetree library
lib_LTLIBRARIES = libdunetypetree.la

# this is just a renaming of libtypetreecommon to prevent name clashes
libdunetypetree_la_SOURCES =
# This forces automake to use the C++ linker
# (see the automake manual, section "Libtool Convenience Libraries")
nodist_EXTRA_libdunetypetree_la_SOURCES = dummy.cc
sourcescheck_DUMMY = dummy.cc
libdunetypetree_la_LIBADD = ../dune/typetree/libtypetree.la

include $(top_srcdir)/am/global-rules

EXTRA_DIST = CMakeLists.txt
