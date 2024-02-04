// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_PROXYNODE_HH
#define DUNE_TYPETREE_PROXYNODE_HH

#include <type_traits>
#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/common/shared_ptr.hh>
#include <dune/common/indices.hh>
#include <dune/common/std/type_traits.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Nodes
     *  \ingroup TypeTree
     *  \{
     */

    template<typename Node>
    class ProxyNode;

    //! Mixin class providing methods for child access with compile-time parameter.
    template<typename ProxiedNode>
    class StaticChildAccessors
    {

      static const bool proxiedNodeIsConst = std::is_const<typename std::remove_reference<ProxiedNode>::type>::value;

      template<std::size_t k>
      struct lazy_enabled
      {
        static const bool value = !proxiedNodeIsConst;
      };

      typedef ProxyNode<ProxiedNode> Node;

      template<bool enabled = !proxiedNodeIsConst>
      typename std::enable_if<enabled,Node&>::type
      node ()
      {
        return static_cast<Node&>(*this);
      }

      const Node& node () const
      {
        return static_cast<const Node&>(*this);
      }

    public:

      //! Access to the type and storage type of the i-th child.
      template<std::size_t k>
      struct Child
        : public ProxiedNode::template Child<k>
      {};

      //! @name Child Access
      //! @{

      //! Returns the i-th child.
      /**
       * \returns a reference to the i-th child.
       */
      template<std::size_t k,
        typename std::enable_if<lazy_enabled<k>::value, int>::type = 0>
      auto& child (index_constant<k> = {})
      {
        return node().proxiedNode().template child<k>();
      }

      //! Returns the i-th child (const version).
      /**
       * \returns a const reference to the i-th child.
       */
      template<std::size_t k>
      const auto& child (index_constant<k> = {}) const
      {
        return node().proxiedNode().template child<k>();
      }

      //! Returns the storage of the i-th child.
      /**
       * \returns a copy of the object storing the i-th child.
       */
      template<std::size_t k,
        typename std::enable_if<lazy_enabled<k>::value, int>::type = 0>
      auto childStorage (index_constant<k> = {})
      {
        return node().proxiedNode().template childStorage<k>();
      }

      //! Returns the storage of the i-th child (const version).
      /**
       * This method is only important if the child is stored as
       * some kind of pointer, as this allows the pointee type to
       * become const.
       * \returns a copy of the object storing the i-th child.
       */
      template<std::size_t k>
      auto childStorage (index_constant<k> = {}) const
      {
        return node().proxiedNode().template childStorage<k>();
      }

      //! Sets the i-th child to the passed-in value.
      template<std::size_t k, class ProxyChild>
      void setChild (ProxyChild&& child, typename std::enable_if<lazy_enabled<k>::value,void*>::type = 0)
      {
        node().proxiedNode().template setChild<k>(std::forward<ProxyChild>(child));
      }

      const typename ProxiedNode::NodeStorage& nodeStorage () const
      {
        return node().proxiedNode().nodeStorage();
      }

    };

    //! Mixin class providing methods for child access with run-time parameter.
    /**
     * This class also provides the compile-time parameter based methods, as
     * multiple inheritance from both DynamicChildAccessors and StaticChildAccessors
     * creates ambigous method lookups.
     */
    template<typename ProxiedNode>
    class DynamicChildAccessors
      : public StaticChildAccessors<ProxiedNode>
    {

      typedef ProxyNode<ProxiedNode> Node;

      static const bool proxiedNodeIsConst = std::is_const<typename std::remove_reference<ProxiedNode>::type>::value;

      template<bool enabled = !proxiedNodeIsConst>
      typename std::enable_if<enabled,Node&>::type
      node ()
      {
        return static_cast<Node&>(*this);
      }

      const Node& node () const
      {
        return static_cast<const Node&>(*this);
      }

    public:

      //! @name Child Access (Dynamic methods)
      //! @{

      //! Returns the i-th child.
      /**
       * \returns a reference to the i-th child.
       */
      template<bool enabled = !proxiedNodeIsConst,
        typename std::enable_if<enabled, int>::type = 0>
      auto& child (std::size_t i)
      {
        return node().proxiedNode().child(i);
      }

      //! Returns the i-th child (const version).
      /**
       * \returns a const reference to the i-th child.
       */
      const auto& child (std::size_t i) const
      {
        return node().proxiedNode().child(i);
      }

      //! Returns the storage of the i-th child.
      /**
       * \returns a copy of the object storing the i-th child.
       */
      template<bool enabled = !proxiedNodeIsConst,
        typename std::enable_if<enabled, int>::type = 0>
      auto childStorage (std::size_t i)
      {
        return node().proxiedNode().childStorage(i);
      }

      //! Returns the storage of the i-th child (const version).
      /**
       * This method is only important if the child is stored as
       * some kind of pointer, as this allows the pointee type to
       * become const.
       * \returns a copy of the object storing the i-th child.
       */
      auto childStorage (std::size_t i) const
      {
        return node().proxiedNode().childStorage(i);
      }

      //! Sets the i-th child to the passed-in value.
      template<class ProxyChild, bool enabled = !proxiedNodeIsConst>
      void setChild (std::size_t i, ProxyChild&& child, typename std::enable_if<enabled,void*>::type = 0)
      {
        node().proxiedNode().setChild(i, std::forward<ProxyChild>(child));
      }

    };

    //! Tag-based dispatch to appropriate base class that provides necessary functionality.
    template<typename Node, typename NodeTag>
    struct ProxyNodeBase;

    //! ProxyNode base class for LeafNode.
    template<typename Node>
    struct ProxyNodeBase<Node,LeafNodeTag>
    {
    };

    //! ProxyNode base class for CompositeNode.
    template<typename Node>
    struct ProxyNodeBase<Node,CompositeNodeTag>
      : public StaticChildAccessors<Node>
    {
      typedef typename Node::ChildTypes ChildTypes;
      typedef typename Node::NodeStorage NodeStorage;
    };

    //! ProxyNode base class for PowerNode.
    template<typename Node>
    struct ProxyNodeBase<Node,PowerNodeTag>
      : public DynamicChildAccessors<Node>
    {
      typedef typename Node::ChildType ChildType;
      typedef typename Node::NodeStorage NodeStorage;
    };

    //! ProxyNode base class for DynamicPowerNode.
    template<typename Node>
    struct ProxyNodeBase<Node,DynamicPowerNodeTag>
      : public DynamicChildAccessors<Node>
    {
      typedef typename Node::ChildType ChildType;
      typedef typename Node::NodeStorage NodeStorage;
    };

    //! Base class for nodes acting as a proxy for an existing node.
    /**
     * ProxyNode is a utility class for implementing proxy classes
     * that need to provide the TypeTree node functionality of the
     * proxied class. It exactly mirrors the TypeTree node characteristics
     * of the proxied node.
     */
    template<typename Node>
    class ProxyNode
      : public ProxyNodeBase<Node,NodeTag<Node>>
    {
      static const bool proxiedNodeIsConst = std::is_const<typename std::remove_reference<Node>::type>::value;

      template <class N>
      using HasStaticDegree = index_constant<N::degree()>;

      template <class N>
      static constexpr bool hasStaticDegree = Std::is_detected<HasStaticDegree, N>::value;

      // accessor mixins need to be friends for access to proxiedNode()
      friend class StaticChildAccessors<Node>;
      friend class DynamicChildAccessors<Node>;

    public:

      typedef Node ProxiedNode;

      typedef Dune::TypeTree::NodeTag<Node> NodeTag;

      //! Mark this class as non leaf in the \ref TypeTree.
      static const bool isLeaf = Node::isLeaf;

      //! Mark this class as a non power in the \ref TypeTree.
      static const bool isPower = Node::isPower;

      //! Mark this class as a composite in the \ref TypeTree.
      static const bool isComposite = Node::isComposite;

      template <class N = Node,
        std::enable_if_t<hasStaticDegree<N>, int> = 0>
      static constexpr auto degree ()
      {
        return N::degree();
      }

      template <class N = Node,
        std::enable_if_t<not hasStaticDegree<N>, int> = 0>
      auto degree () const
      {
        return proxiedNode().degree();
      }


    protected:

      //! @name Access to the proxied node
      //! @{

      //! Returns the proxied node.
      template<bool enabled = !proxiedNodeIsConst>
      typename std::enable_if<enabled,Node&>::type
      proxiedNode ()
      {
        return *_node;
      }

      //! Returns the proxied node (const version).
      const Node& proxiedNode () const
      {
        return *_node;
      }

      //! Returns the storage of the proxied node.
      template<bool enabled = !proxiedNodeIsConst>
      typename std::enable_if<enabled,std::shared_ptr<Node> >::type
      proxiedNodeStorage ()
      {
        return _node;
      }

      //! Returns the storage of the proxied node (const version).
      std::shared_ptr<const Node> proxiedNodeStorage () const
      {
        return _node;
      }

      //! @}

      //! @name Constructors
      //! @{

      ProxyNode (Node& node)
        : _node(stackobject_to_shared_ptr(node))
      {}

      ProxyNode (std::shared_ptr<Node> node)
        : _node(std::move(node))
      {}

      //! @}

    private:

      std::shared_ptr<Node> _node;
    };

    //! \} group Nodes

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_PROXYNODE_HH
