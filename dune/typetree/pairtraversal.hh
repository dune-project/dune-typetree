// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_PAIRTRAVERSAL_HH
#define DUNE_TYPETREE_PAIRTRAVERSAL_HH

#include <dune/common/std/type_traits.hh>

#include <dune/typetree/nodeinterface.hh>
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

      /* The signature is the same as for the public applyToTreePair
       * function in Dune::Typtree, despite the additionally passed
       * treePath argument. The path passed here is associated to
       * the tree and the relative paths of the children (wrt. to tree)
       * are appended to this.  Hence the behavior of the public function
       * is resembled by passing an empty treePath.
       */
      template<class T1, class T2, class TreePath, class V>
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
        Dune::Hybrid::ifElse(Dune::Std::bool_constant<Tree1::isLeaf>{}, [&] (auto id) {
          visitor.leaf(id(tree1), id(tree2), treePath);
        }, [&] (auto id) {
          visitor.pre(id(tree1), id(tree2), treePath);
          auto indices = Dune::Std::make_index_sequence<Tree1::degree()>{};
          Dune::Hybrid::forEach(indices, [&](auto i) {
            auto childTreePath = Dune::TypeTree::push_back(treePath, i);
            auto&& child1 = id(tree1).child(i);
            auto&& child2 = id(tree2).child(i);
            using Child1 = std::decay_t<decltype(child1)>;
            using Child2 = std::decay_t<decltype(child2)>;

            visitor.beforeChild(id(tree1), child1, id(tree2), child2, treePath, i);

            Dune::Hybrid::ifElse(Dune::Std::bool_constant<(i>0)>{}, [&] (auto id) {
              visitor.in(id(tree1), id(tree2), treePath);
            });
            static const auto visitChild = Visitor::template VisitChild<Tree1,Child1,Tree2,Child2,TreePath>::value;
            Dune::Hybrid::ifElse(Dune::Std::bool_constant<visitChild>{}, [&] (auto id) {
              applyToTreePair(child1, child2, childTreePath, visitor);
            });

            visitor.afterChild(id(tree1), child1, id(tree2), child2, treePath, i);
          });
          visitor.post(id(tree1), id(tree2), treePath);
        });
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
