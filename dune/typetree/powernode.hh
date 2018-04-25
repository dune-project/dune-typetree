// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_POWERNODE_HH
#define DUNE_TYPETREE_POWERNODE_HH

#include <cassert>
#include <array>

#include <dune/common/typetraits.hh>

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

#ifndef DOXYGEN

    namespace {

      // prototype and end of recursion
      template<typename T, typename It, typename... Args>
      void assign_reference_pack_to_shared_ptr_array_unpack(It it, Args&&... args) {}

      template<typename T, typename It, typename Arg, typename... Args>
      void assign_reference_pack_to_shared_ptr_array_unpack(It it, Arg&& arg, Args&&... args)
      {
        static_assert(std::is_same<T,typename std::remove_const<typename std::remove_reference<Arg>::type>::type>::value,"type mismatch during array conversion");
        *it = convert_arg(std::forward<Arg>(arg));
        assign_reference_pack_to_shared_ptr_array_unpack<T>(++it,std::forward<Args>(args)...);
      }

      template<typename T, std::size_t n, typename... Args>
      void assign_reference_pack_to_shared_ptr_array(std::array<shared_ptr<T>,n>& res, Args&&... args)
      {
        static_assert(sizeof...(Args) == n, "invalid number of arguments");
        return assign_reference_pack_to_shared_ptr_array_unpack<T>(res.begin(),std::forward<Args>(args)...);
      }


      // prototype and end of recursion
      template<typename T, typename It, typename... Args>
      void assign_shared_ptr_pack_to_shared_ptr_array_unpack(It it, Args&&... args) {}

      template<typename T, typename It, typename Arg, typename... Args>
      void assign_shared_ptr_pack_to_shared_ptr_array_unpack(It it, Arg&& arg, Args&&... args)
      {
        static_assert(std::is_same<T,typename std::remove_reference<Arg>::type::element_type>::value,"type mismatch during array conversion");
        *it = arg;
        assign_shared_ptr_pack_to_shared_ptr_array_unpack<T>(++it,args...);
      }

      template<typename T, std::size_t n, typename... Args>
      void assign_shared_ptr_pack_to_shared_ptr_array(std::array<shared_ptr<T>,n>& res, Args&&... args)
      {
        static_assert(sizeof...(Args) == n, "invalid number of arguments");
        return assign_shared_ptr_pack_to_shared_ptr_array_unpack<T>(res.begin(),args...);
      }

    } // anonymous namespace

#endif

#ifndef DOXYGEN

    //! Helper struct to make constructor enabling mechanism in PowerNode more readable.
    template<typename PowerNode, typename T, std::size_t k>
    struct AssertPowerNodeChildCount
      : public std::enable_if<std::is_same<
                           typename PowerNode::ChildType,
                           T>::value &&
    PowerNode::CHILDREN == k,
                         T>
    {};

