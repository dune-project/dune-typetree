#defined empty so we can add to it later
set(NORMALTESTS  )
set(MOSTLYCLEANFILES )
set(check_SCRIPTS  )

set(noinst_HEADERS
  typetreetargetnodes.hh
  typetreetestswitch.hh
  typetreetestutility.hh)


set(noinst_SCRIPTS
  make_pvd.sh)

set(headercheck_IGNORE
  typetreetargetnodes.hh
  typetreetestswitch.hh
  typetreetestutility.hh)

# scripts are not distributed automatically
# but since this is need for make all:
set(EXTRA_DIST  make_pvd.sh)

list(APPEND check_PROGRAMS testtypetree testtypetree-no-sfinae testtypetree-no-variadic testtypetree-no-rvalue-refs testtypetree-legacy)
add_executable(testtypetree testtypetree.cc)
set_property(TARGET testtypetree APPEND PROPERTY COMPILE_DEFINITIONS TEST_TYPETREE)
target_link_libraries(testtypetree dunepdelab ${DUNE_LIBS})

add_executable(testtypetree_no_sfinae testtypetree.cc)
set_property(TARGET testtypetree APPEND PROPERTY COMPILE_DEFINITIONS TEST_TYPETREE_NO_SFINAE)
target_link_libraries(testtypetree_no_sfinae dunepdelab ${DUNE_LIBS})

add_executable(testtypetree_no_variadic testtypetree.cc)
set_property(TARGET testtypetree_no_variadic APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE_NO_VARIADIC)
target_link_libraries(testtypetree_no_variadic dunepdelab ${DUNE_LIBS})

add_executable(testtypetree_no_rvalue_refs testtypetree.cc)
set_property(TARGET testtypetree_no_rvalue_refs APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE_NO_RVALUE_REFERENCES)
target_link_libraries(testtypetree_no_rvalue_refs dunepdelab ${DUNE_LIBS})

add_executable(testtypetree_legacy testtypetree.cc)
set_property(TARGET testtypetree_legacy APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE_LEGACY)
target_link_libraries(testtypetree_legacy dunepdelab ${DUNE_LIBS})

list(APPEND check_PROGRAMS testtypetreetransformation)
add_executable(testtypetreetransformation testtypetreetransformation.cc)
set_property(TARGET testtypetreetransformation APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE)
target_link_libraries(testtypetreetransformation dunepdelab ${DUNE_LIBS})

list(APPEND check_PROGRAMS testfilteredcompositenode)
add_executable(testfilteredcompositenode testfilteredcompositenode.cc)
set_property(TARGET testfilteredcompositenode APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE)
target_link_libraries(testfilteredcompositenode dunepdelab ${DUNE_LIBS})

list(APPEND check_PROGRAMS testproxynode)
add_executable(testproxynode testproxynode.cc)
target_link_libraries(testproxynode dunepdelab ${DUNE_LIBS})
set_property(TARGET testproxynode APPEND PROPERTY
  COMPILE_DEFINITIONS TEST_TYPETREE)
target_link_libraries(testproxynode dunepdelab ${DUNE_LIBS})

set(DISTCHECK_CONFIGURE_FLAGS  --with-dune=${DUNEROOT} CXX="${CXX}" CC="${CC}")

foreach(i ${TESTS} ${NORMALTESTS})
  add_test(${i} ${i})
endforeach(i ${TESTS})

# programs just to build when "make check" is used
set(check_PROGRAMS  ${check_PROGRAMS} ${NORMALTESTS})

# We do not want want to build the tests during make all
# but just build them on demand
add_directory_test_target(_test_target)
add_dependencies(${_test_target} ${TESTS} ${COMPILE_XFAIL_TESTS} ${check_PROGRAMS})
