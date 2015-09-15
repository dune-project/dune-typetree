// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_UTILITY_HH
#define DUNE_TYPETREE_UTILITY_HH

#include <memory>
#include <tuple>
#include <type_traits>

#include <dune/common/std/utility.hh>
#include <dune/typetree/nodetags.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup TypeTree
     *  \{
     */

#ifndef DOXYGEN

    template<typename T>
    std::shared_ptr<T> convert_arg(const T& t)
    {
      return std::make_shared<T>(t);
    }

    template<typename T>
    std::shared_ptr<T> convert_arg(T& t)
    {
      return stackobject_to_shared_ptr(t);
    }

    template<typename BaseType, typename T>
    T& assertGridViewType(T& t)
    {
      static_assert((std::is_same<typename BaseType::Traits::GridViewType,
                     typename T::Traits::GridViewType>::value),
                    "GridViewType must be equal in all components of composite type");
      return t;
    }

    // this partial specialization is required for the non-variadic case
    template<typename BaseType>
    TypeTree::EmptyNode assertGridViewType(TypeTree::EmptyNode e)
    {
      return e;
    }

    // only bind to real rvalues
    template<typename T>
    typename std::enable_if<!std::is_lvalue_reference<T>::value,std::shared_ptr<T> >::type convert_arg(T&& t)
    {
      return std::make_shared<T>(std::forward<T>(t));
    }

#endif // DOXYGEN

    //! Reference to a pointer to an empty node that is used for all empty slots
    const std::shared_ptr<EmptyNode>& emptyNodePtr();

    //! Struct for obtaining some basic structural information about a TypeTree.
    /**
     * This struct extracts basic information about the passed TypeTree and
     * presents them in a static way suitable for use as compile-time constants.
     *
     * \tparam Tree  The TypeTree to examine.
     * \tparam Tag   Internal parameter, leave at default value.
     */
    template<typename Tree, typename Tag = StartTag>
    struct TreeInfo
    {

    private:
      // Start the tree traversal
      typedef TreeInfo<Tree,typename Tree::NodeTag> NodeInfo;

    public:

      //! The depth of the TypeTree.
      static const std::size_t depth = NodeInfo::depth;

      //! The total number of nodes in the TypeTree.
      static const std::size_t nodeCount = NodeInfo::nodeCount;

      //! The number of leaf nodes in the TypeTree.
      static const std::size_t leafCount = NodeInfo::leafCount;

    };


#ifndef DOXYGEN

    // ********************************************************************************
    // TreeInfo specializations for the different node types
    // ********************************************************************************


    // leaf node
    template<typename Node>
    struct TreeInfo<Node,LeafNodeTag>
    {

      static const std::size_t depth = 1;

      static const std::size_t nodeCount = 1;

      static const std::size_t leafCount = 1;

    };


    // power node - exploit the fact that all children are identical
    template<typename Node>
    struct TreeInfo<Node,PowerNodeTag>
    {

      typedef TreeInfo<typename Node::ChildType,typename Node::ChildType::NodeTag> ChildInfo;

      static const std::size_t depth = 1 + ChildInfo::depth;

      static const std::size_t nodeCount = 1 + Node::CHILDREN * ChildInfo::nodeCount;

      static const std::size_t leafCount = Node::CHILDREN * ChildInfo::leafCount;

    };


    namespace {

      // TMP for iterating over the children of a composite node
      // identical for both composite node implementations
      template<typename Node, std::size_t k, std::size_t n>
      struct generic_compositenode_children_info
      {

        typedef generic_compositenode_children_info<Node,k+1,n> NextChild;

        // extract child info
        typedef typename Node::template Child<k>::Type Child;
        typedef typename Child::NodeTag ChildTag;
        typedef TreeInfo<Child,ChildTag> ChildInfo;

        // combine information of current child with info about following children
        static const std::size_t maxDepth = ChildInfo::depth > NextChild::maxDepth ? ChildInfo::depth : NextChild::maxDepth;

        static const std::size_t nodeCount = ChildInfo::nodeCount + NextChild::nodeCount;

        static const std::size_t leafCount = ChildInfo::leafCount + NextChild::leafCount;

      };

      // End of recursion
      template<typename Node, std::size_t n>
      struct generic_compositenode_children_info<Node,n,n>
      {
        static const std::size_t maxDepth = 0;

        static const std::size_t nodeCount = 0;

        static const std::size_t leafCount = 0;
      };

    } // anonymous namespace


      // Struct for building information about composite node
    template<typename Node>
    struct GenericCompositeNodeInfo
    {

      typedef generic_compositenode_children_info<Node,0,Node::CHILDREN> Children;

      static const std::size_t depth = 1 + Children::maxDepth;

      static const std::size_t nodeCount = 1 + Children::nodeCount;

      static const std::size_t leafCount = Children::leafCount;

    };


    // CompositeNode: delegate to GenericCompositeNodeInfo
    template<typename Node>
    struct TreeInfo<Node,CompositeNodeTag>
      : public GenericCompositeNodeInfo<Node>
    {};


#endif // DOXYGEN


    //! Simple holder class for a template argument pack of indices.
    /**
     * The main use of index_pack is to unpack variadically templated
     * data structures like this:
     *
     * \code
     * template<typename T, typename F, std::size_t... i>
     * void apply_to_tuple(const T& t, F f, index_pack<i...> indices)
     * {
     *   discard((f(get<i>(t)),0)...);
     * }
     *
     * std::tuple<int,double,...,char> t;
     * apply_to_tuple(t,foo,tuple_indices(t));
     * \endcode
     *
     * \sa tuple_indices()
     * \sa discard()
     */
    template<std::size_t... i>
    struct index_pack {};

    //! TMP to build an index_pack containing the sequence 0,...,n-1.
    template<std::size_t n, std::size_t... i>
    struct index_pack_builder
      : public index_pack_builder<n-1,n-1,i...>
    {

#ifdef DOXYGEN
      //! Result.
      typedef index_pack<0,1,...,n-1> type;
#endif // DOXYGEN

    };

#ifndef DOXYGEN

    // end of recursion
    template<std::size_t... i>
    struct index_pack_builder<0,i...>
    {
      typedef index_pack<i...> type;
    };

#endif // DOXYGEN

    //! TMP to build an index_pack for all elements in the tuple.
    template<typename tuple>
    struct tuple_index_pack_builder
      : public index_pack_builder<std::tuple_size<tuple>::value>
    {};

    //! Generate an index_pack for the tuple t.
    template<typename tuple>
    typename tuple_index_pack_builder<tuple>::type tuple_indices(const tuple& t)
    {
      return typename tuple_index_pack_builder<tuple>::type();
    }

    //! Generate an index_pack with the values {0, 1, ..., n-1}.
    /**
     * \tparam n The length of the index pack.
     * \return   index_pack<0,1,...,n-1>.
     **/
    template<std::size_t n>
    typename index_pack_builder<n>::type index_range(std::integral_constant<std::size_t,n> = {})
    {
      return typename index_pack_builder<n>::type();
    }

    namespace Std {

      //! Backport of C++14 std::integer_sequence.
      /**
       * This is just imported from Dune::Std because the version in Dune::Std
       * does not implement the other parts of integer sequences correctly due
       * to a lack of template aliases.
       */
      using Dune::Std::integer_sequence;

      //! A sequence of indices, with each entry a std::size_t.
      template<std::size_t... indices>
      using index_sequence = integer_sequence<std::size_t,indices...>;

      //! Create an integer_sequence [0,n-1] with entries of type T.
      template<typename T, T n>
      using make_integer_sequence = decltype(Dune::Std::make_integer_sequence<T,n>());

      //! Create an index_sequence [0,n-1].
      template<std::size_t n>
      using make_index_sequence = make_integer_sequence<std::size_t,n>;

      //! Create an index_sequence for the pack T..., i.e. [0,sizeof...(T)].
      template<typename... T>
      using index_sequence_for = make_index_sequence<sizeof...(T)>;

    }


    //! An index constant with value i.
    /**
     * An index constant is a simple type alias for an integral_constant.
     * Its main advantages are clarity (it is easier to see that code uses it
     * as an index) and the fact that the integral type is fixed, reducing verbosity
     * and avoiding the problem of maybe trying to overload / specialize using a different
     * integral type.
     */
    template<std::size_t i>
    using index_constant = std::integral_constant<std::size_t, i>;



    //! Namespace with predefined compile time indices for the range [0,19].
    /**
     * The predefined index objects in this namespace are `constexpr`, which allows them to
     * be used in situations where a compile time constant is needed, e.g. for a template
     * parameter. Apart from that, `constexpr` implies internal linkage, which helps to avoid
     * ODR problems.
     *
     * The constants implicitly convert to their contained value, so you can for example write
     *
     * \code{.cc}
     * std::array<int,_10> a;
     * // the above line is equivalent to
     * std::array<int,10> b;
     * \endcode
     *
     */
    namespace Indices {

      //! Compile time index with value 0.
      constexpr index_constant< 0>  _0 = {};

      //! Compile time index with value 1.
      constexpr index_constant< 1>  _1 = {};

      //! Compile time index with value 2.
      constexpr index_constant< 2>  _2 = {};

      //! Compile time index with value 3.
      constexpr index_constant< 3>  _3 = {};

      //! Compile time index with value 4.
      constexpr index_constant< 4>  _4 = {};

      //! Compile time index with value 5.
      constexpr index_constant< 5>  _5 = {};

      //! Compile time index with value 6.
      constexpr index_constant< 6>  _6 = {};

      //! Compile time index with value 7.
      constexpr index_constant< 7>  _7 = {};

      //! Compile time index with value 8.
      constexpr index_constant< 8>  _8 = {};

      //! Compile time index with value 9.
      constexpr index_constant< 9>  _9 = {};

      //! Compile time index with value 10.
      constexpr index_constant<10> _10 = {};

      //! Compile time index with value 11.
      constexpr index_constant<11> _11 = {};

      //! Compile time index with value 12.
      constexpr index_constant<12> _12 = {};

      //! Compile time index with value 13.
      constexpr index_constant<13> _13 = {};

      //! Compile time index with value 14.
      constexpr index_constant<14> _14 = {};

      //! Compile time index with value 15.
      constexpr index_constant<15> _15 = {};

      //! Compile time index with value 16.
      constexpr index_constant<16> _16 = {};

      //! Compile time index with value 17.
      constexpr index_constant<17> _17 = {};

      //! Compile time index with value 18.
      constexpr index_constant<18> _18 = {};

      //! Compile time index with value 19.
      constexpr index_constant<19> _19 = {};

    }



    //! No-op function to make calling a function on a variadic template argument pack legal C++.
    /**
     * \sa index_pack
     */
    template<typename... Args>
    void discard(Args&&... args)
    {}

    //! Policies for the function apply_to_tuple().
    namespace apply_to_tuple_policy {

      //! Do not pass the index of the current tuple to the functor
      struct no_pass_index {};

      //! Pass the index of the current tuple to the functor as its first argument in a std::integral_constant.
      struct pass_index {};

      //! Default policy.
      typedef no_pass_index default_policy;

    }

    namespace {

      // version that does not pass index
      template<typename T, typename F, std::size_t... i>
      void _apply_to_tuple(T&& t, F&& f, index_pack<i...> indices,apply_to_tuple_policy::no_pass_index)
      {
        discard((f(std::get<i>(std::forward<T>(t))),0)...);
      }

      // version that passes index
      template<typename T, typename F, std::size_t... i>
      void _apply_to_tuple(T&& t, F&& f, index_pack<i...> indices,apply_to_tuple_policy::pass_index)
      {
        discard((f(std::integral_constant<std::size_t,i>(),std::get<i>(std::forward<T>(t))),0)...);
      }

    }

    //! Apply a functor to each element of a std::tuple.
    /*
     * This function applies the functor f to each element of the std::tuple t.
     * It works for arbitrary combinations of const- and non const lvalues and rvalues.
     * The function accepts an optional policy argument that can currently be used to make
     * it pass the index of the current tuple argument to the functor as a compile time constant
     * in addition to the tuple element itself.
     */
    template<typename T, typename F, typename Policy>
    void apply_to_tuple(T&& t, F&& f, Policy = apply_to_tuple_policy::default_policy())
    {
      _apply_to_tuple(
        std::forward<T>(t),
        std::forward<F>(f),
        tuple_indices(t),
        Policy()
        );
    }

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_UTILITY_HH
