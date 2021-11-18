// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TREECONTAINER_HH
#define DUNE_TYPETREE_TREECONTAINER_HH

#include <type_traits>
#include <utility>
#include <functional>
#include <array>

#include <dune/common/indices.hh>
#include <dune/common/hybridutilities.hh>
#include <dune/common/rangeutilities.hh>
#include <dune/common/tuplevector.hh>

#include <dune/typetree/treepath.hh>

namespace Dune {
  namespace TypeTree {

    namespace Detail {

      /*
       * \brief A factory class creating a hybrid container compatible with a type tree
       *
       * This class allows to create a nested hybrid container having the same structure
       * as a given type tree. Power nodes are represented as std::array's while composite
       * nodes are represented as Dune::TupleVector's. The stored values for the leaf nodes
       * are creating using a given predicate. Once created, the factory provides an
       * operator() creating the container for the tree given as argument.
       *
       * \tparam LeafToValue Type of a predicate that determines the stored values at the leafs
       */
      template<class LeafToValue>
      class ContainerFactory
      {
        template<class N>
        using DynamicDegreeConcept = decltype((std::size_t(std::declval<N>().degree()), true));

        template<class N>
        using StaticDegreeConcept = decltype((std::integral_constant<std::size_t, N::degree()>{}, true));

        template<class N>
        using DynamicChildAccessConcept = decltype((std::declval<N>().child(0u), true));

      public:

        /**
         * \brief Create ContainerFactory
         *
         * The given predicate will be stored by value.
         *
         * \param A predicate used to generate the stored values for the leaves
         */
        ContainerFactory(LeafToValue leafToValue) :
          leafToValue_(leafToValue)
        {}

        template<class Node>
        auto operator()(const Node& node)
        {
          return (*this)(node, Dune::PriorityTag<5>{});
        }

      private:

        template<class Node,
          std::enable_if_t<Node::isLeaf, bool> = true>
        auto operator()(const Node& node, Dune::PriorityTag<4>)
        {
          return leafToValue_(node);
        }

        template<class Node,
          StaticDegreeConcept<Node> = true,
          DynamicChildAccessConcept<Node> = true>
        auto operator()(const Node& node, Dune::PriorityTag<3>)
        {
          return Dune::unpackIntegerSequence([&](auto... indices) {
              return std::array{(*this)(node.child(indices))...};
            }, std::make_index_sequence<std::size_t(Node::degree())>());
        }

        template<class Node,
          DynamicDegreeConcept<Node> = true,
          DynamicChildAccessConcept<Node> = true>
        auto operator()(const Node& node, Dune::PriorityTag<2>)
        {
          using TransformedChild = decltype((*this)(node.child(0)));
          std::vector<TransformedChild> container;
          container.reserve(node.degree());
          for (std::size_t i = 0; i < node.degree(); ++i)
            container.emplace_back((*this)(node.child(i)));
          return container;
        }

        template<class Node,
          StaticDegreeConcept<Node> = true>
        auto operator()(const Node& node, Dune::PriorityTag<1>)
        {
          return Dune::unpackIntegerSequence([&](auto... indices) {
              return Dune::makeTupleVector((*this)(node.child(indices))...);
            }, std::make_index_sequence<std::size_t(Node::degree())>());
        }

      private:
        LeafToValue leafToValue_;
      };


      /*
       * \brief Wrap nested container to provide a VectorBackend
       */
      template<class Container>
      class TreeContainerVectorBackend
      {
        template<class C>
        static constexpr decltype(auto) accessByTreePath(C&& container, const HybridTreePath<>& path)
        {
          return container;
        }

        template<class C, class... T>
        static constexpr decltype(auto) accessByTreePath(C&& container, const HybridTreePath<T...>& path)
        {
          auto head = path[Dune::Indices::_0];
          auto tailPath = Dune::unpackIntegerSequence([&](auto... i){
                        return treePath(path[Dune::index_constant<i+1>{}]...);
                      }, std::make_index_sequence<sizeof...(T)-1>());
          return accessByTreePath(container[head], tailPath);
        }

        template<class C, class Tree,
          std::enable_if_t<Tree::isLeaf, bool> = true>
        static void resizeImpl(C& /*container*/, const Tree& /*tree*/, Dune::PriorityTag<2>)
        {
          /* do nothing */
        }

        template<class C, class Tree,
          class = decltype(std::declval<C>().resize(0u))>
        static void resizeImpl(C& container, const Tree& tree, Dune::PriorityTag<1>)
        {
          container.resize(tree.degree());
          Dune::Hybrid::forEach(Dune::range(tree.degree()), [&](auto i) {
            resizeImpl(container[i], tree.child(i), Dune::PriorityTag<5>{});
          });
        }

