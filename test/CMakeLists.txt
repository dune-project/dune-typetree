#defined empty so we can add to it later
set(MOSTLYCLEANFILES)

set(noinst_HEADERS
  typetreetargetnodes.hh
  typetreetestswitch.hh
  typetreetestutility.hh)


set(noinst_SCRIPTS
  make_pvd.sh)

exclude_from_headercheck(
  typetreetargetnodes.hh
  typetreetestswitch.hh
  typetreetestutility.hh)

# scripts are not distributed automatically
# but since this is need for make all:
set(EXTRA_DIST  make_pvd.sh)

list(APPEND TESTS testtypetree)
add_executable(testtypetree testtypetree.cc)
set_property(TARGET testtypetree APPEND PROPERTY COMPILE_DEFINITIONS TEST_TYPETREE)

list(APPEND TESTS testtypetreetransformation)
add_executable(testtypetreetransformation testtypetreetransformation.cc)
set_property(TARGET testtypetreetransformation APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE)

list(APPEND TESTS testfilteredcompositenode)
add_executable(testfilteredcompositenode testfilteredcompositenode.cc)
set_property(TARGET testfilteredcompositenode APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE)

list(APPEND TESTS testproxynode)
add_executable(testproxynode testproxynode.cc)
set_property(TARGET testproxynode APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE)

foreach(_test ${TESTS})
  add_test(${_test} ${_test})
  target_link_libraries(${_test} dunetypetree ${DUNE_LIBS})
endforeach()

# We do not want want to build the tests during make all
# but just build them on demand
add_directory_test_target(_test_target)
add_dependencies(${_test_target} ${TESTS})
