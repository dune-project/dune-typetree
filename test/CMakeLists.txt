exclude_from_headercheck(
  typetreetargetnodes.hh
  typetreetestswitch.hh
  typetreetestutility.hh)

dune_add_test(SOURCES testtypetree.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

dune_add_test(SOURCES testtypetreetransformation.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

dune_add_test(SOURCES testfilteredcompositenode.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

dune_add_test(SOURCES testproxynode.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)
