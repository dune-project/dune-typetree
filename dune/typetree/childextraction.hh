// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:

#ifndef DUNE_TYPETREE_CHILDEXTRACTION_HH
#define DUNE_TYPETREE_CHILDEXTRACTION_HH

#include <dune/common/documentation.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/typetree/treepath.hh>


namespace Dune {
  namespace TypeTree {


    //! \addtogroup TypeTreeChildExtraction Child Extraction
    //! Utility functions and metafunctions for extracting children from a TypeTree.
    //! \ingroup TypeTree
    //! \{

    //! Extract the type of the child of Node at position TreePath.
    /**
     * \deprecated extract_child_type is deprecated and will be removed after TypeTree 2.4,
     *             use the template alias Child instead.
     */
    template<typename Node, typename TreePath>
    struct extract_child_type
    {

      //! The type of the child.
      typedef typename extract_child_type<
        typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::Type,
        typename TypeTree::TreePathPopFront<TreePath>::type
        >::type type;

      //! The storage type of the child.
      typedef typename extract_child_type<
        typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::Type,
        typename TypeTree::TreePathPopFront<TreePath>::type
        >::storage_type storage_type;

      //! The const storage type of the child.
      typedef typename extract_child_type<
        typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::Type,
        typename TypeTree::TreePathPopFront<TreePath>::type
        >::const_storage_type const_storage_type;


    };

#ifndef DOXYGEN

    // end of recursion
    template<typename Node>
    struct extract_child_type<Node,TypeTree::TreePath<> >
    {
      typedef Node type;
      typedef shared_ptr<Node> storage_type;
      typedef shared_ptr<const Node> const_storage_type;
    };

#endif // DOXYGEN



#ifdef DOXYGEN

    //! Extract the child of a node located at tp (non-const version).
    /**
     * Use this function to extract a (possibly indirect) child of
     * a TypeTree node.
     *
     * Example:
     *
     * extract_child(node,Dune::TypeTree::TreePath<2,3,0>())
     *
     * returns the first child of the fourth child of the third child
     * of node.
     *
     * \sa Use extract_child_type to determine the type of the return
     *     value.
     *
     * \param node      The node from which to extract the child.
     * \param tp        The path into the tree leading to the child.
     *                  Note that the actual instance is not used
     *                  at all by this function, only the type of
     *                  the parameter.
     * \tparam TreePath A TreePath instantiation which statically
     *                  encodes the path to the child.
     * \return          A reference to the child.
     *
     * \deprecated extract_child is deprecated and will be removed after TypeTree 2.4,
     *             use the freestanding function child() or the enhanced child() methods
     *             on the tree nodes instead.
     */
    template<typename Node, typename TreePath>
    ImplementationDefined& extract_child(Node& node, Treepath tp)
    {}

    //! Extract the child of a node located at tp (const version).
    /**
     * Use this function to extract a (possibly indirect) child of
     * a TypeTree node.
     *
     * Example:
     *
     * extract_child(node,Dune::TypeTree::TreePath<2,3,0>())
     *
     * returns the first child of the fourth child of the third child
     * of node.
     *
     * \sa Use extract_child_type to determine the type of the return
     *     value.
     *
     * \param node      The node from which to extract the child.
     * \param tp        The path into the tree leading to the child.
     *                  Note that the actual instance is not used
     *                  at all by this function, only the type of
     *                  the parameter.
     * \tparam TreePath A TreePath instantiation which statically
     *                  encodes the path to the child.
     * \return          A reference to the child.
     *
     * \deprecated extract_child is deprecated and will be removed after TypeTree 2.4,
     *             use the freestanding function child() or the enhanced child() methods
     *             on the tree nodes instead.
     */
    template<typename Node, typename TreePath>
    const ImplementationDefined& extract_child(const Node& node, Treepath tp)
    {}

#else // DOXYGEN

    // ********************************************************************************
    // non-const implementation
    // ********************************************************************************

