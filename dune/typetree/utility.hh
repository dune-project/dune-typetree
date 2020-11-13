// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_UTILITY_HH
#define DUNE_TYPETREE_UTILITY_HH

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <dune/common/shared_ptr.hh>
#include <dune/common/indices.hh>
#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/typetree/visitor.hh>

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

    // only bind to real rvalues
    template<typename T>
    typename std::enable_if<!std::is_lvalue_reference<T>::value,std::shared_ptr<T> >::type convert_arg(T&& t)
    {
      return std::make_shared<T>(std::forward<T>(t));
    }

#endif // DOXYGEN


#ifndef DOXYGEN

    // ********************************************************************************
    // TreeInfo specializations for the different node types
    // ********************************************************************************

    struct TreeVisitor;

    namespace Impl {

      template<typename Tree, typename Tag = StartTag>
      struct BaseTreeInfo
      {

      private:
        // Start the tree traversal
        typedef BaseTreeInfo<Tree,NodeTag<Tree>> NodeInfo;

      public:

        //! True if the TypeTree has at least one dynamic node
        static const bool dynamic = NodeInfo::dynamic;

        //! The depth of the TypeTree.
        static const std::size_t depth = NodeInfo::depth;

        //! The total number of nodes in the TypeTree.
        static const std::size_t nodeCount = NodeInfo::nodeCount;

        //! The number of leaf nodes in the TypeTree.
        static const std::size_t leafCount = NodeInfo::leafCount;

      };

      constexpr std::size_t voidCount = std::size_t(0);

      // leaf node
      template<typename Node>
      struct BaseTreeInfo<Node,LeafNodeTag>
      {

        static const bool dynamic = false;

        static const std::size_t depth = 1;

        static const std::size_t nodeCount = 1;

        static const std::size_t leafCount = 1;

      };


      // power node - exploit the fact that all children are identical
      template<typename Node>
      struct BaseTreeInfo<Node,PowerNodeTag>
      {

        typedef BaseTreeInfo<typename Node::ChildType,NodeTag<typename Node::ChildType>> ChildInfo;

        static const bool dynamic = ChildInfo::dynamic;

        static const std::size_t depth = 1 + ChildInfo::depth;

        static const std::size_t nodeCount = 1 + StaticDegree<Node>::value * ChildInfo::nodeCount;

        static const std::size_t leafCount = StaticDegree<Node>::value * ChildInfo::leafCount;

      };

      // power node - exploit the fact that all children are identical
      template<typename Node>
      struct BaseTreeInfo<Node,DynamicPowerNodeTag>
      {

        typedef BaseTreeInfo<typename Node::ChildType,NodeTag<typename Node::ChildType>> ChildInfo;

        static const bool dynamic = true;

        static const std::size_t depth = 1 + ChildInfo::depth;

        static const std::size_t nodeCount = voidCount;

        static const std::size_t leafCount = voidCount;

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
          typedef NodeTag<Child> ChildTag;
          typedef BaseTreeInfo<Child,ChildTag> ChildInfo;

          static const bool dynamic = ChildInfo::dynamic or NextChild::dynamic;

          // combine information of current child with info about following children
          static const std::size_t maxDepth = ChildInfo::depth > NextChild::maxDepth ? ChildInfo::depth : NextChild::maxDepth;

          static const std::size_t nodeCount = ChildInfo::nodeCount + NextChild::nodeCount;

          static const std::size_t leafCount = ChildInfo::leafCount + NextChild::leafCount;

        };

        // End of recursion
        template<typename Node, std::size_t n>
        struct generic_compositenode_children_info<Node,n,n>
        {
          static const bool dynamic = false;

          static const std::size_t maxDepth = 0;

          static const std::size_t nodeCount = 0;

          static const std::size_t leafCount = 0;
        };

      } // anonymous namespace


        // Struct for building information about composite node
      template<typename Node>
      struct GenericCompositeNodeInfo
      {

        typedef generic_compositenode_children_info<Node,0,StaticDegree<Node>::value> Children;

        static const bool dynamic = Children::dynamic;

        static const std::size_t depth = 1 + Children::maxDepth;

        static const std::size_t nodeCount = 1 + Children::nodeCount;

        static const std::size_t leafCount = Children::leafCount;

      };


      // CompositeNode: delegate to GenericCompositeNodeInfo
      template<typename Node>
      struct BaseTreeInfo<Node,CompositeNodeTag>
        : public GenericCompositeNodeInfo<Node>
      {};

      template<typename Node, bool d = BaseTreeInfo<Node>::dynamic>
      struct TreeInfo;

      template<typename Node>
      struct TreeInfo<Node,false>
      {
        static const bool dynamic = false;

        static const std::size_t depth = BaseTreeInfo<Node>::depth;

        static const std::size_t nodeCount = BaseTreeInfo<Node>::nodeCount;

        static const std::size_t leafCount = BaseTreeInfo<Node>::leafCount;
      };

      template<typename Node>
      struct TreeInfo<Node,true>
      {
        static const bool dynamic = true;

        static const std::size_t depth = BaseTreeInfo<Node>::depth;
      };

    }
