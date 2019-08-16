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
        std::enable_if_t<(std::decay_t<T1>::isLeaf or std::decay_t<T2>::isLeaf), int> = 0>
      void applyToTreePair(T1&& tree1, T2&& tree2, TreePath treePath, V&& visitor)
      {
        visitor.leaf(tree1, tree2, treePath);
      }

      /*
       * This is the general overload doing static child traversal.
       */
      template<class T1, class T2, class TreePath, class V,
        std::enable_if_t<not(std::decay_t<T1>::isLeaf or std::decay_t<T2>::isLeaf), int> = 0>
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

        // Use statically encoded degree unless both trees
        // are power nodes and dynamic traversal is requested.
        // Dynamic nodes are always traversed dynamically even
        // when visitors are marked with static traversals
        constexpr auto useDynamicTraversal1 =
                  ((Tree1::isPower and Visitor::treePathType==TreePathType::dynamic)
                  or Tree1::isDynamic);
        constexpr auto useDynamicTraversal2 =
                  ((Tree2::isPower and Visitor::treePathType==TreePathType::dynamic)
                  or Tree2::isDynamic);
        constexpr auto useDynamicTraversal = useDynamicTraversal1 and useDynamicTraversal2;

        static_assert(not ((Visitor::treePathType!=TreePathType::fullyStatic) and (Tree1::isDynamic or Tree2::isDynamic) ),
          "Trees with dynamic nodes cannot be traversed with fullStatic tree path type");

        auto degree = traversalDegree(tree1,std::integral_constant<bool,useDynamicTraversal>{});

        auto indices = Dune::range(degree);
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

          static const auto staticVisitChild = Visitor::template VisitChild<Tree1,Child1,Tree2,Child2,TreePath>::value;
          if constexpr (staticVisitChild)
          {
            const auto dynamicVisitChild = visitor.visitChild(tree1,child1,tree2,child2,treePath);
            if (dynamicVisitChild)
              applyToTreePair(child1, child2, childTreePath, visitor);
          }
          visitor.afterChild(tree1, child1, tree2, child2, treePath, i);
        });
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
