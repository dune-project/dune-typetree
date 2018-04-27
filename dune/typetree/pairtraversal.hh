// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_PAIRTRAVERSAL_HH
#define DUNE_TYPETREE_PAIRTRAVERSAL_HH

#include <dune/common/std/type_traits.hh>

#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/typetree/treepath.hh>
#include <dune/typetree/visitor.hh>
#include <dune/typetree/traversal.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */

    namespace Detail {

      template<class Node1, class Node2, class Visitor>
      using DynamicChildPairTraversal = typename Dune::Std::conjunction<DynamicChildTraversal<Node1,Visitor>, DynamicChildTraversal<Node2,Visitor>>;

      template<class Node1, class Node2, class Visitor>
      using LeafChildPairTraversal = typename Dune::Std::disjunction<LeafChildTraversal<Node1,Visitor>, LeafChildTraversal<Node2,Visitor>>;

      template<class Node1, class Node2, class Visitor>
      using StaticChildPairTraversal = typename Dune::Std::bool_constant<
        ((not DynamicChildPairTraversal<Node1,Node2,Visitor>::value) and (not LeafChildPairTraversal<Node1,Node2,Visitor>::value))>;

      // Forward declaration for dynamic child traversal overload
      template<class T1, class T2, class TreePath, class V,
        std::enable_if_t<DynamicChildPairTraversal<T1,T2,V>::value, int> = 0>
      void applyToTreePair(T1&& tree1, T2&& tree2, TreePath treePath, V&& visitor);

      /* The signature is the same as for the public applyToTreePair
       * function in Dune::Typtree, despite the additionally passed
       * treePath argument. The path passed here is associated to
       * the tree and the relative paths of the children (wrt. to tree)
       * are appended to this.  Hence the behavior of the public function
       * is resembled by passing an empty treePath.
       */

      /*
       * This is the overload for leaf traversal
       */
      template<class T1, class T2, class TreePath, class V,
        std::enable_if_t<LeafChildPairTraversal<T1,T2,V>::value, int> = 0>
      void applyToTreePair(T1&& tree1, T2&& tree2, TreePath treePath, V&& visitor)
      {
        visitor.leaf(tree1, tree2, treePath);
      }

      /*
       * This is the general overload doing static child traversal.
       */
      template<class T1, class T2, class TreePath, class V,
        std::enable_if_t<StaticChildPairTraversal<T1,T2,V>::value, int> = 0>
      void applyToTreePair(T1&& tree1, T2&& tree2, TreePath treePath, V&& visitor)
      {
        // Do we really want to take care for const-ness of the Tree
        // when instanciating VisitChild below? I'd rather expect this:
        // using Tree1 = std::decay_t<T1>;
        // using Tree2 = std::decay_t<T2>;
        // using Visitor = std::decay_t<V>;
        using Tree1 = std::remove_reference_t<T1>;
        using Tree2 = std::remove_reference_t<T2>;
        using Visitor = std::remove_reference_t<V>;
        visitor.pre(tree1, tree2, treePath);
        auto indices = Dune::Std::make_index_sequence<Tree1::degree()>{};
        Dune::Hybrid::forEach(indices, [&](auto i) {
          auto childTreePath = Dune::TypeTree::push_back(treePath, i);
          auto&& child1 = tree1.child(i);
          auto&& child2 = tree2.child(i);
          using Child1 = std::decay_t<decltype(child1)>;
          using Child2 = std::decay_t<decltype(child2)>;

          visitor.beforeChild(tree1, child1, tree2, child2, treePath, i);

          // This requires that visiotor.in(...) can always be instantiated,
          // even if there's a single child only.
          if (i>0)
            visitor.in(tree1, tree2, treePath);
          static const auto visitChild = Visitor::template VisitChild<Tree1,Child1,Tree2,Child2,TreePath>::value;
          Dune::Hybrid::ifElse(Dune::Std::bool_constant<visitChild>{}, [&] (auto id) {
            applyToTreePair(child1, child2, childTreePath, visitor);
          });

          visitor.afterChild(tree1, child1, tree2, child2, treePath, i);
        });
        visitor.post(tree1, tree2, treePath);
      }

      /*
       * This is the overload doing dynamic child traversal of power nodes.
       */
      template<class T1, class T2, class TreePath, class V,
        std::enable_if_t<DynamicChildPairTraversal<T1,T2,V>::value, int>>
      void applyToTreePair(T1&& tree1, T2&& tree2, TreePath treePath, V&& visitor)
      {
        // Do we really want to take care for const-ness of the Tree
        // when instanciating VisitChild below? I'd rather expect this:
        // using Tree1 = std::decay_t<T1>;
        // using Tree2 = std::decay_t<T2>;
        // using Visitor = std::decay_t<V>;
        using Tree1 = std::remove_reference_t<T1>;
        using Tree2 = std::remove_reference_t<T2>;
        using Visitor = std::remove_reference_t<V>;
        visitor.pre(tree1, tree2, treePath);
        for(std::size_t i : Dune::range(Tree1::degree()))
        {
          auto childTreePath = Dune::TypeTree::push_back(treePath, i);
          auto&& child1 = tree1.child(i);
          auto&& child2 = tree2.child(i);
          using Child1 = std::decay_t<decltype(child1)>;
          using Child2 = std::decay_t<decltype(child2)>;

          visitor.beforeChild(tree1, child1, tree2, child2, treePath, i);

          // This requires that visiotor.in(...) can always be instantiated,
          // even if there's a single child only.
          if (i>0)
            visitor.in(tree1, tree2, treePath);
          static const auto visitChild = Visitor::template VisitChild<Tree1,Child1,Tree2,Child2,TreePath>::value;
          Dune::Hybrid::ifElse(Dune::Std::bool_constant<visitChild>{}, [&] (auto id) {
            applyToTreePair(child1, child2, childTreePath, visitor);
          });

          visitor.afterChild(tree1, child1, tree2, child2, treePath, i);
        }
        visitor.post(tree1, tree2, treePath);
      }


    } // namespace Detail

    //! Apply visitor to a pair of TypeTrees.
    /**
     * This function applies the given visitor to the given tree. Both visitor and tree may be const
     * or non-const. If the compiler supports rvalue references, they may even be a non-const temporary;
     * otherwise both trees must be either const or non-const. If they have different constness, both will
     * be promoted to const.
     *
     * \note The visitor must implement the interface laid out by DefaultPairVisitor (most easily achieved by
     *       inheriting from it) and specify the required type of tree traversal (static or dynamic) by
     *       inheriting from either StaticTraversal or DynamicTraversal.
     *
     * \param tree1   The first tree the visitor will be applied to.
     * \param tree2   The second tree the visitor will be applied to.
     * \param visitor The visitor to apply to the trees.
     */
    template<typename Tree1, typename Tree2, typename Visitor>
    void applyToTreePair(Tree1&& tree1, Tree2&& tree2, Visitor&& visitor)
    {
      Detail::applyToTreePair(tree1, tree2, hybridTreePath(), visitor);
    }

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_PAIRTRAVERSAL_HH
