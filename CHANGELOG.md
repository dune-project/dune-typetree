<!--
SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception
-->

Changes
=======

TypeTree 2.12-git
-----------------

- ...

TypeTree 2.11
-------------

- A modernized lightweight subset of the typetree interface has been
  extracted to the new subdirectory `dune/common/typetree/` in
  the dune-common module. This subset includes the node concepts,
  the child access via `Child`, `ChildForTreePath`, and `child()`,
  the traversal via the `forEach*()`, the `Dune::HybridMultiIndex`
  class and its alias `Dune::TypeTree::TreePath`, and the utilities
  from `treecontainer.hh`. This subset can be used independently
  of the dune-typetree module and in particular without deriving
  from the node base classes that have not been moved. Downstream
  modules and applications that only relies on this functionality
  no longer need to depend on the module dune-typetree.
- The name `TreePath` has been reintroduced as alias for `HybridMultiIndex`
  and `HybridTreePath`.
- A new algorithm `forEachChild()` has been added, which
  only loops over direct children.
- The requirements of tree nodes have been formalized as
  concepts `TreeNode`, `InnerTreeNode`, `UniformInnerTreeNode`,
  and `StaticDegreeInnerTreeNode`. When using the `forEach...`
  algorithms of dune-typetree, one no longer has to derive
  node types from the node base classes. It is sufficient
  to implement the respective concepts.
- The method `Dune::HybridMultiIndex::element()` has been deprecated.
  Use `operator[]` instead.
- The class `Dune::TypeTree::HybridTreePath` has been extracted
  into `Dune::HybridMultiIndex`. The old class persists as type alias.
- `HybridTreePath` no longer supports entry types other than
  `std::size_t` and `std::integral_constant<std::size_t,i>`.

TypeTree 2.10
----------------

- `HybridTreePath` functions now have `[[nodiscard]]` attribute
- `HybridTreePath` gained several multi-index utilities:
  `max_size`, `join`, `reverse`, and `accumulate_[front|back]`.
- Add user-defined literal `_tp` to generate `HybridTreePath`

Deprecations and removals

- Remove deprecated `TreePath`, use `StaticTreePath` instead.
- Remove deprecated `CHILDREN` from `CompositeNode`, `FilteredCompositeNode`, `LeafNode`,
  `PowerNode`, `ProxyNode`. Use `degree()` instead.
- Remove deprecated `forEachNode`, use `applyToTree` instead.

TypeTree 2.9
----------------

-   Remove `apply_to_tuple` utility.
-   Add support for `ProxyNode` on nodes with dynamic degree.
-   The static member variable `CHILDREN` is deprecated in favor of the function `degree()`.
-   Add comparison operator for `HybridTreePath`.

TypeTree 2.8
------------

-   Add support for tree transformations for `DynamicPowerNode`.
-   A power node with runtime degree, `DynamicPowerNode`, is added.
-   Allow tree traversal for nodes with runtime degree that are not necessarily power nodes.
-   The `forEachNode()` traversal function with multiple callbacks is deprecated. Use the
    more general `applyToTree()` instead.
-   Removed the type aliases for the storage type `Storage` and `ConstStorage` in the
    node implementations.
-   The memberfunction `childStorage()` in the nodes now consistently take an index or an
    `index_constant` as argument.


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
    `HybridTreePath<integral_constant<i>,...>` and is deprecated (i.e. will
    be removed after TypeTree 2.7)
-   A convenience alias `StaticTreePath` is introduced, which should be
    used instead of the deprecated `TreePath` alias, if the path is known to be static.
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
