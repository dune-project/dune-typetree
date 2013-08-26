
#defined empty so we can add to it later
NORMALTESTS =
MOSTLYCLEANFILES =
check_SCRIPTS =


# AM_CPPFLAGS = @AM_CPPFLAGS@				\
# 	$(UG_CPPFLAGS)					\
# 	$(ALUGRID_CPPFLAGS)				\
# 	$(AMIRAMESH_CPPFLAGS)				\
# 	$(DUNE_CPPFLAGS)				\
# 	$(DUNEMPICPPFLAGS)				\
# 	$(PARMETIS_CPPFLAGS)
# AM_LDFLAGS = @AM_LDFLAGS@			\
# 	$(UG_LDFLAGS)				\
# 	$(ALUGRID_LDFLAGS)			\
# 	$(AMIRAMESH_LDFLAGS)			\
# 	$(DUNE_LDFLAGS)				\
# 	$(DUNEMPILDFLAGS)			\
# 	$(PARMETIS_LDFLAGS)
# LDADD =						\
# 	$(PARMETIS_LIBS)			\
# 	$(ALUGRID_LIBS)				\
# 	$(AMIRAMESH_LIBS)			\
# 	$(DUNE_LIBS)				\
# 	$(DUNEMPILIBS)				\
# 	$(UG_LIBS)				\
# 	@LDADD@
#
noinst_HEADERS =				\
  typetreetargetnodes.hh			\
  typetreetestswitch.hh				\
  typetreetestutility.hh

headercheck_IGNORE =				\
  typetreetargetnodes.hh			\
  typetreetestswitch.hh				\
  typetreetestutility.hh

# scripts are not distributed automatically
# but since this is need for make all:
EXTRA_DIST = CMakeLists.txt


NORMALTESTS += testtypetree testtypetree-no-sfinae testtypetree-no-variadic testtypetree-no-rvalue-refs testtypetree-legacy
testtypetree_SOURCES = testtypetree.cc
testtypetree_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE
testtypetree_no_sfinae_SOURCES = testtypetree.cc
testtypetree_no_sfinae_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE_NO_SFINAE
testtypetree_no_variadic_SOURCES = testtypetree.cc
testtypetree_no_variadic_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE_NO_VARIADIC
testtypetree_no_rvalue_refs_SOURCES = testtypetree.cc
testtypetree_no_rvalue_refs_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE_NO_RVALUE_REFERENCES
testtypetree_legacy_SOURCES = testtypetree.cc
testtypetree_legacy_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE_LEGACY

NORMALTESTS += testtypetreetransformation
testtypetreetransformation_SOURCES = testtypetreetransformation.cc
testtypetreetransformation_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE

NORMALTESTS += testfilteredcompositenode
testfilteredcompositenode_SOURCES = testfilteredcompositenode.cc
testfilteredcompositenode_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE

NORMALTESTS += testproxynode
testproxynode_SOURCES = testproxynode.cc
testproxynode_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_TYPETREE

# list of tests to run
TESTS = $(NORMALTESTS)

# programs just to build when "make check" is used
check_PROGRAMS = $(NORMALTESTS)


include $(top_srcdir)/am/global-rules

DISTCHECK_CONFIGURE_FLAGS = --with-dune=$(DUNEROOT) CXX="$(CXX)" CC="$(CC)"