#endif

    /** \brief Collect k instances of type T within a \ref TypeTree.
     *
     *  \tparam T The base type
     *  \tparam k The number of instances this node should collect
     */
    template<typename T, std::size_t k>
    class PowerNode
    {

    public:

      //! Mark this class as non leaf in the \ref TypeTree.
      static const bool isLeaf = false;

      //! Mark this class as a power in the \ref TypeTree.
      static const bool isPower = true;

      //! Mark this class as a non composite in the \ref TypeTree.
      static const bool isComposite = false;

      //! The number of children.
      static const std::size_t CHILDREN = k;

      static constexpr std::size_t degree()
      {
        return k;
      }

      //! The type tag that describes a PowerNode.
      typedef PowerNodeTag NodeTag;

      //! The type of each child.
      typedef T ChildType;

      //! The storage type of each child.
      typedef shared_ptr<T> ChildStorageType;

      //! The const version of the storage type of each child.
      typedef shared_ptr<const T> ChildConstStorageType;

      //! The type used for storing the children.
      typedef std::array<ChildStorageType,k> NodeStorage;


      //! Access to the type and storage type of the i-th child.
      template<std::size_t i>
      struct Child
      {

        static_assert((i < CHILDREN), "child index out of range");

        //! The type of the child.
        typedef T Type;

        //! The type of the child.
        typedef T type;

        //! The storage type of the child.
        typedef ChildStorageType Storage;

        //! The const storage type of the child.
        typedef ChildConstStorageType ConstStorage;
      };

      //! @name Child Access (templated methods)
      //! @{

      //! Returns the i-th child.
      /**
       * \returns a reference to the i-th child.
       */
      template<std::size_t i>
      T& child (index_constant<i> = {})
      {
        static_assert((i < CHILDREN), "child index out of range");
        return *_children[i];
      }

      //! Returns the i-th child (const version).
      /**
       * \returns a const reference to the i-th child.
       */
      template<std::size_t i>
      const T& child (index_constant<i> = {}) const
      {
        static_assert((i < CHILDREN), "child index out of range");
        return *_children[i];
      }

      //! Returns the storage of the i-th child.
      /**
       * \returns a copy of the object storing the i-th child.
       */
      template<std::size_t i>
      ChildStorageType childStorage(index_constant<i> = {})
      {
        static_assert((i < CHILDREN), "child index out of range");
        return _children[i];
      }

      //! Returns the storage of the i-th child (const version).
      /**
       * This method is only important if the child is stored as
       * some kind of pointer, as this allows the pointee type to
       * become const.
       * \returns a copy of the object storing the i-th child.
       */
      template<std::size_t i>
      ChildConstStorageType childStorage(index_constant<i> = {}) const
      {
        static_assert((i < CHILDREN), "child index out of range");
        return _children[i];
      }

      //! Sets the i-th child to the passed-in value.
      template<std::size_t i>
      void setChild (T& t, index_constant<i> = {})
      {
        static_assert((i < CHILDREN), "child index out of range");
        _children[i] = stackobject_to_shared_ptr(t);
      }

      //! Store the passed value in i-th child.
      template<std::size_t i>
      void setChild(T&& t, index_constant<i> = {})
      {
        static_assert((i < CHILDREN), "child index out of range");
        _children[i] = convert_arg(std::move(t));
      }

      //! Sets the stored value representing the i-th child to the passed-in value.
      template<std::size_t i>
      void setChild (ChildStorageType st, index_constant<i> = {})
      {
        static_assert((i < CHILDREN), "child index out of range");
        _children[i] = st;
      }

      //! @}


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

      //! @name Nested Child Access
      //! @{

      // The following two methods require a little bit of SFINAE trickery to work correctly:
      // We have to make sure that they don't shadow the methods for direct child access because
      // those get called by the generic child() machinery. If that machinery picks up the methods
      // defined below, we have an infinite recursion.
      // So the methods make sure that either
      //
      // * there are more than one argument. In that case, we got multiple indices and can forward
      //   to the general machine.
      //
      // * the first argument is not a valid flat index, i.e. either a std::size_t or an index_constant.
      //   The argument thus has to be some kind of TreePath instance that we can also pass to the
      //   generic machine.
      //
      // The above SFINAE logic works, but there is still a problem with the return type deduction.
      // We have to do a lazy lookup of the return type after SFINAE has succeeded, otherwise the return
      // type deduction will trigger the infinite recursion.

      //! Returns the child given by the list of indices.
      /**
       * This method simply forwards to the freestanding function child(). See that
       * function for further information.
       */
#ifdef DOXYGEN
      template<typename... Indices>
      ImplementationDefined& child(Indices... indices)
#else
      template<typename I0, typename... I,
        std::enable_if_t<(sizeof...(I) > 0) || IsTreePath<I0>::value, int > = 0>
      auto child(I0 i0, I... i)
#endif
      {
        static_assert(sizeof...(I) > 0 || impl::_non_empty_tree_path(I0{}),
          "You cannot use the member function child() with an empty TreePath, use the freestanding version child(node,treePath) instead."
          );
        return Dune::TypeTree::child(*this,i0,i...);
      }

      //! Returns the child given by the list of indices.
      /**
       * This method simply forwards to the freestanding function child(). See that
       * function for further information.
       */
#ifdef DOXYGEN
      template<typename... Indices>
      const ImplementationDefined& child(Indices... indices)
#else
      template<typename I0, typename... I,
        std::enable_if_t<(sizeof...(I) > 0) || IsTreePath<I0>::value, int > = 0>
      auto child(I0 i0, I... i) const
#endif
      {
        static_assert(sizeof...(I) > 0 || impl::_non_empty_tree_path(I0{}),
          "You cannot use the member function child() with an empty TreePath, use the freestanding version child(node,treePath) instead."
          );
        return Dune::TypeTree::child(*this,i0,i...);
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
      PowerNode()
      {}

      //! Initialize the PowerNode with a copy of the passed-in storage type.
      explicit PowerNode(const NodeStorage& children)
        : _children(children)
      {}

      //! Initialize all children with copies of a storage object constructed from the parameter \c t.
      explicit PowerNode (T& t, bool distinct_objects = true)
      {
        if (distinct_objects)
          {
            for (typename NodeStorage::iterator it = _children.begin(); it != _children.end(); ++it)
              *it = std::make_shared<T>(t);
          }
        else
          {
            shared_ptr<T> sp = stackobject_to_shared_ptr(t);
            std::fill(_children.begin(),_children.end(),sp);
          }
      }

#ifdef DOXYGEN

      //! Initialize all children with the passed-in objects.
      /**
       * The availability of this constructor depends on the number of children and
       * compiler support for C++0x: For 1 <= k <= 10, it is always present, but for
       * k > 10, it requires C++0x support in the compiler. If your compiler doesn't,
       * use PowerNode(const Storage& children) instead.
       *
       * Moreover, the C++0x-based version also supports passing in temporary objects
       * and will move those objects into the node. Attempting to do so with the legacy
       * version will result in a compile error.
       */
      PowerNode(T& t1, T& t2, ...)
      {}

#else

      // this weird signature avoids shadowing other 1-argument constructors
      template<typename C0, typename C1, typename... Children>
      PowerNode (C0&& c0, C1&& c1, Children&&... children)
      {
        assign_reference_pack_to_shared_ptr_array(_children,std::forward<C0>(c0),std::forward<C1>(c1),std::forward<Children>(children)...);
      }

      // this weird signature avoids shadowing other 1-argument constructors
      template<typename C0, typename C1, typename... Children>
      PowerNode (shared_ptr<C0> c0, shared_ptr<C1> c1, shared_ptr<Children>... children)
      {
        assign_shared_ptr_pack_to_shared_ptr_array(_children,c0,c1,children...);
      }

#endif // DOXYGEN

      //! @}

    private:
      NodeStorage _children;
    };

    //! \} group Nodes

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_POWERNODE_HH
