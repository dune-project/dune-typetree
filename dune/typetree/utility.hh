// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_UTILITY_HH
#define DUNE_TYPETREE_UTILITY_HH

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <algorithm>

#include <dune/common/shared_ptr.hh>
#include <dune/common/indices.hh>
#include <dune/common/hybridutilities.hh>
#include <dune/typetree/nodeinterface.hh>
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

    // only bind to real rvalues
    template<typename T>
    typename std::enable_if<!std::is_lvalue_reference<T>::value,std::shared_ptr<T> >::type convert_arg(T&& t)
    {
      return std::make_shared<T>(std::forward<T>(t));
    }


    namespace Experimental {

      /**
       * @brief Applies left fold to a binary operator
       * @details End of recursion of the fold operator
       *
       * @param binary_op  Binary functor (discarded)
       * @param arg        Final result of the fold expansion
       * @return constexpr decltype(auto)  Final result of the fold expansion
       */
      template<class BinaryOp, class Arg>
      constexpr decltype(auto)
      left_fold(const BinaryOp& binary_op, Arg&& arg)
      {
        return std::forward<Arg>(arg);
      }

      /**
       * @brief Applies left fold to a binary operator
       * @details This function allows to pipe the result of one evaluation of a
       *          binary operator into the next evaluation.
       *
       * - C++17:   `(init op ... op pack);`
       * - here:    `left_fold(op, init, pack...);`
       *
       * @code {.c++}
       *   auto p = std::plus<>{};
       *   auto result_a = p(p(p(0,1),2),3);
       *   auto result_b = left_fold(p, 0, 1, 2, 3);
       * // result_a is same as result_b
       * @endcode
       *
       * @param binary_op  Binary functor
       * @param init       Initial (left-most) value of the fold
       * @param arg_0      First argument of the right side of the fold
       * @param args       Additional arguments of the right side of the fold
       * @return constexpr decltype(auto)  Final result of the fold expansion
       */
      template<class BinaryOp, class Init, class Arg0, class... Args>
      constexpr decltype(auto)
      left_fold(const BinaryOp& binary_op, Init&& init, Arg0&& arg_0, Args&&... args)
      {
        return left_fold(
          binary_op,
          binary_op(std::forward<Init>(init), std::forward<Arg0>(arg_0)),
          std::forward<Args>(args)...);
      }


      namespace Hybrid {
        using namespace Dune::Hybrid;

        namespace Detail {
          template<class Op, class... Args>
          constexpr auto applyOperator(Op&& op, Args&&... args)
          {
            using T = std::common_type_t<Args...>;
            return op(static_cast<T>(args)...);
          }

          template<class Op, class T, T... Args>
          constexpr auto applyOperator(Op, std::integral_constant<T,Args>...)
          {
            static_assert(std::is_default_constructible_v<Op>,
              "Operator in integral expressions shall be default constructible");
            constexpr auto result = Op{}(T{Args}...);
            return std::integral_constant<std::decay_t<decltype(result)>,result>{};
          }

          // FIXME: use lambda when we adpot c++20
          struct Max {
            template<class... Args>
            constexpr auto operator()(Args&&... args) const
            {
              using T = std::common_type_t<Args...>;
              return std::max({static_cast<T>(args)...});
            }
          };
        }

        static constexpr auto max = [](const auto& a, const auto& b)
        {
          return Detail::applyOperator(Detail::Max{}, a, b);
        };

        static constexpr auto plus = [](const auto& a, const auto& b)
        {
          return Detail::applyOperator(std::plus<>{}, a, b);
        };

        static constexpr auto minus = [](const auto& a, const auto& b)
        {
          return Detail::applyOperator(std::minus<>{}, a, b);
        };
      } // namespace Hybrid

    } // namespace Experimental


#endif // DOXYGEN

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
      typedef TreeInfo<Tree,NodeTag<Tree>> NodeInfo;

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

      typedef TreeInfo<typename Node::ChildType,NodeTag<typename Node::ChildType>> ChildInfo;

      static const std::size_t depth = 1 + ChildInfo::depth;

      static const std::size_t nodeCount = 1 + StaticDegree<Node>::value * ChildInfo::nodeCount;

      static const std::size_t leafCount = StaticDegree<Node>::value * ChildInfo::leafCount;

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

      typedef generic_compositenode_children_info<Node,0,StaticDegree<Node>::value> Children;

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


    using Dune::index_constant;
    namespace Indices = Dune::Indices;

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_UTILITY_HH
