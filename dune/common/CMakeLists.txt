# SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
# SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

add_subdirectory(typetree)

install(FILES
  hybridmultiindex.hh
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/common)
