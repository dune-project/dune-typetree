Changes
=======

TypeTree 2.8-git
----------------

-   Removed the type aliases for the storage type `Storage` and `ConstStorage` in the
    node implementations.
-   The memberfunction `childStorage()` in the nodes now consistently take an index or an
    index_constant as argument.


TypeTree 2.7
------------

-   The class template `index_pack` and the utility functionality for its creation have been
    removed. Use `std::index_sequence` instead.
-   `forEachNode()`, `applyToTree()` and `applyToTreePair()` have been reimplemented.
    The user interface and functionality is the same. However, code using removed internal
    implementation details of the old implementation will fail.
-   `HybridTreePath` gained new member functions `size()`,
    `element(Dune::index_constant<i>)`, `element(std::size_t)`,
    `operator[](Dune::index_constant<i>)`, and `operator[](std::size_t)`
    to be compatible with `DynamicTreePath`.
-   A `HybridTreePath` can now be constructed using the
    global `treePath()` function which is an alias for `hybridTreePath()`.
-   `HybridTreePath` now supports all static operations of the old
    `TreePath`. The `TreePath<i,...>` is thus only an alias to
    `HybridTreePath<integral_constant<i>,...> and is deprecated (i.e. will
    be removed after TypeTree 2.7)
-   A convenience alias `StaticTreePath` is introduced, which shuld be
    used instead of the `TypeTree`, if the path is known to be static.
-   The type alias template `Child<>` now fails to instantiate if the child cannot be extracted.
    This is much more useful than the previous behavior of just defaulting to `void` in case of
    failure.


TypeTree 2.6
------------

-   TypeTree has updated its minimum build toolchain requirements. You now need a compiler that is at
    least compatible with GCC 5 in C++14 mode and CMake 3.1.0.

-   There is a new, simpler way of applying a functor to each tree node that keeps you from having
    to write a visitor by yourself, e.g.:

    ```c++
    int i = 0;
    forEachNode(tree, [](auto&&... node) {
      ++i;
    });
    ```

    Thanks to Carsten Gräser for contributing this feature!



TypeTree 2.5
------------

- TypeTree has updated its minimum compiler requirements. You now need a compiler that is at
  least compatible with GCC 4.9 in C++14 mode.

- Removed support for Autotools.


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
