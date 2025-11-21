// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_TREEPATH_HH
#define DUNE_TYPETREE_TREEPATH_HH

#include <cstddef>
#include <cassert>
#include <iostream>
#include <type_traits>

#include <dune/common/documentation.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/indices.hh>
#include <dune/common/hybridutilities.hh>
#include <dune/common/typelist.hh>

#include <dune/common/hybridmultiindex.hh>


namespace Dune {
  namespace TypeTree {

    // The Impl namespace collects some free standing functions helper functions
    namespace Impl {

      template<class T>
      constexpr bool isHybridSizeT()
      {
        if constexpr (std::is_same_v<T, std::size_t>)
          return true;
        else
        {
          if constexpr (requires { T::value; })
            return std::is_same_v<T, std::integral_constant<std::size_t, T::value>>;
          else
            return false;
        }
      }

      template<class T>
      constexpr auto castToHybridSizeT(T t)
      {
        if constexpr (Dune::IsIntegralConstant<T>::value)
        {
          using VT = typename T::value_type;
          static_assert(
            std::is_convertible_v<VT,std::size_t> &&
            std::is_integral_v<VT> &&
            T::value >= 0,
            "HybridTreePath indices must be convertible to std::size_t or std::integral_constant<std::size_t,v>");
          return std::integral_constant<std::size_t, T::value>{};
        } else {
          static_assert(
            std::is_convertible_v<T,std::size_t> &&
            std::is_integral_v<T>,
            "HybridTreePath indices must be convertible to std::size_t or std::integral_constant<std::size_t,v>");
          assert(t >= 0 &&
            "HybridTreePath indices must be convertible to std::size_t or std::integral_constant<std::size_t,v>");
          return std::size_t(t);
        }
      }

    }

    //! \addtogroup TreePath
    //! \ingroup TypeTree
    //! \{

    namespace TreePathType {
      enum Type { fullyStatic, dynamic };
    }

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename>
    struct TreePathSize;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename,std::size_t>
    struct TreePathPushBack;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename,std::size_t>
    struct TreePathPushFront;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename>
    struct TreePathBack;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename>
    struct TreePathFront;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename, std::size_t...>
    struct TreePathPopBack;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename>
    struct TreePathPopFront;

    /**
     * \deprecated This class template will be removed after Dune 2.11
     */
    template<typename, typename>
    struct TreePathConcat;

    template<std::size_t... i>
    [[deprecated("This function will be removed after Dune 2.11.")]]
    void print_tree_path(std::ostream& os)
    {}

    template<std::size_t k, std::size_t... i>
    [[deprecated("This function will be removed after Dune 2.11.")]]
    void print_tree_path(std::ostream& os)
    {
      os << k << " ";
      print_tree_path<i...>(os);
    }

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

    //! helper function to construct a new `HybridTreePath` from the given indices.
    /**
     * This function returns a new `HybridTreePath` with the given index values.
     *
     * \note Internally all indices are stored as std::size_t or
     * std::integral_constant<std::size_t,v>. The latter is the same
     * as Dune::index_constant<v>. If indices of other integral
     * or std::integral_constant types are passed as arguments,
     * they are converted.
     */
    template<typename... T>
    requires (((std::is_integral_v<T> or Dune::IsIntegralConstant<T>::value) && ...))
    [[nodiscard]] constexpr auto makeTreePath(const T... t)
    {
      return HybridMultiIndex(t...);
    }

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


    //! Returns the size (number of components) of the given `HybridTreePath`.
    template<typename... T>
    [[nodiscard]] constexpr std::size_t treePathSize(const HybridTreePath<T...>&)
    {
      return sizeof...(T);
    }

