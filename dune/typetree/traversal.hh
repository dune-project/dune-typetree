// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_TRAVERSAL_HH
#define DUNE_TYPETREE_TRAVERSAL_HH

#include <utility>

#include <dune/common/hybridutilities.hh>
#include <dune/common/std/type_traits.hh>

#include <dune/typetree/childextraction.hh>
#include <dune/typetree/treepath.hh>
#include <dune/typetree/visitor.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */

#ifndef DOXYGEN
    /// A functor with no operation
    struct NoOp
    {
      template<class... T>
      constexpr void operator()(T&&...) const { /* do nothing */ }
    };
#endif

    namespace Detail {

      // SFINAE template check that Tree has a degree() function and a child() function accepting integer indices
      template<class Tree>
      using DynamicTraversalConcept = decltype((
        std::declval<Tree>().degree(),
        std::declval<Tree>().child(0u)
      ));

      // SFINAE template check that Tree has static (constexpr) function Tree::degree()
      template<class Tree>
      using StaticTraversalConcept = decltype((
        std::integral_constant<std::size_t, Tree::degree()>{}
      ));


      template<class Tree, TreePathType::Type pathType, class Prefix,
        std::enable_if_t<Tree::isLeaf, int> = 0>
      constexpr auto leafTreePathTuple(Prefix prefix)
      {
        return std::make_tuple(prefix);
      }

      template<class Tree, TreePathType::Type pathType, class Prefix,
        std::enable_if_t<not Tree::isLeaf, int> = 0>
      constexpr auto leafTreePathTuple(Prefix prefix);

      template<class Tree, TreePathType::Type pathType, class Prefix, std::size_t... indices,
        std::enable_if_t<(Tree::isComposite or (Tree::isPower and (pathType!=TreePathType::dynamic))), int> = 0>
      constexpr auto leafTreePathTuple(Prefix prefix, std::index_sequence<indices...>)
      {
        return std::tuple_cat(Detail::leafTreePathTuple<TypeTree::Child<Tree,indices>, pathType>(Dune::TypeTree::push_back(prefix, Dune::index_constant<indices>{}))...);
      }

      template<class Tree, TreePathType::Type pathType, class Prefix, std::size_t... indices,
        std::enable_if_t<(Tree::isPower and (pathType==TreePathType::dynamic)), int> = 0>
      constexpr auto leafTreePathTuple(Prefix prefix, std::index_sequence<indices...>)
      {
        return std::tuple_cat(Detail::leafTreePathTuple<TypeTree::Child<Tree,indices>, pathType>(Dune::TypeTree::push_back(prefix, indices))...);
      }

      template<class Tree, TreePathType::Type pathType, class Prefix,
        std::enable_if_t<not Tree::isLeaf, int>>
      constexpr auto leafTreePathTuple(Prefix prefix)
      {
        return Detail::leafTreePathTuple<Tree, pathType>(prefix, std::make_index_sequence<Tree::degree()>{});
      }

      /* The signature is the same as for the public applyToTree
       * function in Dune::Typetree, despite the additionally passed
       * treePath argument. The path passed here is associated to
       * the tree and the relative paths of the children (wrt. to tree)
       * are appended to this.  Hence the behavior of the public function
       * is resembled by passing an empty treePath.
       */

      /*
       * This is the overload for leaf traversal
       */
      template<class T, class TreePath, class V,
        std::enable_if_t<std::decay_t<T>::isLeaf, int> = 0>
      void applyToTree(T&& tree, TreePath treePath, V&& visitor)
      {
        visitor.leaf(tree, treePath);
      }

      /*
       * This is the general overload doing child traversal.
       */
      template<class T, class TreePath, class V,
        std::enable_if_t<not std::decay_t<T>::isLeaf, int> = 0>
      void applyToTree(T&& tree, TreePath treePath, V&& visitor)
      {
        using Tree = std::remove_reference_t<T>;
        using Visitor = std::remove_reference_t<V>;
        visitor.pre(tree, treePath);

        // check which type of traversal is supported by the tree
        using allowDynamicTraversal = Dune::Std::is_detected<DynamicTraversalConcept,Tree>;
        using allowStaticTraversal = Dune::Std::is_detected<StaticTraversalConcept,Tree>;

        // the tree must support either dynamic or static traversal
        static_assert(allowDynamicTraversal::value || allowStaticTraversal::value);

        // the visitor may specify preferred dynamic traversal
        using preferDynamicTraversal = std::bool_constant<Visitor::treePathType == TreePathType::dynamic>;

        // create a dynamic or static index range
        auto indices = [&]{
          if constexpr(preferDynamicTraversal::value && allowDynamicTraversal::value)
            return Dune::range(std::size_t(tree.degree()));
          else
            return Dune::range(tree.degree());
        }();

        if constexpr(allowDynamicTraversal::value || allowStaticTraversal::value) {
          Hybrid::forEach(indices, [&](auto i) {
            auto&& child = tree.child(i);
            using Child = std::decay_t<decltype(child)>;

            visitor.beforeChild(tree, child, treePath, i);

            // This requires that visitor.in(...) can always be instantiated,
            // even if there's a single child only.
            if (i>0)
              visitor.in(tree, treePath);

            constexpr bool visitChild = Visitor::template VisitChild<Tree,Child,TreePath>::value;
            if constexpr(visitChild) {
              auto childTreePath = Dune::TypeTree::push_back(treePath, i);
              applyToTree(child, childTreePath, visitor);
            }

            visitor.afterChild(tree, child, treePath, i);
          });
        }
        visitor.post(tree, treePath);
      }

      /* Traverse tree and visit each node. The signature is the same
       * as for the public forEachNode function in Dune::Typtree,
       * despite the additionally passed treePath argument. The path
       * passed here is associated to the tree and the relative
       * paths of the children (wrt. to tree) are appended to this.
       * Hence the behavior of the public function is resembled
       * by passing an empty treePath.
       */
      template<class T, class TreePath, class PreFunc, class LeafFunc, class PostFunc>
      void forEachNode(T&& tree, TreePath treePath, PreFunc&& preFunc, LeafFunc&& leafFunc, PostFunc&& postFunc)
      {
        using Tree = std::decay_t<T>;
        if constexpr(Tree::isLeaf) {
          leafFunc(tree, treePath);
        } else {
          preFunc(tree, treePath);

          // check which type of traversal is supported by the tree, prefer dynamic traversal
          using allowDynamicTraversal = Dune::Std::is_detected<DynamicTraversalConcept,Tree>;
          using allowStaticTraversal = Dune::Std::is_detected<StaticTraversalConcept,Tree>;

          // the tree must support either dynamic or static traversal
          static_assert(allowDynamicTraversal::value || allowStaticTraversal::value);

          if constexpr(allowDynamicTraversal::value) {
            // Specialization for dynamic traversal
            for (std::size_t i = 0; i < tree.degree(); ++i) {
              auto childTreePath = Dune::TypeTree::push_back(treePath, i);
              forEachNode(tree.child(i), childTreePath, preFunc, leafFunc, postFunc);
            }
          } else if constexpr(allowStaticTraversal::value) {
            // Specialization for static traversal
            auto indices = std::make_index_sequence<Tree::degree()>{};
            Hybrid::forEach(indices, [&](auto i) {
              auto childTreePath = Dune::TypeTree::push_back(treePath, i);
              forEachNode(tree.child(i), childTreePath, preFunc, leafFunc, postFunc);
            });
          }
          postFunc(tree, treePath);
        }
      }

    } // namespace Detail


    // ********************************************************************************
    // Public Interface
    // ********************************************************************************

    /**
     * \brief Create tuple of tree paths to leafs
     *
     * This creates a tuple of HybridTreePath objects refering to the
     * leaf nodes in the tree. The generated tree paths are always
     * HybridTreePath instances. If pathType is TreePathType::dynamic
     * then path entries for power nodes are of type std::size_t.
     * For all other nodes and if pathType is not TreePathType::dynamic,
     * the entries are instances of Dune::index_constant.
     *
     * \tparam Tree Type of tree to generate tree paths for
     * \tparam pathType Type of paths to generate
     */
    template<class Tree, TreePathType::Type pathType=TreePathType::dynamic>
    constexpr auto leafTreePathTuple()
    {
      return Detail::leafTreePathTuple<std::decay_t<Tree>, pathType>(hybridTreePath());
    }

    //! Apply visitor to TypeTree.
    /**
     * \code
     #include <dune/typetree/traversal.hh>
     * \endcode
     * This function applies the given visitor to the given tree. Both visitor and tree may be const
     * or non-const (if the compiler supports rvalue references, they may even be a non-const temporary).
     *
     * \note The visitor must implement the interface laid out by DefaultVisitor (most easily achieved by
     *       inheriting from it) and specify the required type of tree traversal (static or dynamic) by
     *       inheriting from either StaticTraversal or DynamicTraversal.
     *
     * \param tree    The tree the visitor will be applied to.
     * \param visitor The visitor to apply to the tree.
     */
    template<typename Tree, typename Visitor>
    void applyToTree(Tree&& tree, Visitor&& visitor)
    {
      Detail::applyToTree(tree, hybridTreePath(), visitor);
    }

    /**
     * \brief Traverse tree and visit each node
     *
     * This does an in-order traversal of the tree.
     * For leaf node the leafNodeFunc callback is called.
     * For each inner node this function first calls the preNodeFunc,
     * then it traverses the children, and then it calls the postNodeFunc.
     * All callback functions are called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param preNodeFunc This function is called for each inner node
     * \param leafNodeFunc This function is called for each leaf node
     * \param postNodeFunc This function is called for each inner node
     */
    template<class Tree, class PreNodeFunc, class LeafNodeFunc, class PostNodeFunc>
    void forEachNode(Tree&& tree, PreNodeFunc&& preNodeFunc, LeafNodeFunc&& leafNodeFunc, PostNodeFunc&& postNodeFunc)
    {
      Detail::forEachNode(tree, hybridTreePath(), preNodeFunc, leafNodeFunc, postNodeFunc);
    }

    /**
     * \brief Traverse tree and visit each node
     *
     * The passed callback function is called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param nodeFunc This function is called for each node
     */
    template<class Tree, class NodeFunc>
    void forEachNode(Tree&& tree, NodeFunc&& nodeFunc)
    {
      Detail::forEachNode(tree, hybridTreePath(), nodeFunc, nodeFunc, NoOp{});
    }

    /**
     * \brief Traverse tree and visit each leaf node
     *
     * The passed callback function is called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param leafFunc This function is called for each leaf node
     */
    template<class Tree, class LeafFunc>
    void forEachLeafNode(Tree&& tree, LeafFunc&& leafFunc)
    {
      Detail::forEachNode(tree, hybridTreePath(), NoOp{}, leafFunc, NoOp{});
    }

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TRAVERSAL_HH
