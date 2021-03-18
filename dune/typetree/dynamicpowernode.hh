// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_DYNAMICPOWERNODE_HH
#define DUNE_TYPETREE_DYNAMICPOWERNODE_HH

#include <cassert>
#include <vector>
#include <memory>
#include <type_traits>

#include <dune/common/typetraits.hh>
#include <dune/common/std/type_traits.hh>

#include <dune/typetree/nodetags.hh>
#include <dune/typetree/utility.hh>
#include <dune/typetree/typetraits.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Nodes
     *  \ingroup TypeTree
     *  \{
     */

    /** \brief Collect multiple instances of type T within a \ref TypeTree.
     *
     *  \tparam T  Type of the tree-node children
     */
    template<typename T>
    class DynamicPowerNode
    {

    public:

      //! Mark this class as non leaf in the \ref TypeTree.
      static const bool isLeaf = false;

      //! Mark this class as a power in the \ref TypeTree.
      static const bool isPower = true;

      //! Mark this class as a non composite in the \ref TypeTree.
      static const bool isComposite = false;

      //! The number of children.
      std::size_t degree() const
      {
        return _children.size();
      }

      //! The type tag that describes the node.
      typedef DynamicPowerNodeTag NodeTag;

      //! The type of each child.
      typedef T ChildType;

      //! The storage type of each child.
      typedef std::shared_ptr<T> ChildStorageType;

      //! The const version of the storage type of each child.
      typedef std::shared_ptr<const T> ChildConstStorageType;

      //! The type used for storing the children.
      typedef std::vector<ChildStorageType> NodeStorage;


      //! @name Child Access (Dynamic methods)
      //! @{

      //! Returns the i-th child.
      /**
       * \returns a reference to the i-th child.
       */
      ChildType& child (std::size_t i)
      {
        assert(i < degree() && "child index out of range");
        return *_children[i];
      }

      //! Returns the i-th child (const version).
      /**
       * \returns a const reference to the i-th child.
       */
      const ChildType& child (std::size_t i) const
      {
        assert(i < degree() && "child index out of range");
        return *_children[i];
      }

      //! Returns the storage of the i-th child.
      /**
       * \returns a copy of the object storing the i-th child.
       */
      ChildStorageType childStorage (std::size_t i)
      {
        assert(i < degree() && "child index out of range");
        return _children[i];
      }

      //! Returns the storage of the i-th child (const version).
      /**
       * This method is only important if the child is stored as
       * some kind of pointer, as this allows the pointee type to
       * become const.
       * \returns a copy of the object storing the i-th child.
       */
      ChildConstStorageType childStorage (std::size_t i) const
      {
        assert(i < degree() && "child index out of range");
        return _children[i];
      }

      //! Sets the i-th child to the passed-in value.
      void setChild (std::size_t i, ChildType& t)
      {
        assert(i < degree() && "child index out of range");
        _children[i] = stackobject_to_shared_ptr(t);
      }

      //! Store the passed value in i-th child.
      void setChild (std::size_t i, ChildType&& t)
      {
        assert(i < degree() && "child index out of range");
        _children[i] = convert_arg(std::move(t));
      }

      //! Sets the stored value representing the i-th child to the passed-in value.
      void setChild (std::size_t i, ChildStorageType st)
      {
        assert(i < degree() && "child index out of range");
        _children[i] = std::move(st);
      }

      const NodeStorage& nodeStorage () const
      {
        return _children;
      }

      //! @}

      //! @name Constructors
      //! @{

    protected:

      //! The Default constructor is deleted, since you need to pass the number of children. There
      //! is currently no dynamic resize of this node type implemented.
      DynamicPowerNode () = delete;

      //! Construct a node with the given number of children.
      /**
       * The constructor is protected, as DynamicPowerNode is a utility
       * class that needs to be filled with meaning by subclassing it
       * and adding useful functionality to the subclass.
       *
       * \warning When using this constructor, make sure to set ALL children
       * by means of the setChild() methods!
       */
      explicit DynamicPowerNode (std::size_t size)
        : _children(size)
      {}

      //! Initialize the DynamicPowerNode with a copy of the passed-in storage type.
      explicit DynamicPowerNode (NodeStorage children)
        : _children(std::move(children))
      {}

#ifdef DOXYGEN

      //! Initialize all children with the passed-in objects.
      DynamicPowerNode (T& t1, T& t2, ...)
      {}

#else

      template<typename... Children,
        std::enable_if_t<(std::is_same_v<ChildType, std::decay_t<Children>> &&...), bool> = true>
      DynamicPowerNode (Children&&... children)
      {
        _children = NodeStorage{convert_arg(std::forward<Children>(children))...};
      }

      template<typename... Children,
        std::enable_if_t<(std::is_same_v<ChildType, std::decay_t<Children>> &&...), bool> = true>
      DynamicPowerNode (std::shared_ptr<Children>... children)
      {
        _children = NodeStorage{std::move(children)...};
      }

#endif // DOXYGEN

      //! @}

    private:
      NodeStorage _children;
    };

    //! \} group Nodes

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_DYNAMICPOWERNODE_HH
