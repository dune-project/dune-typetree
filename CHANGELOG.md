TypeTree
========

This is the 2.4.0-rc1 version of the TypeTree library for statically typed object trees.

TypeTree 2.4.0-rc1 is compatible with the 2.4.0 release of the DUNE core modules and
the 2.4.0 release of PDELab.

Changes
=======

TypeTree 2.4
------------

- TypeTree has updated its minimum compiler requirements. You now need a compiler that is at
  least compatible with GCC 4.7 in C++11 mode.

- A lot of compatibility code for older compilers has been removed. In particular, the old
  CompositeNode with a fixed number of children has been removed and the old VariadicCompositeNode
  has been renamed to CompositeNode.

- As part of these changes, we were able to remove libdunetypetree, so TypeTree is now a header-only
  library. Thanks, Ansgar!

- In addition to the `DynamicTreePath` and the fully static `TreePath`, there is now a `HybridTreePath`,
  which allows users to use a runtime index in some places of the path and a compile time index at others.

- As part of this new `HybridTreePath`, there is a backport of the C++14 index_sequence to C++11 in the namespace
  `Dune::TypeTree::Std`. Moreover, there is an `index_constant<i>`, which is an alias template for
  `std::integral_constant<std::size_t,i>`, and a number of predefined `constexpr` objects `Dune::TypeTree::Indices::_0`
  etc, which are just `index_constant`s of the specified value. These exist up to the index 19 for now.

- Based on the new `HybridTreePath` and those `index_constant`s, there is now a much more user-friendly way of
  extracting child nodes: All interior nodes now have a method `child()` which accepts either
  - a sequence of integers and `index_constant`s
  - a `HybridTreePath`
  - a `TreePath`
  and returns the subtree rooted at that path. There is also a freestanding function `child(node,path)`, which should be
  used in generic algorithm code as it is more robust (it can e.g. deal correctly with an empty path).

- The old child extraction using `extract_child()` has been deprecated.

- Lots of smaller fixes

- Release history

  - TypeTree 2.4.0
    - Fix rendering of links in README

  - TypeTree 2.4.0-rc1
    - Initial release candidate


TypeTree 2.3
------------

This release only contains minor changes to fix buildsystem issues and fix a number of
bugs.

- Full support for CMake (thanks to Christoph and Markus!).

- Fixes to Autotools tests.

- Resolved some compilation problems on older compilers.

- Release history

  TypeTree 2.3.1
  - Release without further changes

  TypeTree 2.3.1-rc1
  - Small fixes to included headers and documentation

  TypeTree 2.3.0
  - Skipped because version 2.3.1 of the core modules was released in the meantime
    and it was considered better to keep the version numbers in sync



TypeTree 1.0
------------

This is the initial standalone release. It only contains changes due to the extraction
from the PDELab sources.

- Moved headers from dune/pdelab/common/typetree/ to dune/typetree/.

- Changed namespace from Dune::PDELab::TypeTree to Dune::TypeTree.

- Moved tests to top-level directory tests/.