    //! Returns a copy of the i-th element of the `HybridTreePath`.
    /**
     * As `HybridTreePath` instances should not be mutated after their creation, this function
     * returns a copy of the value. As values are either `std::integral_constant` or `std::size_t`, that's
     * just as cheap as returning a reference.
     *
     * The index for the entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::Indices;
     * // the following two lines are equivalent
     * std::cout << treePathEntry<2>(tp) << std::endl;
     * std::cout << treePathEntry(tp,_2) << std::endl;
     * \endcode
     *
     */
    template<std::size_t i, typename... T>
    [[nodiscard]] constexpr auto treePathEntry(const HybridTreePath<T...>& tp, index_constant<i> = {})
    {
      return tp[index_constant<i>{}];
    }

    //! Returns the index value of the i-th element of the `HybridTreePath`.
    /**
     * This function always returns the actual index value, irrespective of whether the
     * entry is a compile time index or a run time index.
     *
     * \deprecated The functions is deprecated. Use operator[] instead.
     *
     * The index for the entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::Indices;
     * // the following two lines are equivalent
     * std::cout << treePathIndex<2>(tp) << std::endl;
     * std::cout << treePathIndex(tp,_2) << std::endl;
     * \endcode
     *
     */
    template<std::size_t i,typename... T>
    [[deprecated("This function will be removed after Dune 2.11. Use operator[] instead.")]]
    [[nodiscard]] constexpr std::size_t treePathIndex(const HybridTreePath<T...>& tp, index_constant<i> = {})
    {
      return tp[index_constant<i>{}];
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


    template<std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the size() member function instead.")]]
    TreePathSize<HybridTreePath<index_constant<i>...> >
      : public index_constant<sizeof...(i)>
    {};


    template<std::size_t k, std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the free push_back() function instead.")]]
    TreePathPushBack<HybridTreePath<index_constant<i>...>,k>
    {
      typedef HybridTreePath<index_constant<i>...,index_constant<k>> type;
    };

    template<std::size_t k, std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the free push_front() function instead.")]]
    TreePathPushFront<HybridTreePath<index_constant<i>...>,k>
    {
      typedef HybridTreePath<index_constant<k>,index_constant<i>...> type;
    };

    template<std::size_t k>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the back() member function instead.")]]
    TreePathBack<HybridTreePath<index_constant<k>>>
      : public index_constant<k>
    {};

    template<std::size_t j, std::size_t k, std::size_t... l>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the back() member function instead.")]]
    TreePathBack<HybridTreePath<index_constant<j>,index_constant<k>,index_constant<l>...>>
      : public TreePathBack<HybridTreePath<index_constant<k>,index_constant<l>...>>
    {};

    template<std::size_t k, std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the front() member function instead.")]]
    TreePathFront<HybridTreePath<index_constant<k>,index_constant<i>...>>
      : public index_constant<k>
    {};

    template<std::size_t k, std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the free pop_back() function instead.")]]
    TreePathPopBack<HybridTreePath<index_constant<k>>,i...>
    {
      typedef HybridTreePath<index_constant<i>...> type;
    };

    template<std::size_t j,
             std::size_t k,
             std::size_t... l,
             std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the free pop_back() function instead.")]]
    TreePathPopBack<HybridTreePath<index_constant<j>,index_constant<k>,index_constant<l>...>,i...>
      : public TreePathPopBack<HybridTreePath<index_constant<k>,index_constant<l>...>,i...,j>
    {};

    template<std::size_t k, std::size_t... i>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the free pop_front() function instead.")]]
    TreePathPopFront<HybridTreePath<index_constant<k>,index_constant<i>...> >
    {
      typedef HybridTreePath<index_constant<i>...> type;
    };

    template<std::size_t... i, std::size_t... k>
    struct
    [[deprecated("This class will be removed after Dune 2.11. Use the free join() function instead.")]]
    TreePathConcat<HybridTreePath<index_constant<i>...>,HybridTreePath<index_constant<k>...> >
    {
      typedef HybridTreePath<index_constant<i>...,index_constant<k>...> type;
    };

    template<std::size_t... i>
    using StaticTreePath = HybridTreePath<Dune::index_constant<i>...>;

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune



#endif // DUNE_TYPETREE_TREEPATH_HH
