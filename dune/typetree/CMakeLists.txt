set(typetreedir  ${CMAKE_INSTALL_INCLUDEDIR}/dune/typetree)

set(typetree_HEADERS
  accumulate_static.hh
  applytochildrensingletree.hh
  applytochildrentreepair.hh
  childextraction.hh
  compositenode.hh
  exceptions.hh
  filteredcompositenode.hh
  filters.hh
  fixedcapacitystack.hh
  generictransformationdescriptors.hh
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
  treepath.hh
  typetraits.hh
  typetree.hh
  utility.hh
  visitor.hh)


dune_add_library(dunetypetree
  utility.cc
  ADD_LIBS ${DUNE_LIBS})

# include not needed for CMake
# include $(top_srcdir)/am/global-rules

install(FILES ${typetree_HEADERS} DESTINATION ${typetreedir})
