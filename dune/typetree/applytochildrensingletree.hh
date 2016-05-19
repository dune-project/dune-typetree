// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_APPLYTOCHILDRENSINGLETREE_HH
#define DUNE_TYPETREE_APPLYTOCHILDRENSINGLETREE_HH

#include <dune/common/typetraits.hh>

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

#ifndef DOXYGEN // these are all internals and not public API.

    // forward declaration of main engine struct
    template<TreePathType::Type tpType, typename tag = StartTag, bool doApply = true>
    struct ApplyToTree;

    namespace {

      // For the CompositeNode, we do need a TMP for iterating over the
      // children. Note that we use an index that counts down instead of up.
      // This allows us to specialize the TMP for the last child, where we
      // do not want to invoke the infix visitor on the CompositeNode.

      // There are two versions of this TMP, one for iteration with a static TreePath, and one
      // for iteration with a dynamic TreePath.




      // ********************************************************************************
      // Static Version
      // ********************************************************************************

      template<std::size_t inverse_k, std::size_t count>
      struct apply_to_children_fully_static
      {

        template<typename N, typename V, typename TreePath>
        static void apply(N&& n, V&& v, TreePath tp)
        {
          // make sure we do not try to work with references to the actual types
          typedef typename std::remove_reference<N>::type Node;
          typedef typename std::remove_reference<V>::type Visitor;

          // get child type
          typedef typename Node::template Child<count-inverse_k>::Type C;

          // extend TreePath by child index
          typedef typename TreePathPushBack<TreePath,count-inverse_k>::type ChildTreePath;

          // is the visitor interested in this child?
          const bool visit = Visitor::template VisitChild<Node,C,ChildTreePath>::value;

          // beforeChild() gets called regardless of the value of visit
          v.beforeChild(std::forward<N>(n),n.template child<count-inverse_k>(),tp,std::integral_constant<std::size_t,count-inverse_k>());

          // traverse to child
          ApplyToTree<Visitor::treePathType,NodeTag<C>,visit>::apply(n.template child<count-inverse_k>(),
                                                                     std::forward<V>(v),
                                                                     ChildTreePath());

          // afterChild() gets called regardless of the value of visit
          v.afterChild(std::forward<N>(n),n.template child<count-inverse_k>(),tp,std::integral_constant<std::size_t,count-inverse_k>());

          // we are not at the last child (that is specialized), so call infix visitor callback
          v.in(std::forward<N>(n),tp);

          // continue with next child
          apply_to_children_fully_static<inverse_k-1,count>::apply(std::forward<N>(n),
                                                                   std::forward<V>(v),
                                                                   tp);
        }

      };

      // Specialization for last child. This specialization stops the recursion and
      // does not call the infix visitor on the CompositeNode.
      template<std::size_t count>
      struct apply_to_children_fully_static<1,count>
      {

        template<typename N, typename V, typename TreePath>
        static void apply(N&& n, V&& v, TreePath tp)
        {
          typedef typename std::remove_reference<N>::type Node;
          typedef typename std::remove_reference<V>::type Visitor;
          typedef typename Node::template Child<count-1>::Type C;
          typedef typename TreePathPushBack<TreePath,count-1>::type ChildTreePath;
          const bool visit = Visitor::template VisitChild<Node,C,ChildTreePath>::value;
          v.beforeChild(std::forward<N>(n),n.template child<count-1>(),tp,std::integral_constant<std::size_t,count-1>());
          ApplyToTree<Visitor::treePathType,NodeTag<C>,visit>::apply(n.template child<count-1>(),
                                                                     std::forward<V>(v),
                                                                     ChildTreePath());
          v.afterChild(std::forward<N>(n),n.template child<count-1>(),tp,std::integral_constant<std::size_t,count-1>());
        }

      };

      // Specialization for CompositeNode without any children.
      template<>
      struct apply_to_children_fully_static<0,0>
      {

        template<typename N, typename V, typename TreePath>
        static void apply(N&& n, V&& v, TreePath tp) {}

      };




      // ********************************************************************************
      // Dynamic Version
      // ********************************************************************************

      template<std::size_t inverse_k, std::size_t count>
      struct apply_to_children_dynamic
      {

        template<typename N, typename V, typename TreePath>
        static void apply(N&& n, V&& v, TreePath tp)
        {
          typedef typename std::remove_reference<N>::type Node;
          typedef typename std::remove_reference<V>::type Visitor;
          typedef typename Node::template Child<count-inverse_k>::Type C;
          const bool visit = Visitor::template VisitChild<Node,C,typename TreePath::ViewType>::value;
          v.beforeChild(std::forward<N>(n),n.template child<count-inverse_k>(),tp.view(),count-inverse_k);
          tp.push_back(count-inverse_k);
          ApplyToTree<Visitor::treePathType,NodeTag<C>,visit>::apply(n.template child<count-inverse_k>(),
                                                                     std::forward<V>(v),
                                                                     tp);
          tp.pop_back();
          v.afterChild(std::forward<N>(n),n.template child<count-inverse_k>(),tp.view(),count-inverse_k);
          v.in(std::forward<N>(n),tp.view());
          apply_to_children_dynamic<inverse_k-1,count>::apply(std::forward<N>(n),
                                                              std::forward<V>(v),
                                                              tp);
        }

      };

      // Specialization for last child. This specialization stops the recursion and
      // does not call the infix visitor on the CompositeNode.
      template<std::size_t count>
      struct apply_to_children_dynamic<1,count>
      {

        template<typename N, typename V, typename TreePath>
        static void apply(N&& n, V&& v, TreePath tp)
        {
          typedef typename std::remove_reference<N>::type Node;
          typedef typename std::remove_reference<V>::type Visitor;
          typedef typename Node::template Child<count-1>::Type C;
          const bool visit = Visitor::template VisitChild<Node,C,typename TreePath::ViewType>::value;
          v.beforeChild(std::forward<N>(n),n.template child<count-1>(),tp.view(),count-1);
          tp.push_back(count-1);
          ApplyToTree<Visitor::treePathType,NodeTag<C>,visit>::apply(n.template child<count-1>(),
                                                                     std::forward<V>(v),
                                                                     tp);
          tp.pop_back();
          v.afterChild(std::forward<N>(n),n.template child<count-1>(),tp.view(),count-1);
        }

      };

      // Specialization for CompositeNode without any children.
      template<>
      struct apply_to_children_dynamic<0,0>
      {

        template<typename N, typename V, typename TreePath>
        static void apply(N&& n, V&& v, TreePath tp) {}

      };


      // helper struct for automatically picking the right traversal
      // algorithm variant
      template<TreePathType::Type treePathType, std::size_t CHILDREN>
      struct apply_to_children;

      template<std::size_t CHILDREN>
      struct apply_to_children<TreePathType::fullyStatic,CHILDREN>
        : public apply_to_children_fully_static<CHILDREN,CHILDREN>
      {};

      template<std::size_t CHILDREN>
      struct apply_to_children<TreePathType::dynamic,CHILDREN>
        : public apply_to_children_dynamic<CHILDREN,CHILDREN>
      {};


    } // anonymous namespace



    // Base class for composite node traversal

    // The traversal algorithm is identical for CompositeNode
    // and PowerNode in the case of static traversal,
    // so the implementation can be bundled
    // in a single base class.
    struct ApplyToGenericCompositeNode
    {

      template<typename N, typename V, typename TreePath>
      static void apply(N&& n, V&& v, TreePath tp)
      {
        v.pre(std::forward<N>(n),tp);
        typedef typename std::remove_reference<N>::type Node;
        typedef typename std::remove_reference<V>::type Visitor;
        apply_to_children<Visitor::treePathType,staticDegree<Node>>::apply(std::forward<N>(n),
                                                                           std::forward<V>(v),
                                                                           tp);
        v.post(std::forward<N>(n),tp);
      }

    };

#endif // DOXYGEN

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_APPLYTOCHILDRENSINGLETREE_HH