#endif // DOXYGEN

    //! Struct for obtaining some basic structural information about a TypeTree.
    /**
     * This struct extracts basic information about the passed TypeTree and
     * presents them in a static way when possible.
     *
     * \tparam Tree  The TypeTree to examine.
     */
    template<typename Tree>
    struct TreeInfo
#ifndef DOXYGEN
    : public Impl::TreeInfo<Tree> {};
#else
    {
      //! True if the TypeTree has at least one dynamic node
      static const bool dynamic;

      //! The depth of the TypeTree.
      static const std::size_t depth;

      //! The total number of nodes in the TypeTree. Only for static trees.
      static const std::size_t nodeCount;

      //! The number of leaf nodes in the TypeTree. Only for static trees.
      static const std::size_t leafCount;

    };
#endif // DOXYGEN

    //! True if the TypeTree has at least one dynamic node
    template<typename Tree>
    constexpr
    bool dynamic(const Tree& tree)
    {
      return TreeInfo<Tree>::dynamic;
    }

    //! The depth of the TypeTree.
    template<typename Tree>
    constexpr
    std::size_t depth(const Tree& tree)
    {
      return TreeInfo<Tree>::depth;
    }

    //! The total number of nodes in the TypeTree.
    template<typename Tree>
    std::enable_if_t<TreeInfo<Tree>::dynamic, std::size_t>
    nodeCount(const Tree& tree)
    {
      CountVisitor counter;
      applyToTree(tree,counter);
      return counter.nodeCount;
    }

    //! The total number of nodes in the TypeTree.
    template<typename Tree>
    constexpr
    std::enable_if_t<!TreeInfo<Tree>::dynamic, std::size_t>
    nodeCount(const Tree& tree)
    {
      return TreeInfo<Tree>::nodeCount;
    }

    //! The number of leaf nodes in the TypeTree.
    template<typename Tree>
    std::enable_if_t<TreeInfo<Tree>::dynamic, std::size_t>
    leafCount(const Tree& tree)
    {
      CountVisitor counter;
      applyToTree(tree,counter);
      return counter.leafCount;
    }

    //! The number of leaf nodes in the TypeTree.
    template<typename Tree>
    constexpr
    std::enable_if_t<!TreeInfo<Tree>::dynamic, std::size_t>
    leafCount(const Tree& tree)
    {
      return TreeInfo<Tree>::leafCount;
    }

    using Dune::index_constant;
    namespace Indices = Dune::Indices;

    //! No-op function to make calling a function on a variadic template argument pack legal C++.
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
      void _apply_to_tuple(T&& t, F&& f, std::index_sequence<i...>,apply_to_tuple_policy::no_pass_index)
      {
        discard((f(std::get<i>(std::forward<T>(t))),0)...);
      }

      // version that passes index
      template<typename T, typename F, std::size_t... i>
      void _apply_to_tuple(T&& t, F&& f, std::index_sequence<i...>,apply_to_tuple_policy::pass_index)
      {
        discard((f(index_constant<i>{},std::get<i>(std::forward<T>(t))),0)...);
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
      const std::size_t size = std::tuple_size<typename std::decay<T>::type>::value;
      _apply_to_tuple(
        std::forward<T>(t),
        std::forward<F>(f),
        std::make_index_sequence<size>{},
        Policy()
        );
    }

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_UTILITY_HH
