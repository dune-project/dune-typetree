// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_PAIRTRAVERSAL_HH
#define DUNE_TYPETREE_PAIRTRAVERSAL_HH

#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/typetree/treepath.hh>
#include <dune/typetree/visitor.hh>
#include <dune/typetree/applytochildrentreepair.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */

#ifndef DOXYGEN // these are all internals and not public API. Only access is using applyToTree().

    template<TreePathType::Type tpType>
    struct ApplyToTreePair<tpType,StartTag,StartTag,true>
    {

      template<typename Node1, typename Node2, typename Visitor>
      static void apply(Node1&& node1, Node2&& node2, Visitor&& visitor)
      {
        ApplyToTreePair<tpType,
                        NodeTag<Node1>,
                        NodeTag<Node2>
                        >::apply(std::forward<Node1>(node1),
                                 std::forward<Node2>(node2),
                                 std::forward<Visitor>(visitor),
                                 TreePathFactory<tpType>::create(node1).mutablePath());
      }

    };


    // Do not visit nodes the visitor is not interested in
    template<TreePathType::Type tpType, typename Tag1, typename Tag2>
    struct ApplyToTreePair<tpType,Tag1,Tag2,false>
    {
      template<typename Node1, typename Node2, typename Visitor, typename TreePath>
      static void apply(const Node1& node1, const Node2& node2, const Visitor& visitor, TreePath treePath)
      {}
    };


    /*

    // LeafNode - again, this is easy: just do all three visits
    template<TreePathType::Type tpType>
    struct ApplyToTree<tpType,LeafNodeTag,LeafNodeTag,true>
    {

    #if HAVE_RVALUE_REFERENCES

    template<typename N1, typename N2, typename V, typename TreePath>
    static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
    {
    v.leaf(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
    }

    #else

    template<typename N1, typename N2, typename V, typename TreePath>
    static void apply(N1& n1, N2& n2, V& v, TreePath tp)
    {
    v.leaf(n1,n2,tp.view());
    }

    template<typename N1, typename N2, typename V, typename TreePath>
    static void apply(const N1& n1, const N2& n2, V& v, TreePath tp)
    {
    v.leaf(n1,n2,tp.view());
    }

    template<typename N1, typename N2, typename V, typename TreePath>
    static void apply(N1& n1, N2& n2, const V& v, TreePath tp)
    {
    v.leaf(n1,n2,tp.view());
    }

    template<typename N1, typename N2, typename V, typename TreePath>
    static void apply(const N1& n1, const N2& n2, const V& v, TreePath tp)
    {
    v.leaf(n1,n2,tp.view());
    }

    #endif // HAVE_RVALUE_REFERENCES

    };
    */


    // Automatically pick the correct traversal algorithm for the two nodes
    template<TreePathType::Type treePathType,typename FirstTag, typename SecondTag>
    struct ApplyToTreePair<treePathType,FirstTag,SecondTag,true>
      : public ApplyToGenericCompositeNodePair<treePathType>
    {
    };



    // ********************************************************************************
    // Specialization for dynamic traversal and two PowerNodes -> use runtime iteration
    // ********************************************************************************

    template<>
    struct ApplyToTreePair<TreePathType::dynamic,PowerNodeTag,PowerNodeTag,true>
    {

      template<typename N1, typename N2, typename V, typename TreePath>
      static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
      {
        v.pre(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
        typedef typename std::remove_reference<N1>::type Node1;
        typedef typename std::remove_reference<N2>::type Node2;
        typedef typename Node1::template Child<0>::Type C1;
        typedef typename Node2::template Child<0>::Type C2;
        static_assert(staticDegree<Node1> == staticDegree<Node2>,
                      "non-leaf nodes with different numbers of children " \
                      "are not allowed during simultaneous grid traversal");
        const bool visit = std::remove_reference<V>::type
          ::template VisitChild<Node1,C1,Node2,C2,typename TreePath::ViewType>::value;
        for (std::size_t k = 0; k < degree(n1); ++k)
          {
            v.beforeChild(std::forward<N1>(n1),n1.child(k),std::forward<N2>(n2),n2.child(k),tp.view(),k);
            tp.push_back(k);
            ApplyToTreePair<TreePathType::dynamic, // we know that due to the specialization
                            NodeTag<C1>,
                            NodeTag<C2>,
                            visit>::apply(n1.child(k),
                                          n2.child(k),
                                          std::forward<V>(v),
                                          tp);
            tp.pop_back();
            v.afterChild(std::forward<N1>(n1),n1.child(k),std::forward<N2>(n2),n2.child(k),tp.view(),k);
            if (k < degree(n1) - 1)
              v.in(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
          }
        v.post(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
      }

    };

#endif // DOXYGEN

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
      ApplyToTreePair<std::remove_reference<Visitor>::type::treePathType>::apply(std::forward<Tree1>(tree1),
                                                                                 std::forward<Tree2>(tree2),
                                                                                 std::forward<Visitor>(visitor));
    }

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_PAIRTRAVERSAL_HH