        template<class C, class Tree>
        static void resizeImpl(C& container, const Tree& tree, Dune::PriorityTag<0>)
        {
          Dune::Hybrid::forEach(Dune::range(tree.degree()), [&](auto i) {
            resizeImpl(container[i], tree.child(i), Dune::PriorityTag<5>{});
          });
        }

        template<class T>
        using TypeTreeConcept = decltype((
          std::declval<T>().degree(),
          T::isLeaf,
          T::isPower,
          T::isComposite,
        true));

      public:
        //! Move the passed container into the internal storage
        TreeContainerVectorBackend(Container&& container) :
          container_(std::move(container))
        {}

        //! Default construct the container and perform a resize depending on the tree-node degrees.
        template <class Tree, TypeTreeConcept<Tree> = true>
        TreeContainerVectorBackend(const Tree& tree) :
          TreeContainerVectorBackend()
        {
          this->resize(tree);
        }

        //! Default constructor. The stored container might need to be resized before usage.
        template <class C = Container,
          std::enable_if_t<std::is_default_constructible_v<C>, bool> = true>
        TreeContainerVectorBackend() :
          container_()
        {}

        template<class... T>
        decltype(auto) operator[](const HybridTreePath<T...>&  path) const
        {
          return accessByTreePath(container_, path);
        }

        template<class... T>
        decltype(auto) operator[](const HybridTreePath<T...>&  path)
        {
          return accessByTreePath(container_, path);
        }

        //! Resize the (nested) container depending on the degree of the tree nodes
        template<class Tree, TypeTreeConcept<Tree> = true>
        void resize(const Tree& tree)
        {
          resizeImpl(container_, tree, Dune::PriorityTag<5>{});
        }

        const Container& data() const
        {
          return container_;
        }

        Container& data()
        {
          return container_;
        }

      private:
        Container container_;
      };

      template<class Container>
      auto makeTreeContainerVectorBackend(Container&& container)
      {
        return TreeContainerVectorBackend<std::decay_t<Container>>(std::forward<Container>(container));
      }

      /*
       * \brief A simple lambda for creating default constructible values from a node
       *
       * This simply returns LeafToValue<Node>{} for a given Node. It's needed
       * because using a lambda expression in a using declaration is not allowed
       * because it's an unevaluated context.
       */
      template<template<class Node> class LeafToValue>
      struct LeafToDefaultConstructibleValue
      {
        template<class Node>
        auto operator()(const Node& node) const
        {
          return LeafToValue<Node>{};
        }
      };

    } // namespace Detail

    /** \addtogroup TypeTree
     *  \{
     */

    /**
     * \brief Create container havin the same structure as the given tree
     *
     * This class allows to create a nested hybrid container having the same structure
     * as a given type tree. Power nodes are represented as std::array's while composite
     * nodes are represented as Dune::TupleVector's. The stored values for the leaf nodes
     * are creating using a given predicate. For convenience the created container is
     * not returned directly. Instead, the returned object stores the container and
     * provides operator[] access using a HybridTreePath.
     *
     * \param tree The tree which should be mapper to a container
     * \param leafToValue A predicate used to generate the stored values for the leaves
     *
     * \returns A container matching the tree structure
     */
    template<class Tree, class LeafToValue>
    auto makeTreeContainer(const Tree& tree, LeafToValue&& leafToValue)
    {
      auto f = std::ref(leafToValue);
      auto factory = Detail::ContainerFactory<decltype(f)>(f);
      return Detail::makeTreeContainerVectorBackend(factory(tree));
    }

    /**
     * \brief Create container havin the same structure as the given tree
     *
     * This class allows to create a nested hybrid container having the same structure
     * as a given type tree. Power nodes are represented as std::array's while composite
     * nodes are represented as Dune::TupleVector's. The stored values for the leaf nodes
     * are of the given type Value. For convenience the created container is
     * not returned directly. Instead, the returned object stores the container and
     * provides operator[] access using a HybridTreePath.
     *
     * \tparam Value Type of the values to be stored for the leafs. Should be default constructible.
     * \param leafToValue A predicate used to generate the stored values for the leaves
     *
     * \returns A container matching the tree structure
     */
    template<class Value, class Tree>
    auto makeTreeContainer(const Tree& tree)
    {
      return makeTreeContainer(tree, [](const auto&) {return Value{};});
    }

    /**
     * \brief Alias to container type generated by makeTreeContainer for given tree type and uniform value type
     */
    template<class Value, class Tree>
    using UniformTreeContainer = std::decay_t<decltype(makeTreeContainer<Value>(std::declval<const Tree&>()))>;

    /**
     * \brief Alias to container type generated by makeTreeContainer for give tree type and when using LeafToValue to create values
     */
    template<template<class Node> class LeafToValue, class Tree>
    using TreeContainer = std::decay_t<decltype(makeTreeContainer(std::declval<const Tree&>(), std::declval<Detail::LeafToDefaultConstructibleValue<LeafToValue>>()))>;

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TREECONTAINER_HH
