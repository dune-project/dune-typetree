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
    void print_tree_path(std::ostream& os)
    {}

    template<std::size_t k, std::size_t... i>
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
     * as Dune::index_constant<v>.
     */
    template<typename... T>
    class HybridTreePath
    {

      // make sure that all indices use std::size_t as the underlying number type
      static_assert((... && Impl::isHybridSizeT<T>()),
        "HybridTreePath index storage must be std::size_t or std::integral_constant<std::size_t,v>");

    public:

      //! An `index_sequence` for the entries in this `HybridTreePath`.
      using index_sequence = std::index_sequence_for<T...>;

      //! Default constructor
      constexpr HybridTreePath() = default;

      constexpr HybridTreePath(const HybridTreePath& tp) = default;
      constexpr HybridTreePath(HybridTreePath&& tp) = default;

      constexpr HybridTreePath& operator=(const HybridTreePath& tp) = default;
      constexpr HybridTreePath& operator=(HybridTreePath&& tp) = default;

      //! Constructor from a `std::tuple`
      explicit constexpr HybridTreePath(std::tuple<T...> t)
        : _data(t)
      {}

      //! Constructor from arguments
      template<typename... U,
        typename std::enable_if_t<(sizeof...(T) > 0 && sizeof...(U) == sizeof...(T)),bool> = true>
      explicit constexpr HybridTreePath(U... t)
        : _data(t...) // we assume that all arguments are convertible to the types T...
      {}

      //! Returns an index_sequence for enumerating the components of this HybridTreePath.
      [[nodiscard]] constexpr static index_sequence enumerate()
      {
        return {};
      }

      //! Get the size (length) of this path.
      [[nodiscard]] constexpr static std::size_t size()
      {
        return sizeof...(T);
      }

      //! Get the size (length) of this path.
      [[nodiscard]] constexpr static std::size_t max_size()
      {
        return size();
      }

      //! Get the index value at position pos.
      template<std::size_t i,
        std::enable_if_t<(sizeof...(T) > i),bool> = true>
      [[nodiscard]] constexpr auto operator[](Dune::index_constant<i>) const
      {
        return std::get<i>(_data);
      }

      //! Get the index value at position pos.
      [[nodiscard]] constexpr std::size_t operator[](std::size_t pos) const
      {
        std::size_t entry = 0;
        Dune::Hybrid::forEach(enumerate(), [&] (auto i) {
            if (i==pos)
              entry = (*this)[i];
        });
        return entry;
      }

      //! Get the last index value.
      template<std::size_t i,
        std::enable_if_t<(sizeof...(T) > i),bool> = true>
      [[nodiscard]] constexpr auto element(Dune::index_constant<i> pos = {}) const
      {
        return std::get<i>(_data);
      }

      //! Get the index value at position pos.
      [[nodiscard]] constexpr std::size_t element(std::size_t pos) const
      {
        std::size_t entry = 0;
        Dune::Hybrid::forEach(enumerate(), [&] (auto i) {
            if (i==pos)
              entry = (*this)[i];
        });
        return entry;
      }

      //! Get the first index value. Only available in non-empty paths
      template<std::size_t n = sizeof...(T),
        std::enable_if_t<(n > 0 && n == sizeof...(T)),bool> = true>
      [[nodiscard]] constexpr auto front() const
      {
        return std::get<0>(_data);
      }

      //! Get the last index value. Only available in non-empty paths
      template<std::size_t n = sizeof...(T),
        std::enable_if_t<(n > 0 && n == sizeof...(T)),bool> = true>
      [[nodiscard]] constexpr auto back() const
      {
        return std::get<n-1>(_data);
      }

#ifndef DOXYGEN

      // I can't be bothered to make all the external accessors friends of HybridTreePath,
      // so we'll only hide the data tuple from the user in Doxygen.

      template<class... Head, class... Other>
      friend constexpr auto join(const HybridTreePath<Head...>&, const Other&...);

      using Data = std::tuple<T...>;
      Data _data;

#endif // DOXYGEN

    };

    //! helper function to construct a new `HybridTreePath` from the given indices.
    /**
     * This function returns a new `HybridTreePath` with the given index values. It is
     * only a helper function used by `treePath` and `hybridTreePath`.
     *
     * It expects that all indices use std::size_t as basic number type.
     */
    template<typename... T>
    [[nodiscard]] constexpr auto makeTreePath(const T... t)
    {
      // check that all entries are based on std::size_t
      static_assert((... && Impl::isHybridSizeT<T>()),
        "HybridTreePath indices must be of type std::size_t or std::integral_constant<std::size_t,v>");
      return HybridTreePath<T...>(t...);
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
    [[nodiscard]] constexpr auto hybridTreePath(const T&... t)
    {
      return makeTreePath(Impl::castToHybridSizeT(t)...);
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
    [[nodiscard]] constexpr auto treePath(const T&... t)
    {
      return makeTreePath(Impl::castToHybridSizeT(t)...);
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
    [[nodiscard]] constexpr std::size_t treePathIndex(const HybridTreePath<T...>& tp, index_constant<i> = {})
    {
      return tp[index_constant<i>{}];
    }

    //! Returns a copy of the last element of the `HybridTreePath`.
    /**
     * As `HybridTreePath` instances should not be mutated after their creation, this function
     * returns a copy of the value. As values are either `std::integral_constant` or `std::size_t`, that's
     * just as cheap as returning a reference.
     */
    template<typename... T>
    [[nodiscard]] constexpr auto back(const HybridTreePath<T...>& tp)
      -> decltype(tp.back())
    {
      return tp.back();
    }

    //! Returns a copy of the first element of the `HybridTreePath`.
    /**
     * As `HybridTreePath` instances should not be mutated after their creation, this function
     * returns a copy of the value. As values are either `std::integral_constant` or `std::size_t`, that's
     * just as cheap as returning a reference.
     */
    template<typename... T>
    [[nodiscard]] constexpr auto front(const HybridTreePath<T...>& tp)
      -> decltype(tp.front())
    {
      return tp.front();
    }

    //! Appends a run time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the run time index `i` appended.
     */
    template<typename... T>
    [[nodiscard]] constexpr HybridTreePath<T...,std::size_t> push_back(const HybridTreePath<T...>& tp, std::size_t i)
    {
      return unpackIntegerSequence([&](auto... j){
        return treePath(tp[j] ..., i);
      }, tp.enumerate());
    }

    //! Appends a compile time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the compile time index `i` appended.
     *
     * The value for the new entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::Indices;
     * // the following two lines are equivalent
     * auto tp_a = push_back<1>(tp);
     * auto tp_b = push_back(tp,_1);
     * \endcode
     *
     */
    template<std::size_t i, typename... T>
    [[nodiscard]] constexpr HybridTreePath<T...,index_constant<i>> push_back(const HybridTreePath<T...>& tp, index_constant<i> iConstant = {})
    {
      return unpackIntegerSequence([&](auto... j){
        return treePath(tp[j] ..., iConstant);
      }, tp.enumerate());
    }

    //! Prepends a run time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the run time index `i` prepended.
     */
    template<typename... T>
    [[nodiscard]] constexpr HybridTreePath<std::size_t,T...> push_front(const HybridTreePath<T...>& tp, std::size_t i)
    {
      return unpackIntegerSequence([&](auto... j){
        return treePath(i, tp[j] ...);
      }, tp.enumerate());
    }

    //! Prepends a compile time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the compile time index `i` prepended.
     *
     * The value for the new entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::Indices;
     * // the following two lines are equivalent
     * auto tp_a = push_front<1>(tp);
     * auto tp_b = push_front(tp,_1);
     * \endcode
     *
     */
    template<std::size_t i, typename... T>
    [[nodiscard]] constexpr HybridTreePath<index_constant<i>,T...> push_front(const HybridTreePath<T...>& tp, index_constant<i> iConstant = {})
    {
      return unpackIntegerSequence([&](auto... j){
        return treePath(iConstant, tp[j] ...);
      }, tp.enumerate());
    }

    //! Hybrid utility that accumulates to the back of a multi-index
    /**
     * @brief The back of the path will be accumulated and promoted in order to
     * hold the new index:
     *
     * \code{.cc}
     *  accumulate_back(treePath(_0,_2),_2) -> treePath(_0,_4)
     *  accumulate_back(treePath(_0,_2), 2) -> treePath(_0, 4)
     *  accumulate_back(treePath(_0, 2),_2) -> treePath(_0, 4)
     *  accumulate_back(treePath(_0, 2), 2) -> treePath(_0, 4)
     * \endcode
     */
    template<typename I, typename... T, std::enable_if_t<(sizeof...(T) > 0),bool> = true>
    [[nodiscard]] constexpr auto accumulate_back(const HybridTreePath<T...>& tp, I i) {
      using ::Dune::Hybrid::plus;
      return push_back(pop_back(tp), plus(back(tp), i));
    }


    //! Hybrid utility that accumulates to the front of a multi-index
    /**
     * @brief The front of the path will be accumulated and promoted in order to
     * hold the new index:
     *
     * \code{.cc}
     *  accumulate_front(treePath(_0,_2),_2) -> treePath(_2,_2)
     *  accumulate_front(treePath(_0,_2), 2) -> treePath( 2,_2)
     *  accumulate_front(treePath( 0,_2),_2) -> treePath( 2,_2)
     *  accumulate_front(treePath( 0,_2), 2) -> treePath( 2,_2)
     * \endcode
     */
    template<typename I, typename... T, std::enable_if_t<(sizeof...(T) > 0),bool> = true>
    [[nodiscard]] constexpr auto accumulate_front(const HybridTreePath<T...>& tp, I i) {
      using ::Dune::Hybrid::plus;
      return push_front(pop_front(tp), plus(front(tp), i));
    }

    //! Join two tree paths into one
    template<class... Head, class... Other>
    [[nodiscard]] constexpr auto join(const HybridTreePath<Head...>& head, const Other&... tail) {
      return TypeTree::HybridTreePath{std::tuple_cat(head._data, tail._data...)};
    }

    //! Reverses the order of the elements in the path
    template<class... T>
    [[nodiscard]] constexpr auto reverse(const HybridTreePath<T...>& tp) {
      constexpr std::size_t size = sizeof...(T);
      return unpackIntegerSequence([&](auto... i){
        return treePath(tp[index_constant<size-i-1>{}] ...);
      }, std::make_index_sequence<size>{});
    }

    //! Removes first index on a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` without the first index.
     */
    template <class... T, std::enable_if_t<(sizeof...(T) > 0),bool> = true>
    [[nodiscard]] constexpr auto pop_front(const HybridTreePath<T...>& tp)
    {
      return unpackIntegerSequence([&](auto... i){
        return HybridTreePath{std::make_tuple(tp[Dune::index_constant<i+1>{}]...)};
      }, std::make_index_sequence<(sizeof...(T) - 1)>{});
    }

    //! Removes last index on a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` without the last index.
     */
    template <class... T, std::enable_if_t<(sizeof...(T) > 0),bool> = true>
    [[nodiscard]] constexpr auto pop_back(const HybridTreePath<T...>& tp)
    {
      return unpackIntegerSequence([&](auto... i){
        return HybridTreePath{std::make_tuple(tp[i]...)};
      }, std::make_index_sequence<(sizeof...(T) - 1)>{});
    }

    //! Compare two `HybridTreePath`s for value equality
    /**
     * The function returns true if both tree paths are of the same length
     * and all entries have the same value.
     *
     * Note, it might be that the values are represented with different types.
     * To check for same value and same type, use a combination of `std::is_same`
     * and this comparison operator.
     **/
    template <class... S, class... T>
    [[nodiscard]] constexpr bool operator==(
      const HybridTreePath<S...>& lhs,
      const HybridTreePath<T...>& rhs)
    {
      if constexpr (sizeof...(S) == sizeof...(T)) {
        if constexpr ((Dune::IsInteroperable<S,T>::value &&...)) {
          return unpackIntegerSequence([&](auto... i){
            return ((lhs[i] == rhs[i]) &&...);
          }, lhs.enumerate());
        } else {
          return false;
        }
      } else {
        return false;
      }
    }

    //! Overload for purely static `HybridTreePath`s.
    /**
     * The function returns `std::true_type` if the values of the passed
     * treepaths are equal. Otherwise returns `std::false_type`. Note, this
     * overload is chosen for purely static treepaths only.
     **/
    template <class S, S... lhs, class T, T... rhs>
    [[nodiscard]] constexpr auto operator==(
      const HybridTreePath<std::integral_constant<S,lhs>...>&,
      const HybridTreePath<std::integral_constant<T,rhs>...>&)
    {
      return std::bool_constant<hybridTreePath(lhs...) == hybridTreePath(rhs...)>{};
    }


    //! Compare two `HybridTreePath`s for unequality
    template <class... S, class... T>
    [[nodiscard]] constexpr auto operator!=(
      const HybridTreePath<S...>& lhs,
      const HybridTreePath<T...>& rhs)
    {
      return !(lhs == rhs);
    }

    //! Compare two static `HybridTreePath`s for unequality
    template <class S, S... lhs, class T, T... rhs>
    [[nodiscard]] constexpr auto operator!=(
      const HybridTreePath<std::integral_constant<S,lhs>...>&,
      const HybridTreePath<std::integral_constant<T,rhs>...>&)
    {
      return std::bool_constant<hybridTreePath(lhs...) != hybridTreePath(rhs...)>{};
    }


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

    //! Dumps a `HybridTreePath` to a stream.
    template<typename... T>
    std::ostream& operator<<(std::ostream& os, const HybridTreePath<T...>& tp)
    {
      os << "HybridTreePath< ";
      Dune::Hybrid::forEach(tp, [&] (auto tp_i) {
        os << tp_i << " ";
      });
      os << ">";
      return os;
    }

    template<std::size_t... i>
    using StaticTreePath = HybridTreePath<Dune::index_constant<i>...>;

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TREEPATH_HH
