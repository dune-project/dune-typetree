// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TRAVERSAL_HH
#define DUNE_TYPETREE_TRAVERSAL_HH

#if HAVE_RVALUE_REFERENCES
#include <utility>
#endif

#include <dune/common/std/utility.hh>
#include <dune/common/hybridutilities.hh>

#include <dune/typetree/nodetags.hh>
#include <dune/typetree/treepath.hh>
#include <dune/typetree/visitor.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */


    namespace Detail {

      /* The signature is the same as for the public applyToTree
       * function in Dune::Typtree, despite the additionally passed
       * treePath argument. The path passed here is associated to
       * the tree and the relative paths of the children (wrt. to tree)
       * are appended to this.  Hence the behavior of the public function
       * is resembled by passing an empty treePath.
       */
      template<class T, class TreePath, class V>
      void applyToTree(T&& tree, TreePath treePath, V&& visitor)
      {
        // Do we really want to take care for const-ness of the Tree
        // when instanciating VisitChild below? I'd rather expect this:
        // using Tree = std::decay_t<T>;
        // using Visitor = std::decay_t<V>;
        using Tree = std::remove_reference_t<T>;
        using Visitor = std::remove_reference_t<V>;
        Dune::Hybrid::ifElse(Dune::Std::bool_constant<Tree::isLeaf>{}, [&] (auto id) {
          visitor.leaf(id(tree), treePath);
        }, [&] (auto id) {
          visitor.pre(id(tree), treePath);
          auto indices = Dune::Std::make_index_sequence<Tree::degree()>{};
          Dune::Hybrid::forEach(indices, [&](auto i) {
            auto childTreePath = Dune::TypeTree::push_back(treePath, i);
            auto&& child = id(tree).child(i);
            using Child = std::decay_t<decltype(child)>;

            visitor.beforeChild(id(tree), child, treePath, i);

            Dune::Hybrid::ifElse(Dune::Std::bool_constant<(i>0)>{}, [&] (auto id) {
              visitor.in(id(tree), treePath);
            });
            static const auto visitChild = Visitor::template VisitChild<Tree,Child,TreePath>::value;
            Dune::Hybrid::ifElse(Dune::Std::bool_constant<visitChild>{}, [&] (auto id) {
              applyToTree(child, childTreePath, visitor);
            });

            visitor.afterChild(id(tree), child, treePath, i);
          });
          visitor.post(id(tree), treePath);
        });
      }

      /* Traverse tree and visit each node. The signature is the same
       * as for the public forEachNode function in Dune::Typtree,
       * despite the additionally passed treePath argument. The path
       * passed here is associated to the tree and the relative
       * paths of the children (wrt. to tree) are appended to this.
       * Hence the behavior of the public function is resembled
       * by passing an empty treePath.
       */
      template<class Tree, class TreePath, class PreFunc, class LeafFunc, class PostFunc>
      void forEachNode(Tree&& tree, TreePath treePath, PreFunc&& preFunc, LeafFunc&& leafFunc, PostFunc&& postFunc)
      {
        using TreeType = std::decay_t<Tree>;
        Dune::Hybrid::ifElse(Dune::Std::bool_constant<TreeType::isLeaf>{}, [&] (auto id) {
          // If we have a leaf tree just visit it using the leaf function.
          leafFunc(id(tree), treePath);
        }, [&] (auto id) {
          // Otherwise visit the tree with the pre function,
          // visit all children using a static loop, and
          // finally visit the tree with the post function.
          preFunc(id(tree), treePath);
          auto indices = Dune::Std::make_index_sequence<TreeType::degree()>{};
          Dune::Hybrid::forEach(indices, [&](auto i) {
            auto childTreePath = Dune::TypeTree::push_back(treePath, i);
            forEachNode(id(tree).child(i), childTreePath, preFunc, leafFunc, postFunc);
          });
          postFunc(id(tree), treePath);
        });
      }

    } // namespace Detail


    // ********************************************************************************
    // Public Interface
    // ********************************************************************************

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
     * All passed callback functions are called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param preFunc This function is called for each inner node before visiting its children
     * \param leafFunc This function is called for each leaf node
     * \param postFunc This function is called for each inner node after visiting its children
     */
    template<class Tree, class PreFunc, class LeafFunc, class PostFunc>
    void forEachNode(Tree&& tree, PreFunc&& preFunc, LeafFunc&& leafFunc, PostFunc&& postFunc)
    {
      Detail::forEachNode(tree, hybridTreePath(), preFunc, leafFunc, postFunc);
    }

    /**
     * \brief Traverse tree and visit each node
     *
     * All passed callback functions are called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param innerFunc This function is called for each inner node before visiting its children
     * \param leafFunc This function is called for each leaf node
     */
    template<class Tree, class InnerFunc, class LeafFunc>
    void forEachNode(Tree&& tree, InnerFunc&& innerFunc, LeafFunc&& leafFunc)
    {
      auto nop = [](auto&&... args) {};
      forEachNode(tree, innerFunc, leafFunc, nop);
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
      forEachNode(tree, nodeFunc, nodeFunc);
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
      auto nop = [](auto&&... args) {};
      forEachNode(tree, nop, leafFunc, nop);
    }

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TRAVERSAL_HH
