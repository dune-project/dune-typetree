// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_CHILDEXTRACTION_HH
#define DUNE_TYPETREE_CHILDEXTRACTION_HH

#include <type_traits>
#include <utility>
#include <tuple>

#include <dune/common/documentation.hh>
#include <dune/common/indices.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/common/typetree/childaccess.hh>

#include <dune/typetree/treepath.hh>


namespace Dune {
  namespace TypeTree {

#ifndef DOXYGEN

    namespace Impl {

      // check at run time whether index is a valid child index
      template <class Node, class Index>
      std::true_type checkChildIndex (Node const& node, Index i)
      {
        assert(std::size_t(i) < node.degree() && "Child index out of range");
        return {};
      }

      // check at compile time whether index is a valid index
      template <class Node, std::size_t i>
      std::bool_constant<(i < Node::degree())> checkChildIndex (Node const& node, index_constant<i>)
      {
        static_assert(i < Node::degree(), "Child index out of range");
        return {};
      }

      template<class NodePtr>
      auto childStorageImpl (NodePtr&& nodePtr)
      {
        return std::forward<NodePtr>(nodePtr);
      }

      // recursively call `node.childStorage(...)` with the given indices
      template<class NodePtr, class I0, class... I>
      decltype(auto) childStorageImpl (NodePtr&& nodePtr, I0 i0, [[maybe_unused]] I... i)
      {
        auto valid = checkChildIndex(*nodePtr,i0);
        if constexpr (valid)
          return childStorageImpl(nodePtr->childStorage(i0),i...);
        else
          return;
      }

      // forward to the impl methods by extracting the indices from the treepath
      template<class NodePtr, class... Indices, std::size_t... i>
      decltype(auto) childStorage (NodePtr&& nodePtr, [[maybe_unused]] HybridTreePath<Indices...> tp, std::index_sequence<i...>)
      {
        return childStorageImpl(std::forward<NodePtr>(nodePtr),treePathEntry<i>(tp)...);
      }

    } // end namespace Impl

#endif // DOXYGEN

    template<typename Node, typename... Indices>
    auto childStorage (Node&& node, Indices... indices)
    {
      static_assert(sizeof...(Indices) > 0, "childStorage() cannot be called with an empty list of child indices");
      return Impl::childStorageImpl(&node,indices...);
    }

    template<typename Node, typename... Indices>
    auto childStorage (Node&& node, HybridTreePath<Indices...> treePath)
    {
      static_assert(sizeof...(Indices) > 0, "childStorage() cannot be called with an empty TreePath");
      return Impl::childStorage(&node, treePath, std::index_sequence_for<Indices...>{});
    }

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
    using is_flat_index = typename impl::_is_flat_index<std::decay_t<T>>::type;

#ifndef DOXYGEN

    namespace impl {

      // helper function for check in member child() functions that tolerates being passed something that
      // isn't a TreePath. It will just return 0 in that case

      template<typename T>
      constexpr typename std::enable_if<
        Dune::TypeTree::is_flat_index<T>::value,
        bool
        >::type
      _non_empty_tree_path (T)
      {
        return false;
      }

      template<typename T>
      constexpr typename std::enable_if<
        !Dune::TypeTree::is_flat_index<T>::value,
        bool
        >::type
      _non_empty_tree_path (T t)
      {
        return treePathSize(t) > 0;
      }

    }

#endif // DOXYGEN

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_CHILDEXTRACTION_HH
