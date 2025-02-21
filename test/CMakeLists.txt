# SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
# SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

link_libraries(Dune::TypeTree)

exclude_from_headercheck(
  typetreetargetnodes.hh
  typetreetestswitch.hh
  typetreetestutility.hh)

dune_add_test(SOURCES testtypetree.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

foreach(TEST_FAILURE 2 3 5 6)
  dune_add_test(NAME testtypetree_fail${TEST_FAILURE}
                SOURCES testtypetree.cc
                COMPILE_DEFINITIONS TEST_TYPETREE;FAILURE${TEST_FAILURE}
                EXPECT_COMPILE_FAIL)
endforeach()

dune_add_test(SOURCES testtypetreetransformation.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

dune_add_test(SOURCES testfilteredcompositenode.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

dune_add_test(SOURCES testproxynode.cc
              COMPILE_DEFINITIONS TEST_TYPETREE)

dune_add_test(SOURCES testcallbacktraversal.cc)

dune_add_test(SOURCES testhybridtreepath.cc)

dune_add_test(SOURCES testtreecontainer.cc)
