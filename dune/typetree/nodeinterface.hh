// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_NODEINTERFACE_HH
#define DUNE_TYPETREE_NODEINTERFACE_HH

#include <cstddef>
#include <type_traits>

#include <dune/common/documentation.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Nodes
     *  \ingroup TypeTree
     *  \{
     */

    /** \brief Interface for nodes in a \ref TypeTree.
     *
     * This class cannot be used itself, it is for documentation purposes
     * only.
     *
     * \note Constructor signatures are explicitly not specified by this
     *       interface.
     * \note In addition, every node in a tree must be derived from one of
     *       the node base classes LeafNode, PowerNode, DynamicPowerNodeTag, or
     *       CompositeNode, or from a base class for a yet-to-be-defined new
     *       node type.
     */
    struct NodeInterface
    {
      //! Whether this is a leaf node in a \ref TypeTree.
      static const bool isLeaf = implementationDefined;

      //! Whether this is a power node in the \ref TypeTree.
      static const bool isPower = implementationDefined;

      //! Whether this is a composite node in the \ref TypeTree.
      static const bool isComposite = implementationDefined;

      //! Number of children of this node in the \ref TypeTree.
      /**
       * \note Static as long as the the information is known at compilation
       * time, otherwise non-static.
       * \return Dune::index_constant<k> if static, std::size_t otherwise
       */
      static auto degree();

      //! The type tag that describes what kind of node this is
      /**
       * One of LeafNodeTag, PowerNodeTag or CompositeNodeTag.
       * Other tags are also possible when new
       * kinds of nodes are defined.
       */
      typedef ImplementationDefined NodeTag;

      //! container type to pass around a collection of children
      /**
       * \note This typedef is not present for leaf nodes.
       */
      typedef ImplementationDefined NodeStorage;
    };

    //! Returns the node tag of the given Node.
    template<typename Node>
    using NodeTag = typename std::decay_t<Node>::NodeTag;

    //! Returns the implementation tag of the given Node.
    template<typename T>
    using ImplementationTag = typename std::decay_t<T>::ImplementationTag;


    //! Returns the degree of node as run time information.
    template<typename Node>
    std::size_t degree(const Node& node)
    {
      return degree(&node,NodeTag<Node>());
    }

#ifndef DOXYGEN

    //! Default implementation of degree dispatch function.
    /**
     * This dispatches using a pointer to the node instead of a reference,
     * as we can easily create a constexpr pointer to the node, while a constexpr
     * reference might not even be possible to manufacture (std::declval is not
     * constexpr).
     */
    template<typename Node, typename NodeTag>
    std::size_t degree(const Node* node, NodeTag)
    {
      return node->degree();
    }

#endif // DOXYGEN

    //! Returns the statically known degree of the given Node type as a std::integral_constant.
    /**
     * \note If a node has a static number of children, it returns directly the degree as
     * integral-constant.
     */
    template<typename Node>
    using StaticDegree = decltype(Node::degree());

    //! \} group Nodes

  } // namespace TypeTree
} //namespace Dune

#endif //  DUNE_TYPETREE_NODEINTERFACE_HH
