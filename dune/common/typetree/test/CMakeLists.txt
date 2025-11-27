# SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
# SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-DUNE-exception

link_libraries(Dune::TypeTree)

dune_add_test(SOURCES testtypetreechildaccess.cc)
dune_add_test(SOURCES testtypetreenodeconcepts.cc)
dune_add_test(SOURCES testtypetreetraversal.cc)
dune_add_test(SOURCES testtypetreetreecontainer.cc)
dune_add_test(SOURCES testtypetreetreepath.cc)
