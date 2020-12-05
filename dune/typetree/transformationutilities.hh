// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TRANSFORMATIONUTILITIES_HH
#define DUNE_TYPETREE_TRANSFORMATIONUTILITIES_HH

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>
#include <memory>

#include <dune/common/indices.hh>
#include <dune/common/std/apply.hh>

#include <dune/typetree/simpletransformationdescriptors.hh>
#include <dune/typetree/generictransformationdescriptors.hh>


namespace Dune {
  namespace TypeTree {

    /**
     * \addtogroup Transformation
     * \ingroup TypeTree
     * \{
     */



    /**
     * \brief Use given transformation to transform children of PowerNode
     *
     * This requires that the passed node provides dynamic child access
     * using a child(std::size_t) method. The transformation is applied
     * to each child and the result is returned as std::array of
     * std::shared_ptr's to the transformed values.
     * Notice that this loops dynamically over the children.
     *
     * \param node The PowerNode whose children should be transformed
     * \param transformation A predicate to be used for transforming the children
     * \returns An std::array of stdd::shared_ptr's to the transformed children
     */
    template<class Node, class Transformation>
    auto transformPowerNodeChildren(const Node& node, Transformation&& transformation)
    {
      using TransformedChild = decltype(transformation(node.child(0)));

      std::array<std::shared_ptr<TransformedChild>, Node::degree()> transformedChildren;
      for(auto i : Dune::range(Node::degree()))
        transformedChildren[i] = std::make_shared<TransformedChild>(transformation(node.child(i)));
      return transformedChildren;
    }



    /**
     * \brief Use given transformation to transform children of CompositeNode
     *
     * This requires that the passed node provides static child access
     * using child(Dune::index_constant<i>) methods. The transformation is applied
     * to each child and the result is returned as std::tuple of
     * std::shared_ptr's to the transformed values.
     * Notice that this loops statically over the children.
     *
     * \param node The CompositeNode whose children should be transformed
     * \param transformation A predicate to be used for transforming the children
     * \returns An std::tuple of stdd::shared_ptr's to the transformed children
     */
    template<class Node, class Transformation>
    auto transformCompositeNodeChildren(const Node& node, Transformation&& transformation)
    {
      return Dune::unpackIntegerSequence([&](auto... indices) {
          return std::make_tuple(Dune::wrap_or_move(transformation(node.child(indices)))...);
        }, std::make_index_sequence<Node::degree()>());
    }



    /**
     * \brief A generic function for transforming PowerNodes using a given child transformation
     *
     * This requires that the transformed node type has a special template signature
     * and provides a special constructor syntax. Both are compatible with the requirements
     * of the GenericPowerNodeTransformation. That is, the template parameters are the
     * untransformed power node type, the transformed child node type, and the child count.
     * The constructor is called with the the untransformed node, the transformation object,
     * and an std::array of std::shared_ptr's to the transformed children. The children
     * are transformed using the given transformation predicate.
     *
     * \param node The PowerNode to be transformed
     * \param transformation A predicate to be used for transforming the children
     * \returns The transformed node
     */
    template<template<class,class,std::size_t> class TransformedNode, class Node, class Transformation>
    auto genericPowerNodeTransformation(const Node& node, Transformation&& transformation)
    {
      using TransformedChild = decltype(transformation(node.child(0)));
      using TransformedNodeRealization = TransformedNode<Node, TransformedChild, Node::degree()>;
        return TransformedNodeRealization(node, transformation, transformPowerNodeChildren(node, transformation));
    }



    /**
     * \brief A generic function for transforming CompositeNodes using a given child transformation
     *
     * This requires that the transformed node type has a special template signature
     * and provides a special constructor syntax. Both are compatible with the requirements
     * of the GenericCompositeNodeTransformation. That is, the template parameters are the
     * untransformed power node type, the types of the transformed child nodes.
     * The constructor is called with the the untransformed node, the transformation object,
     * and std::shared_ptr's to the transformed children. The children
     * are transformed using the given transformation predicate.
     *
     * \param node The CompositeNode to be transformed
     * \param transformation A predicate to be used for transforming the children
     * \returns The transformed node
     */
    template<template<class,class...> class TransformedNode, class Node, class Transformation>
    auto genericCompositeNodeTransformation(const Node& node, Transformation&& transformation)
    {
      return Dune::Std::apply([&](auto... transformedChildren) {
          using TransformedNodeRealization = TransformedNode<Node, typename decltype(transformedChildren)::element_type...>;
          return TransformedNodeRealization(node, transformation, transformedChildren...);
        }, transformCompositeNodeChildren(node, transformation));
    }



    /**
     * \} group Traversal
     */

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TRANSFORMATIONUTILITIES_HH
