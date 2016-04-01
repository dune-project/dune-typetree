// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:

#ifndef DUNE_TYPETREE_CHILDEXTRACTION_HH
#define DUNE_TYPETREE_CHILDEXTRACTION_HH

#include <utility>

#include <dune/common/concept.hh>
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

#ifndef DOXYGEN

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

      // we need a concept to make sure that the node has a templated child()
      // method
      struct HasTemplateChildMethod {
        template <class Node>
        auto require(const Node& node) -> decltype(node.template child<0>());
      };

      // The actual implementation is rather simple, we just use an overload that requires the first index
      // to be an index_constant, get the child and then recurse.
      // It only gets ugly due to the enable_if, but without that trick, the error messages for the user
      // can get *very* obscure (they are bad enough as it is).
      template<typename Node, std::size_t i, typename... J,
        typename std::enable_if<
          Dune::models<HasTemplateChildMethod, Node>() &&
          (i < std::decay<Node>::type::CHILDREN), int>::type = 0>
      decltype(auto) child(Node&& node, index_constant<i>, J... j)
      {
        return child(std::forward<Node>(node).template child<i>(),j...);
      }

      // This overload is only present to give useful compiler
      // error messages via static_assert in case the other overloads
      // fail.
      template<typename Node, std::size_t i, typename... J,
        typename std::enable_if<
          (!Dune::models<HasTemplateChildMethod, Node>()) ||
          (i >= std::decay<Node>::type::CHILDREN), int>::type = 0>
      void child(Node&& node, index_constant<i>, J... j)
      {
        static_assert(Dune::models<HasTemplateChildMethod, Node>(), "Node does not have a template method child()");
        static_assert(i < std::decay<Node>::type::CHILDREN, "Child index out of range");
      }

      // ********************************************************************************
      // next index is a run-time value
      // ********************************************************************************


      // This struct lazily evaluates the return type by recursively calling child. This has
      // to happen lazily because we only want to do it if the child access at the current
      // level succeeds; otherwise, we would swamp the user with error messages as the algorithm
      // walks down the remaining indices
      //
      // This struct gets created inside an enable_if, but the nested alias template that triggers
      // the recursion is only instantiated if the enable_if was successful.
      template<typename Node>
      struct _lazy_dynamic_decltype
      {
        template<typename... J>
        struct evaluate
        {
          using type = decltype(child(std::declval<Node>().child(0),std::declval<J>()...));
        };
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
          >::type::template evaluate<J...>::type
      {
        return child(std::forward<Node>(node).template child(i),j...);
      }

      template<typename Node, typename... Indices, std::size_t... i>
      auto child(Node&& node, HybridTreePath<Indices...> tp, std::index_sequence<i...>) -> decltype(child(std::forward<Node>(node),treePathEntry<i>(tp)...))
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
      return impl::child(std::forward<Node>(node),tp,std::index_sequence_for<Indices...>{});
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

      // By default, types are flat indices if they are integral
      template<typename T>
      struct _is_flat_index
      {
        using type = std::is_integral<T>;
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
        struct evaluate
        {
          using type = decltype(Dune::TypeTree::child(std::declval<Node>(),std::declval<Indices>()...));
        };
      };

      // helper function for check in member child() functions that tolerates being passed something that
      // isn't a TreePath. It will just return 0 in that case

      template<typename T>
      constexpr typename std::enable_if<
        is_flat_index<T>{},
        bool
        >::type
      _non_empty_tree_path(T)
      {
        return false;
      }

      template<typename T>
      constexpr typename std::enable_if<
        !is_flat_index<T>{},
        bool
        >::type
      _non_empty_tree_path(T t)
      {
        return treePathSize(t) > 0;
      }

    }

#endif // DOXYGEN

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_CHILDEXTRACTION_HH
