# SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
# SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

install(FILES
  accumulate_static.hh
  childextraction.hh
  compositenode.hh
  dynamicpowernode.hh
  exceptions.hh
  filteredcompositenode.hh
  filters.hh
  fixedcapacitystack.hh
  generictransformationdescriptors.hh
  hybridmultiindex.hh
  leafnode.hh
  nodeinterface.hh
  nodetags.hh
  pairtraversal.hh
  powercompositenodetransformationtemplates.hh
  powernode.hh
  proxynode.hh
  simpletransformationdescriptors.hh
  transformation.hh
  transformationutilities.hh
  traversal.hh
  traversalutilities.hh
  treecontainer.hh
  treepath.hh
  typetraits.hh
  typetree.hh
  utility.hh
  visitor.hh
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/typetree)
