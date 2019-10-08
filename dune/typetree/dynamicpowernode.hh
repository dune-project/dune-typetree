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
#include <dune/typetree/childextraction.hh>
#include <dune/typetree/typetraits.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Nodes
     *  \ingroup TypeTree
     *  \{
     */

    /** \brief TODO
     *
     *  \tparam T The base type
     *  \tparam k The number of instances this node should collect
     */
    template<typename T>
    class DynamicPowerNode
    {

    public:

      //! Mark this class as non leaf in the \ref TypeTree.
      static const bool isLeaf = false;

      //! Mark this class as a power in the \ref TypeTree.
      static const bool isPower = true;

      //! Mark this class as a dynamic in the \ref TypeTree.
      static const bool isDynamic = true;

      //! Mark this class as a non composite in the \ref TypeTree.
      static const bool isComposite = false;

      //! The number of children.
      const std::size_t CHILDREN;

      std::size_t degree() const
      {
        return _children.size();
      }

      //! The type tag that describes a PowerNode.
      typedef DynamicPowerNodeTag NodeTag;

      //! The type of each child.
      typedef T ChildType;

      //! The storage type of each child.
      typedef shared_ptr<T> ChildStorageType;

      //! The const version of the storage type of each child.
      typedef shared_ptr<const T> ChildConstStorageType;

      //! The type used for storing the children.
      typedef std::vector<ChildStorageType> NodeStorage;


      //! @name Child Access (Dynamic methods)
      //! @{

      //! Returns the i-th child.
      /**
       * \returns a reference to the i-th child.
       */
      T& child (std::size_t i)
      {
        assert(i < CHILDREN && "child index out of range");
        return *_children[i];
      }

      //! Returns the i-th child (const version).
      /**
       * \returns a const reference to the i-th child.
       */
      const T& child (std::size_t i) const
      {
        assert(i < CHILDREN && "child index out of range");
        return *_children[i];
      }

      //! Returns the storage of the i-th child.
      /**
       * \returns a copy of the object storing the i-th child.
       */
      ChildStorageType childStorage(std::size_t i)
      {
        assert(i < CHILDREN && "child index out of range");
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
        assert(i < CHILDREN && "child index out of range");
        return (_children[i]);
      }

      //! Sets the i-th child to the passed-in value.
      void setChild (std::size_t i, T& t)
      {
        assert(i < CHILDREN && "child index out of range");
        _children[i] = stackobject_to_shared_ptr(t);
      }

      //! Store the passed value in i-th child.
      void setChild(std::size_t i, T&& t)
      {
        assert(i < CHILDREN && "child index out of range");
        _children[i] = convert_arg(std::move(t));
      }

      //! Sets the stored value representing the i-th child to the passed-in value.
      void setChild (std::size_t i, ChildStorageType st)
      {
        assert(i < CHILDREN && "child index out of range");
        _children[i] = st;
      }

      const NodeStorage& nodeStorage() const
      {
        return _children;
      }

      //! @}

      //! @name Constructors
      //! @{

    protected:

      //! Default constructor.
      /**
       * The default constructor is protected, as PowerNode is a utility
       * class that needs to be filled with meaning by subclassing it
       * and adding useful functionality to the subclass.
       *
       * \warning When using the default constructor, make sure to set ALL children
       * by means of the setChild() methods!
       */
      DynamicPowerNode()
      {}

      //! Initialize the DynamicPowerNode with a copy of the passed-in storage type.
      explicit DynamicPowerNode(const NodeStorage& children)
        : CHILDREN(children.size())
        , _children(children)
      {}

#ifdef DOXYGEN

      //! Initialize all children with the passed-in objects.
      DynamicPowerNode(T& t1, T& t2, ...)
      {}

#else

      template<typename... Children,
        std::enable_if_t<
          Dune::Std::conjunction<std::is_same<ChildType, std::decay_t<Children>>...>::value
          ,int> = 0>
      DynamicPowerNode (Children&&... children)
        : CHILDREN(sizeof...(Children))
      {
        assert(CHILDREN == sizeof...(Children) && "DynamicPowerNode constructor is called with incorrect number of children");
        _children = NodeStorage{convert_arg(std::forward<Children>(children))...};
      }

      template<typename... Children,
        std::enable_if_t<
          Dune::Std::conjunction<std::is_same<ChildType, Children>...>::value
          ,int> = 0>
      DynamicPowerNode (std::shared_ptr<Children>... children)
        : CHILDREN(sizeof...(Children))
      {
        assert(CHILDREN == sizeof...(Children) && "PowerNode constructor is called with incorrect number of children");
        _children = NodeStorage{children...};
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
