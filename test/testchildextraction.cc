// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#include <cstdlib>

#include <dune/common/typetree/childaccess.hh>

// Bring concepts into scope
using Dune::TypeTree::Concept::TreeNode;
using Dune::TypeTree::Concept::InnerTreeNode;
using Dune::TypeTree::Concept::StaticDegreeInnerTreeNode;
using Dune::TypeTree::Concept::UniformInnerTreeNode;
using Dune::TypeTree::Concept::LeafTreeNode;
using namespace Dune::Indices;

// Leaf node
template<int tag>
struct Leaf {
  static constexpr auto degree() { return _0; }
};


// Inner node with dynamic degree
template<std::size_t n, class C>
struct DynamicInner {
  auto degree() const { return n; }
  const auto& child(std::size_t i) const { return children_[i]; };
  std::array<C, n> children_;
};

// Inner node with static degree
template<class... CC>
struct StaticInner {
  static constexpr auto degree() { return Dune::index_constant<sizeof...(CC)>{}; }
  template<std::size_t i>
  const auto& child(Dune::index_constant<i>) const { return std::get<i>(children_); }
  std::tuple<CC...> children_;
};



template<class Tree, class Child, std::size_t... i>
void checkChild() {

  auto tree = Tree{};
  using TreePath = Dune::HybridMultiIndex<Dune::index_constant<i>...>;
  auto treePath = TreePath{};

  using ChildByIndices = Dune::TypeTree::Child<Tree, i...>;
  static_assert(std::is_same_v<Child, ChildByIndices>);

  using ChildByTreePath = Dune::TypeTree::ChildForTreePath<Tree, TreePath>;
  static_assert(std::is_same_v<Child, ChildByTreePath>);

  const auto& childByIndices = Dune::TypeTree::child(tree, Dune::index_constant<i>{}...);
  static_assert(std::is_same_v<Child, std::decay_t<decltype(childByIndices)>>);

  const auto& childByTreePath = Dune::TypeTree::child(tree, treePath);
  static_assert(std::is_same_v<Child, std::decay_t<decltype(childByTreePath)>>);
}

template<class Tree, class... Child>
void checkChildren() {
  using DeducedChildren = Dune::TypeTree::Impl::Children<Tree>;
  if constexpr (sizeof...(Child)>0)
    static_assert(std::is_same_v<DeducedChildren, std::tuple<Child...>>);
  else
    static_assert(std::is_same_v<DeducedChildren, void>);
}

int main() {

  using Tree = StaticInner<
      DynamicInner<3,
        Leaf<23>
      >,
      Leaf<42>,
      StaticInner<
        Leaf<237>,
        Leaf<47>
      >
    >;

  checkChild<Tree,
    Tree
    >();

  checkChildren<Tree,
      DynamicInner<3,
        Leaf<23>
      >,
      Leaf<42>,
      StaticInner<
        Leaf<237>,
        Leaf<47>
      >
    >();

  checkChild<Tree,
    DynamicInner<3,
      Leaf<23>
    >
    , 0>();

  checkChildren<Dune::TypeTree::Child<Tree, 0>>();

  checkChild<Tree,
    Leaf<23>
    , 0, 0>();

  checkChildren<Dune::TypeTree::Child<Tree, 0, 0>>();

  checkChild<Tree,
    Leaf<42>
    , 1>();

  checkChildren<Dune::TypeTree::Child<Tree, 1>>();

  checkChild<Tree,
    StaticInner<
      Leaf<237>,
      Leaf<47>
    >
    , 2>();

  checkChildren<Dune::TypeTree::Child<Tree, 2>,
      Leaf<237>,
      Leaf<47>
    >();

  checkChild<Tree,
    Leaf<237>
    , 2, 0>();

  checkChildren<Dune::TypeTree::Child<Tree, 2, 0>>();

  checkChild<Tree,
    Leaf<47>
    , 2, 1>();

  checkChildren<Dune::TypeTree::Child<Tree, 2, 1>>();

  return EXIT_SUCCESS;
}
