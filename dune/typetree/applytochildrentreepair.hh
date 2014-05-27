// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_APPLYTOCHILDRENTREEPAIR_HH
#define DUNE_TYPETREE_APPLYTOCHILDRENTREEPAIR_HH

#include <dune/common/typetraits.hh>

#include <dune/typetree/nodetags.hh>
#include <dune/typetree/treepath.hh>
#include <dune/typetree/visitor.hh>

#include <utility>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */

#ifndef DOXYGEN // these are all internals and not public API. Only access is using applyToTree().

    // forward declaration of main engine struct
    template<TreePathType::Type tpType, typename tag1 = StartTag, typename tag2 = StartTag, bool doApply = true>
    struct ApplyToTreePair;

    namespace {

      // TMP for traversing the children of non-leaf nodes with a static TreePath.
      // Due to the static TreePath, we have to use this TMP for both CompositeNode
      // and PowerNode.
      template<std::size_t inverse_k, std::size_t count>
      struct apply_to_children_pair_fully_static
      {

        template<typename N1, typename N2, typename V, typename TreePath>
        static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
        {
          typedef typename remove_reference<N1>::type::template Child<count-inverse_k>::Type C1;
          typedef typename remove_reference<N2>::type::template Child<count-inverse_k>::Type C2;
          typedef typename TreePathPushBack<TreePath,count-inverse_k>::type ChildTreePath;
          const bool visit = std::remove_reference<V>::type
            ::template VisitChild<typename remove_reference<N1>::type,
                                  C1,
                                  typename remove_reference<N2>::type,
                                  C2,
                                  ChildTreePath>::value;
          v.beforeChild(std::forward<N1>(n1),n1.template child<count-inverse_k>(),
                        std::forward<N2>(n2),n2.template child<count-inverse_k>(),
                        tp,integral_constant<std::size_t,count-inverse_k>());
          ApplyToTreePair<std::remove_reference<V>::type::treePathType,
                          typename C1::NodeTag,
                          typename C2::NodeTag,
                          visit>::apply(n1.template child<count-inverse_k>(),
                                        n2.template child<count-inverse_k>(),
                                        std::forward<V>(v),
                                        ChildTreePath());
          v.afterChild(std::forward<N1>(n1),n1.template child<count-inverse_k>(),
                       std::forward<N2>(n2),n2.template child<count-inverse_k>(),
                       tp,integral_constant<std::size_t,count-inverse_k>());
          v.in(std::forward<N1>(n1),std::forward<N2>(n2),tp);
          apply_to_children_pair_fully_static<inverse_k-1,count>::apply(std::forward<N1>(n1),
                                                                        std::forward<N2>(n2),
                                                                        std::forward<V>(v),
                                                                        tp);
        }

      };

      // Specialization for last child. This specialization stops the recursion and
      // does not call the infix visitor.
      template<std::size_t count>
      struct apply_to_children_pair_fully_static<1,count>
      {

        template<typename N1, typename N2, typename V, typename TreePath>
        static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
        {
          typedef typename remove_reference<N1>::type::template Child<count-1>::Type C1;
          typedef typename remove_reference<N2>::type::template Child<count-1>::Type C2;
          typedef typename TreePathPushBack<TreePath,count-1>::type ChildTreePath;
          const bool visit = std::remove_reference<V>::type
            ::template VisitChild<typename remove_reference<N1>::type,
                                  C1,
                                  typename remove_reference<N2>::type,
                                  C2,
                                  ChildTreePath>::value;
          v.beforeChild(std::forward<N1>(n1),n1.template child<count-1>(),
                        std::forward<N2>(n2),n2.template child<count-1>(),
                        tp,integral_constant<std::size_t,count-1>());
          ApplyToTreePair<std::remove_reference<V>::type::treePathType,
                          typename C1::NodeTag,
                          typename C2::NodeTag,
                          visit>::apply(n1.template child<count-1>(),
                                        n2.template child<count-1>(),
                                        std::forward<V>(v),
                                        ChildTreePath());
          v.afterChild(std::forward<N1>(n1),n1.template child<count-1>(),
                       std::forward<N2>(n2),n2.template child<count-1>(),
                       tp,integral_constant<std::size_t,count-1>());
        }

      };

      // Specialization for CompositeNode without any children.
      template<>
      struct apply_to_children_pair_fully_static<0,0>
      {

        template<typename N1, typename N2, typename V, typename TreePath>
        static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp) {}

      };


      // TMP for traversing the children of non-leaf nodes with a dynamic TreePath.
      // This is used if at least one of the nodes is a CompositeNode.
      // If both nodes are PowerNodes, this will not be instantiated and we simply
      // use a runtime loop to iterate over the children.
      template<std::size_t inverse_k, std::size_t count>
      struct apply_to_children_pair_dynamic
      {

        template<typename N1, typename N2, typename V, typename TreePath>
        static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
        {
          typedef typename remove_reference<N1>::type::template Child<count-inverse_k>::Type C1;
          typedef typename remove_reference<N2>::type::template Child<count-inverse_k>::Type C2;
          const bool visit = std::remove_reference<V>::type
            ::template VisitChild<typename remove_reference<N1>::type,
                                  C1,
                                  typename remove_reference<N2>::type,
                                  C2,
                                  typename TreePath::ViewType>::value;
          v.beforeChild(std::forward<N1>(n1),n1.template child<count-inverse_k>(),
                        std::forward<N2>(n2),n2.template child<count-inverse_k>(),
                        tp.view(),count-inverse_k);
          tp.push_back(count-inverse_k);
          ApplyToTreePair<std::remove_reference<V>::type::treePathType,
                          typename C1::NodeTag,
                          typename C2::NodeTag,
                          visit>::apply(n1.template child<count-inverse_k>(),
                                        n2.template child<count-inverse_k>(),
                                        std::forward<V>(v),
                                        tp);
          tp.pop_back();
          v.afterChild(std::forward<N1>(n1),n1.template child<count-inverse_k>(),
                       std::forward<N2>(n2),n2.template child<count-inverse_k>(),
                       tp.view(),count-inverse_k);
          v.in(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
          apply_to_children_pair_dynamic<inverse_k-1,count>::apply(std::forward<N1>(n1),
                                                                   std::forward<N2>(n2),
                                                                   std::forward<V>(v),
                                                                   tp);
        }

      };

      // Specialization for last child. This specialization stops the recursion and
      // does not call the infix visitor on the CompositeNode.
      template<std::size_t count>
      struct apply_to_children_pair_dynamic<1,count>
      {

        template<typename N1, typename N2, typename V, typename TreePath>
        static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
        {
          typedef typename remove_reference<N1>::type::template Child<count-1>::Type C1;
          typedef typename remove_reference<N2>::type::template Child<count-1>::Type C2;
          const bool visit = std::remove_reference<V>::type
            ::template VisitChild<typename remove_reference<N1>::type,
                                  C1,
                                  typename remove_reference<N2>::type,
                                  C2,
                                  typename TreePath::ViewType>::value;
          v.beforeChild(std::forward<N1>(n1),n1.template child<count-1>(),
                        std::forward<N2>(n2),n2.template child<count-1>(),
                        tp.view(),count-1);
          tp.push_back(count-1);
          ApplyToTreePair<std::remove_reference<V>::type::treePathType,
                          typename C1::NodeTag,
                          typename C2::NodeTag,
                          visit>::apply(n1.template child<count-1>(),
                                        n2.template child<count-1>(),
                                        std::forward<V>(v),
                                        tp);
          tp.pop_back();
          v.afterChild(std::forward<N1>(n1),n1.template child<count-1>(),
                       std::forward<N2>(n2),n2.template child<count-1>(),
                       tp.view(),count-1);
        }

      };

      // Specialization for CompositeNode without any children.
      template<>
      struct apply_to_children_pair_dynamic<0,0>
      {

        template<typename N1, typename N2, typename V, typename TreePath>
        static void apply(N1&& n1, N2&& n2, V&& v, TreePath tp) {}

      };

      // helper struct for automatically picking the correct child
      // traversal algorithm variant
      template<TreePathType::Type treePathType, std::size_t CHILDREN>
      struct apply_to_children_pair;

      template<std::size_t CHILDREN>
      struct apply_to_children_pair<TreePathType::fullyStatic,CHILDREN>
        : public apply_to_children_pair_fully_static<CHILDREN,CHILDREN>
      {};

      template<std::size_t CHILDREN>
      struct apply_to_children_pair<TreePathType::dynamic,CHILDREN>
        : public apply_to_children_pair_dynamic<CHILDREN,CHILDREN>
      {};


    } // anonymous namespace


      // Base class for composite node traversal
    template<TreePathType::Type treePathType>
    struct ApplyToGenericCompositeNodePair
    {

      // one node is a leaf -> treat node pair as a leaf
      template<typename N1, typename N2, typename V, typename TreePath>
      static typename enable_if<(remove_reference<N1>::type::isLeaf || remove_reference<N2>::type::isLeaf)>::type
      apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
      {
        v.leaf(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
      }

      // both nodes contain children -> iterate over them
      template<typename N1, typename N2, typename V, typename TreePath>
      static typename enable_if<!(remove_reference<N1>::type::isLeaf || remove_reference<N2>::type::isLeaf)>::type
      apply(N1&& n1, N2&& n2, V&& v, TreePath tp)
      {
        v.pre(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
        typedef typename remove_reference<N1>::type Node1;
        typedef typename remove_reference<N2>::type Node2;
        // make sure both nodes have the same number of children - otherwise, it
        // would be difficult to match the children to each other.
        static_assert(Node1::CHILDREN == Node2::CHILDREN,
                      "non-leaf nodes with different numbers of children " \
                      "are not allowed during simultaneous grid traversal");
        apply_to_children_pair<treePathType,Node1::CHILDREN>::apply(std::forward<N1>(n1),
                                                                    std::forward<N2>(n2),
                                                                    std::forward<V>(v),
                                                                    tp);
        v.post(std::forward<N1>(n1),std::forward<N2>(n2),tp.view());
      }

    };


#endif // DOXYGEN

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_APPLYTOCHILDRENTREEPAIR_HH
