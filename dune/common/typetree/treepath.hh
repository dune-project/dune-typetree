// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_COMMON_TYPETREE_TREEPATH_HH
#define DUNE_COMMON_TYPETREE_TREEPATH_HH

#include <cstddef>
#include <cassert>
#include <type_traits>

#include <dune/common/documentation.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/indices.hh>

#include <dune/common/hybridmultiindex.hh>


namespace Dune {
  namespace TypeTree {

    //! \addtogroup TreePath
    //! \ingroup TypeTree
    //! \{

    //! A hybrid version of TreePath that supports both compile time and run time indices.
    /**
     * A `HybridTreePath` supports storing a combination of run time and compile time indices.
     * This makes it possible to store the tree path to a tree node inside the tree node itself,
     * even if the path contains one or more `PowerNode`s, where each child must have exactly the
     * same type. At the same time, as much information as possible is kept accessible at compile
     * time, allowing for more efficient algorithms.
     *
     * \note Internally all indices are stored as std::size_t or
     * std::integral_constant<std::size_t,v>. The latter is the same
     * as Dune::index_constant<v>. If indices of other integral
     * or std::integral_constant types are passed as arguments,
     * they are converted.
     */
    template<typename... T>
    using HybridTreePath = Dune::HybridMultiIndex<T...>;

    //! Constructs a new `HybridTreePath` from the given indices.
    /**
     * This function returns a new `HybridTreePath` with the given index values. It exists
     * mainly to avoid having to manually specify the exact type of the new object.
     *
     * \note Internally all indices are stored as std::size_t or
     * std::integral_constant<std::size_t,v>. The latter is the same
     * as Dune::index_constant<v>. If indices of other integral
     * or std::integral_constant types are passed as arguments,
     * they are converted.
     */
    template<typename... T>
    requires (((std::is_integral_v<T> or Dune::IsIntegralConstant<T>::value) && ...))
    [[nodiscard]] constexpr auto hybridTreePath(const T&... t)
    {
      return HybridMultiIndex(t...);
    }

    //! Constructs a new `HybridTreePath` from the given indices.
    /**
     * This function returns a new `HybridTreePath` with the given index values. It exists
     * mainly to avoid having to manually specify the exact type of the new object.
     *
     * It further ensures that the basic number type is std::size_t
     * and casts any indices accordingly.
     */
    template<typename... T>
    requires (((std::is_integral_v<T> or Dune::IsIntegralConstant<T>::value) && ...))
    [[nodiscard]] constexpr auto treePath(const T&... t)
    {
      return HybridMultiIndex(t...);
    }

    // Pull in the free utility function for HybridMultiIndex/HybridTreePath
    // We cannot add forwarding functions of the same name here, since this
    // leads to ambiguous overloads.
    // Unfortunately doxygen ignores documentation for using statements.
    using Dune::back;
    using Dune::front;
    using Dune::push_back;
    using Dune::push_front;
    using Dune::accumulate_back;
    using Dune::accumulate_front;
    using Dune::join;
    using Dune::reverse;
    using Dune::pop_front;
    using Dune::pop_back;

    inline namespace Literals {

    //! Literal to create treepath
    /**
     * Example:
     * `2_tp -> HybridTreePath<index_constant<2>>`
     **/
    template <char... digits>
    constexpr auto operator""_tp()
    {
      using namespace Dune::Indices::Literals;
      return hybridTreePath(operator""_ic<digits...>());
    }

    } // end namespace Literals


    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune



#endif // DUNE_TYPETREE_TREEPATH_HH
