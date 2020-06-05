// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TYPETRAITS_HH
#define DUNE_TYPETREE_TYPETRAITS_HH

#include <type_traits>
#include <dune/common/typetraits.hh>

#include <dune/typetree/treepath.hh>
#include <dune/typetree/nodeinterface.hh>

namespace Dune {

  // Provide some more C++11 TMP helpers.
  // These should be upstreamed to dune-common ASAP.

  template<typename... T>
  struct first_type;

  template<typename T0, typename... T>
  struct first_type<T0,T...>
  {
    typedef T0 type;
  };

  namespace TypeTree {

    template<typename T>
    struct has_node_tag
    {
      struct yes { char dummy[1]; };
      struct no  { char dummy[2]; };

      template<typename X>
      static yes test(NodeTag<X> *);
      template<typename X>
      static no  test(...);

      enum {
        /** @brief True if class T defines a NodeTag. */
        value = sizeof(test<T>(0)) == sizeof(yes)
      };
    };

    template<typename T, typename V>
    struct has_node_tag_value
    {
      template<int N>
      struct maybe { char dummy[N+1]; };
      struct yes { char dummy[2]; };
      struct no  { char dummy[1]; };

      template<typename X>
      static maybe<std::is_base_of<V, NodeTag<X>>::value>
      test(NodeTag<X> * a);
      template<typename X>
      static no test(...);

      enum {
        /** @brief True if class T defines a NodeTag of type V. */
        value = sizeof(test<T>(0)) == sizeof(yes)
      };
    };

    template<typename T>
    struct has_implementation_tag
    {
      struct yes { char dummy[1]; };
      struct no  { char dummy[2]; };

      template<typename X>
      static yes test(ImplementationTag<X> *);
      template<typename X>
      static no  test(...);

      enum {
        /** @brief True if class T defines an ImplementationTag. */
        value = sizeof(test<T>(0)) == sizeof(yes)
      };
    };

    template<typename T, typename V>
    struct has_implementation_tag_value
    {
      template<int N>
      struct maybe { char dummy[N+1]; };
      struct yes { char dummy[2]; };
      struct no  { char dummy[1]; };

      template<typename X>
      static maybe<std::is_base_of<V, ImplementationTag<X>>::value>
      test(ImplementationTag<X> * a);
      template<typename X>
      static no test(...);

      enum {
        /** @brief True if class T defines an ImplementationTag of type V. */
        value = sizeof(test<T>(0)) == sizeof(yes)
      };
    };

    template<typename>
    struct AlwaysVoid
    {
      typedef void type;
    };


    //! Helper function for generating a pointer to a value of type T in an unevaluated operand setting.
    template<typename T>
    T* declptr();


    // Support for lazy evaluation of meta functions. This is required when doing
    // nested tag dispatch without C++11-style typedefs (based on using syntax).
    // The standard struct-based meta functions cause premature evaluation in a
    // context that is not SFINAE-compatible. We thus have to return the meta function
    // without evaluating it, placing that burden on the caller. On the other hand,
    // the lookup will often directly be the target type, so here is some helper code
    // to automatically do the additional evaluation if necessary.
    // Too bad that the new syntax is GCC 4.6+...


    //! Marker tag declaring a meta function.
    /**
     * Just inherit from this type to cause lazy evaluation
     */
    struct meta_function {};

    //! Helper meta function to delay evaluation of F.
    template<typename F>
    struct lazy_evaluate
    {
      typedef typename F::type type;
    };

    //! Identity function.
    template<typename F>
    struct lazy_identity
    {
      typedef F type;
    };

    //! Meta function that evaluates its argument iff it inherits from meta_function.
    template<typename F>
    struct evaluate_if_meta_function
    {
      typedef typename std::conditional<
        std::is_base_of<meta_function,F>::value,
        lazy_evaluate<F>,
        lazy_identity<F>
        >::type::type type;
    };

    namespace impl {

      // Check if type is a or is derived from one of the tree path types

      // Default overload for types not representing a tree path
      constexpr auto isTreePath(void*)
        -> std::false_type
      {
        return std::false_type();
      }

      // Overload for instances of HybridTreePath<...>
      template<class... I>
      constexpr auto isTreePath(const HybridTreePath<I...>*)
        -> std::true_type
      {
        return std::true_type();
      }

    }

    /**
     * \brief Check if type represents a tree path
     *
     * If T is a or derived from one of the tree path types this
     * struct derives from std::true_type, otherwise from std::false_type.
     * Hence the result of the check is available via ::value, cast to bool,
     * or operator().
     *
     * \tparam T Check if this type represents a tree path
     */
    template<class T>
    struct IsTreePath :
      public decltype(impl::isTreePath((typename std::decay<T>::type*)(nullptr)))
    {};

    /**
     * \brief Check if given object represents a tree path
     *
     * \tparam T Check if this type represents a tree path
     * \returns std::true_type if argument is a tree path and std::false_type if not
     */
    template<class T>
    constexpr auto isTreePath(const T&)
      -> IsTreePath<T>
    {
      return IsTreePath<T>();
    }


  } // end namespace TypeTree
} // end namespace Dune

#endif // DUNE_TYPETREE_TYPETRAITS_HH