    template<typename Node, typename TreePath>
    typename enable_if<
      (TypeTree::TreePathSize<TreePath>::value > 1),
      typename extract_child_type<Node,TreePath>::type&
      >::type
    extract_child(Node& node, TreePath tp)
    {
      return extract_child(node.template child<TypeTree::TreePathFront<TreePath>::value>(),
                           typename TypeTree::TreePathPopFront<TreePath>::type());
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 1,
      typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::Type&
      >::type
    extract_child(Node& node, TreePath tp)
    {
      return node.template child<TypeTree::TreePathFront<TreePath>::value>();
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 0,
      Node&
      >::type
    extract_child(Node& node, TreePath tp)
    {
      return node;
    }

    // ********************************************************************************
    // const implementation
    // ********************************************************************************

    template<typename Node, typename TreePath>
    typename enable_if<
      (TypeTree::TreePathSize<TreePath>::value > 1),
      const typename extract_child_type<Node,TreePath>::type&
      >::type
    extract_child(const Node& node, TreePath tp)
    {
      return extract_child(node.template child<TypeTree::TreePathFront<TreePath>::value>(),
                           typename TypeTree::TreePathPopFront<TreePath>::type());
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 1,
      const typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::Type&
      >::type
    extract_child(const Node& node, TreePath tp)
    {
      return node.template child<TypeTree::TreePathFront<TreePath>::value>();
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 0,
      const Node&
      >::type
    extract_child(const Node& node, TreePath tp)
    {
      return node;
    }


#endif // DOXYGEN



#ifdef DOXYGEN

    //! Extract the storage for the child of a node located at tp
    //! (non-const version).
    /**
     * Use this function to extract the storage (usually a shared_ptr)
     * of a (possibly indirect) child of a TypeTree node.
     *
     * Example:
     *
     * extract_child_storage(node,Dune::TypeTree::TreePath<2,3,0>())
     *
     * returns the first child of the fourth child of the third child
     * of node.
     *
     * \sa Use extract_child_type to determine the type of the return
     *     value.
     *
     * \param node      The node from which to extract the child.
     * \param tp        The path into the tree leading to the child.
     *                  Note that the actual instance is not used
     *                  at all by this function, only the type of
     *                  the parameter.
     * \tparam TreePath A TreePath instantiation which statically
     *                  encodes the path to the child.
     * \return          A reference to the child.
     */
    template<typename Node, typename TreePath>
    ImplementationDefined extract_child_storage(Node& node, Treepath tp)
    {}

    //! Extract the storage for the child of a node located at tp
    //! (const version).
    /**
     * Use this function to extract the const storage (usually a shared_ptr)
     * of a (possibly indirect) child of a TypeTree node.
     *
     * Example:
     *
     * extract_child_storage(node,Dune::TypeTree::TreePath<2,3,0>())
     *
     * returns the first child of the foruth child of the third child
     * of node.
     *
     * \sa Use extract_child_type to determine the type of the return
     *     value.
     *
     * \param node      The node from which to extract the child.
     * \param tp        The path into the tree leading to the child.
     *                  Note that the actual instance is not used
     *                  at all by this function, only the type of
     *                  the parameter.
     * \tparam TreePath A TreePath instantiation which statically
     *                  encodes the path to the child.
     * \return          A reference to the child.
     */
    template<typename Node, typename TreePath>
    ImplementationDefined extract_child_storage(const Node& node, Treepath tp)
    {}

#else // DOXYGEN

    // ********************************************************************************
    // non-const implementation
    // ********************************************************************************

    template<typename Node, typename TreePath>
    typename enable_if<
      (TypeTree::TreePathSize<TreePath>::value > 1),
      typename extract_child_type<Node,TreePath>::storage_type
      >::type
    extract_child_storage(Node& node, TreePath tp)
    {
      return extract_child_storage(node.template child<TypeTree::TreePathFront<TreePath>::value>(),
                                   typename TypeTree::TreePathPopFront<TreePath>::type());
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 1,
      typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::Storage&
      >::type
    extract_child_storage(Node& node, TreePath tp)
    {
      return node.template childStorage<TypeTree::TreePathFront<TreePath>::value>();
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 0
      >::type
    extract_child_storage(Node& node, TreePath tp)
    {
      static_assert((Dune::AlwaysFalse<Node>::value),
                    "extract_child_storage only works for real children, not the node itself.");
    }

    // ********************************************************************************
    // const implementation
    // ********************************************************************************

    template<typename Node, typename TreePath>
    typename enable_if<
      (TypeTree::TreePathSize<TreePath>::value > 1),
      typename extract_child_type<Node,TreePath>::const_storage_type
      >::type
    extract_child_storage(const Node& node, TreePath tp)
    {
      return extract_child_storage(node.template child<TypeTree::TreePathFront<TreePath>::value>(),
                                   typename TypeTree::TreePathPopFront<TreePath>::type());
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 1,
      typename Node::template Child<TypeTree::TreePathFront<TreePath>::value>::ConstStorage
      >::type
    extract_child_storage(const Node& node, TreePath tp)
    {
      return node.template childStorage<TypeTree::TreePathFront<TreePath>::value>();
    }

    template<typename Node, typename TreePath>
    typename enable_if<
      TypeTree::TreePathSize<TreePath>::value == 0
      >::type
    extract_child_storage(const Node& node, TreePath tp)
    {
      static_assert((Dune::AlwaysFalse<Node>::value),
                    "extract_child_storage only works for real children, not the node itself.");
    }


    namespace impl {

      // ********************************************************************************
      // end of the recursion, there are no child indices, so just return the node itself
      // ********************************************************************************

      template<typename Node>
      auto child(Node&& node) -> decltype(std::forward<Node>(node))
      {
        return std::forward<Node>(node);
      }

      // ********************************************************************************
      // next index is a compile-time constant
      // ********************************************************************************

      // we need a little helper trait to make sure that the node has a templated child()
      // method
      template<typename Node, typename _ = decltype(std::declval<Node>().template child<0>())>
      static constexpr auto _has_template_child_method(Node*) -> std::true_type;

      template<typename Node>
      static constexpr auto _has_template_child_method(void*) -> std::false_type;

      // This struct lazily evaluates the return type by recursively calling child. This has
      // to happen lazily because we only want to do it if the child access at the current
      // level succeeds; otherwise, we would swamp the user with error messages as the algorithm
      // walks down the remaining indices
      //
      // This struct gets created inside an enable_if, but the nested alias template that triggers
      // the recursion is only instantiated if the enable_if was successful.
      template<typename Node>
      struct _lazy_static_decltype
      {
        template<typename I, typename... J>
        using evaluate = decltype(child(std::declval<Node>().template child<I::value>(),std::declval<J>()...));
      };

      // The actual implementation is rather simple, we just use an overload that requires the first index
      // to be an index_constant, get the child and then recurse.
      // It only gets ugly due to the enable_if, but without that trick, the error messages for the user
      // can get *very* obscure (they are bad enough as it is, concepts where are you?).
      template<typename Node, std::size_t i, typename... J>
      auto child(Node&& node, index_constant<i>, J... j) ->
        typename std::enable_if<
          decltype(_has_template_child_method(std::declval<typename std::remove_reference<Node>::type*>()))::value &&
          (i < std::decay<Node>::type::CHILDREN),
          _lazy_static_decltype<
            typename std::remove_reference<Node>::type
            >
          >::type::template evaluate<index_constant<i>,J...>
      {
        return child(std::forward<Node>(node).template child<i>(),j...);
      }

      // ********************************************************************************
      // next index is a run-time value
      // ********************************************************************************


      // again, a lazy struct for the recursion to further child nodes (see above for further explanation)
      template<typename Node>
      struct _lazy_dynamic_decltype
      {
        template<typename... J>
        using evaluate = decltype(child(std::declval<Node>().child(0),std::declval<J>()...));
      };

      // The actual implemention here overloads on std::size_t. It is a little less ugly because it currently
      // has a hard requirement on the PowerNode Tag (although only using is_convertible, as tags can be
      // inherited (important!).
      template<typename Node, typename... J>
      auto child(Node&& node, std::size_t i, J... j) ->
        typename std::enable_if<
          std::is_convertible<
            typename std::remove_reference<Node>::type::NodeTag,
            PowerNodeTag
            >::value,
          _lazy_dynamic_decltype<
            typename std::remove_reference<Node>::type
            >
          >::type::template evaluate<J...>
      {
        return child(std::forward<Node>(node).template child(i),j...);
      }

      template<typename Node, typename... Indices, std::size_t... i>
      auto child(Node&& node, HybridTreePath<Indices...> tp, Std::index_sequence<i...>) -> decltype(child(std::forward<Node>(node),treePathEntry<i>(tp)...))
      {
        return child(std::forward<Node>(node),treePathEntry<i>(tp)...);
      }


    } // namespace imp

#endif // DOXYGEN

    //! Extracts the child of a node given by a sequence of compile-time and run-time indices.
    /**
     * Use this function to extract a (possibly indirect) child of a TypeTree node.
     *
     * Example:
     *
     * \code{.cc}
     * using namespace Dune::TypeTree::Indices; // for compile-time indices
     * auto&& c = child(node,_4,2,_0,1);
     * \endcode
     *
     * returns the second child of the first child of the third child
     * of the fifth child of node, where some child lookups were done using
     * a compile-time index and some using a run-time index.
     *
     * \param node        The node from which to extract the child.
     * \param indices...  A list of indices that describes the path into the tree to the
     *                    wanted child. These parameters can be a combination of run time indices
     *                    (for tree nodes that allow accessing their children using run time information,
     *                    like PowerNode) and instances of index_constant, which work for all types of inner
     *                    nodes.
     * \return            A reference to the child, its cv-qualification depends on the passed-in node.
     */
    template<typename Node, typename... Indices>
#ifdef DOXYGEN
    ImplementationDefined child(Node&& node, Indices... indices)
#else
    auto child(Node&& node, Indices... indices) -> decltype(impl::child(std::forward<Node>(node),indices...))
#endif
    {
      return impl::child(std::forward<Node>(node),indices...);
    }


    //! Extracts the child of a node given by a static TreePath object.
    /**
     * Use this function to extract a (possibly indirect) child of a TypeTree node.
     *
     * Example:
     *
     * \code{.cc}
     * Dune::TypeTree::TreePath<4,2,0,1> tp;
     * auto&& c = child(node,tp);
     * \endcode
     *
     * returns the second child of the first child of the third child
     * of the fifth child of node.
     *
     * \param node        The node from which to extract the child.
     * \param treePath    A TreePath object that describes the path into the tree to the
     *                    wanted child.
     * \return            A reference to the child, its cv-qualification depends on the passed-in node.
     */
    template<typename Node, std::size_t... Indices>
#ifdef DOXYGEN
    ImplementationDefined child(Node&& node, TreePath<Indices...> treePath)
#else
    auto child(Node&& node, TreePath<Indices...>) -> decltype(child(std::forward<Node>(node),index_constant<Indices>()...))
#endif
    {
      return child(std::forward<Node>(node),index_constant<Indices>{}...);
    }


    //! Extracts the child of a node given by a HybridTreePath object.
    /**
     * Use this function to extract a (possibly indirect) child of a TypeTree node.
     *
     * Example:
     *
     * \code{.cc}
     * using namespace Dune::TypeTree::Indices; // for compile-time indices
     * auto tp = Dune::TypeTree::hybridTreePath(_4,2,_0,1);
     * auto&& c = child(node,tp);
     * \endcode
     *
     * returns the second child of the first child of the third child
     * of the fifth child of node, where some child lookups were done using
     * a compile-time index and some using a run-time index.
     *
     * \param node        The node from which to extract the child.
     * \param tree{ath    A HybridTreePath that describes the path into the tree to the
     *                    wanted child. This tree path object  can be a combination of run time indices
     *                    (for tree nodes that allow accessing their children using run time information,
     *                    like PowerNode) and instances of index_constant, which work for all types of inner
     *                    nodes.
     * \return            A reference to the child, its cv-qualification depends on the passed-in node.
     */
    template<typename Node, typename... Indices>
#ifdef DOXYGEN
    ImplementationDefined child(Node&& node, HybridTreePath<Indices...> treePath)
#else
    auto child(Node&& node, HybridTreePath<Indices...> tp) -> decltype(impl::child(std::forward<Node>(node),tp,tp.enumerate()))
#endif
    {
      return impl::child(std::forward<Node>(node),tp,Std::index_sequence_for<Indices...>{});
    }

#ifndef DOXYGEN

    namespace impl {

      template<typename Node, std::size_t... indices>
      struct _Child
      {
        using type = typename std::decay<decltype(child(std::declval<Node>(),index_constant<indices>{}...))>::type;
      };

    }

#endif // DOXYGEN

    //! Template alias for the type of a child node given by a list of child indices.
    /**
     * This template alias is implemented in terms of the free-standing child() functions and uses those
     * in combination with decltype() to extract the child type.

     * \tparam Node     The type of the parent node.
     * \tparam indices  A list of index values the describes the path to the wanted child.
     */
    template<typename Node, std::size_t... indices>
    using Child = typename impl::_Child<Node,indices...>::type;


#ifndef DOXYGEN

    namespace impl {

      template<typename Node, typename TreePath>
      struct _ChildForTreePath
      {
        using type = typename std::decay<decltype(child(std::declval<Node>(),std::declval<TreePath>()))>::type;
      };

    }

#endif // DOXYGEN

    //! Template alias for the type of a child node given by a TreePath or a HybridTreePath type.
    /**
     * This template alias is implemented in terms of the free-standing child() functions and uses those
     * in combination with decltype() to extract the child type. It supports both TreePath and
     * HybridTreePath.
     *
     * \tparam Node      The type of the parent node.
     * \tparam TreePath  The type of a TreePath or a HybridTreePath that describes the path to the wanted child.
     */
    template<typename Node, typename TreePath>
    using ChildForTreePath = typename impl::_ChildForTreePath<Node,TreePath>::type;


#ifndef DOXYGEN

    namespace impl {

      // By default, types are not flat indices
      template<typename T>
      struct _is_flat_index
      {
        using type = std::false_type;
      };

      // But std::size_t is
      template<>
      struct _is_flat_index<std::size_t>
      {
        using type = std::true_type;
      };

      // And so is any index_constant
      template<std::size_t i>
      struct _is_flat_index<index_constant<i>>
      {
        using type = std::true_type;
      };

    }

#endif // DOXYGEN

    //! Type trait that determines whether T is a flat index in the context of child extraction.
    /*
     * This type trait can be used to check whether T is a flat index (i.e. either `std::size_t`
     * or `index_constant`). The type trait normalizes T before doing the check, so it will also
     * work correctly for references and cv-qualified types.
     */
    template<typename T>
    using is_flat_index = typename impl::_is_flat_index<typename std::decay<T>::type>::type;

#ifndef DOXYGEN

    namespace impl {

      // helper struct to perform lazy return type evaluation in the forwarding member child() methods
      // of nodes
      template<typename Node>
      struct _lazy_member_child_decltype
      {
        template<typename... Indices>
        using evaluate = decltype(Dune::TypeTree::child(std::declval<Node>(),std::declval<Indices>()...));
      };

    }

#endif // DOXYGEN

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_CHILDEXTRACTION_HH
