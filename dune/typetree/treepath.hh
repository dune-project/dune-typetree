// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TREEPATH_HH
#define DUNE_TYPETREE_TREEPATH_HH

#include <cstddef>

#include <dune/common/documentation.hh>
#include <dune/common/typetraits.hh>

#include <dune/typetree/fixedcapacitystack.hh>
#include <dune/typetree/utility.hh>


namespace Dune {
  namespace TypeTree {


    //! \addtogroup TreePath
    //! \ingroup TypeTree
    //! \{

    namespace TreePathType {
      enum Type { fullyStatic, mostlyStatic, dynamic };
    }

    template<std::size_t... i>
    struct TreePath {
      typedef TreePath ViewType;
      TreePath view() { return *this; }
      TreePath mutablePath() { return *this; }
    };

    template<typename>
    struct TreePathSize;

    template<std::size_t... i>
    struct TreePathSize<TreePath<i...> >
      : public index_constant<sizeof...(i)>
    {};

    template<typename,std::size_t>
    struct TreePathPushBack;

    template<std::size_t k, std::size_t... i>
    struct TreePathPushBack<TreePath<i...>,k>
    {
      typedef TreePath<i...,k> type;
    };

    template<typename,std::size_t>
    struct TreePathPushFront;

    template<std::size_t k, std::size_t... i>
    struct TreePathPushFront<TreePath<i...>,k>
    {
      typedef TreePath<k,i...> type;
    };

    template<typename>
    struct TreePathBack;

    // There is only a single element, so return that...
    template<std::size_t k>
    struct TreePathBack<TreePath<k> >
      : public index_constant<k>
    {};

    // We need to explicitly provide two elements here, as
    // the template argument pack would match the empty list
    // and create a conflict with the single-element specialization.
    // Here, we just shave off the first element and recursively
    // instantiate ourselves.
    template<std::size_t j, std::size_t k, std::size_t... l>
    struct TreePathBack<TreePath<j,k,l...> >
      : public TreePathBack<TreePath<k,l...> >
    {};

    template<typename>
    struct TreePathFront;

    template<std::size_t k, std::size_t... i>
    struct TreePathFront<TreePath<k,i...> >
      : public index_constant<k>
    {};

    template<typename, std::size_t...>
    struct TreePathPopBack;

    template<std::size_t k, std::size_t... i>
    struct TreePathPopBack<TreePath<k>,i...>
    {
      typedef TreePath<i...> type;
    };

    template<std::size_t j,
             std::size_t k,
             std::size_t... l,
             std::size_t... i>
    struct TreePathPopBack<TreePath<j,k,l...>,i...>
      : public TreePathPopBack<TreePath<k,l...>,i...,j>
    {};

    template<typename>
    struct TreePathPopFront;

    template<std::size_t k, std::size_t... i>
    struct TreePathPopFront<TreePath<k,i...> >
    {
      typedef TreePath<i...> type;
    };

    template<typename, typename>
    struct TreePathConcat;

    template<std::size_t... i, std::size_t... k>
    struct TreePathConcat<TreePath<i...>,TreePath<k...> >
    {
      typedef TreePath<i...,k...> type;
    };

    template<std::size_t... i>
    void print_tree_path(std::ostream& os)
    {}

    template<std::size_t k, std::size_t... i>
    void print_tree_path(std::ostream& os)
    {
      os << k << " ";
      print_tree_path<i...>(os);
    }

    template<std::size_t... i>
    std::ostream& operator<<(std::ostream& os, const TreePath<i...>& tp)
    {
      os << "TreePath< ";
      print_tree_path<i...>(os);
      os << ">";
      return os;
    }

    //! A TreePath that stores the path of a node as runtime information.
    class DynamicTreePath
    {

    public:

      //! Get the size (length) of this path.
      std::size_t size() const
      {
        return _stack.size();
      }

      //! Get the index value at position pos.
      std::size_t element(std::size_t pos) const
      {
        return _stack[pos];
      }

      //! Get the last index value.
      std::size_t back() const
      {
        return _stack.back();
      }

      //! Get the first index value.
      std::size_t front() const
      {
        return _stack.front();
      }

      friend std::ostream& operator<<(std::ostream& os, const DynamicTreePath& tp)
      {
        os << "TreePath( ";
        for (std::size_t i = 0; i < tp.size(); ++i)
          os << tp.element(i) << " ";
        os << ")";
        return os;
      }

    protected:

#ifndef DOXYGEN

      typedef FixedCapacityStackView<std::size_t> Stack;

      Stack& _stack;

      DynamicTreePath(Stack& stack)
        : _stack(stack)
      {}

#endif // DOXYGEN

    };

#ifndef DOXYGEN // DynamicTreePath subclasses are implementation details and never exposed to the user

    // This is the object that gets passed around by the traversal algorithm. It
    // extends the DynamicTreePath with stack-like modifier methods. Note that
    // it does not yet allocate any storage for the index values. It just uses
    // the reference to a storage vector of the base class. This implies that all
    // objects that are copy-constructed from each other share a single index storage!
    // The reason for this is to avoid differentiating the visitor signature for static
    // and dynamic traversal: Having very cheap copy-construction for these objects
    // allows us to pass them by value.
    class MutableDynamicTreePath
      : public DynamicTreePath
    {

    public:

      typedef DynamicTreePath ViewType;

      void push_back(std::size_t v)
      {
        _stack.push_back(v);
      }

      void pop_back()
      {
        _stack.pop_back();
      }

      void set_back(std::size_t v)
      {
        _stack.back() = v;
      }

      DynamicTreePath view()
      {
        return *this;
      }

    protected:

      MutableDynamicTreePath(Stack& stack)
        : DynamicTreePath(stack)
      {}

    };

    // DynamicTreePath storage provider.
    // This objects provides the storage for the DynamicTreePath
    // during the tree traversal. After construction, it should
    // not be used directly - the traversal framework uses the
    // base class returned by calling mutablePath().
    template<std::size_t treeDepth>
    class MakeableDynamicTreePath
      : private FixedCapacityStack<std::size_t,treeDepth>
      , public MutableDynamicTreePath
    {

    public:

      MutableDynamicTreePath mutablePath()
      {
        return static_cast<MutableDynamicTreePath&>(*this);
      }

      MakeableDynamicTreePath()
        : MutableDynamicTreePath(static_cast<FixedCapacityStackView<std::size_t>&>(*this))
      {
      }

    };

    // Factory for creating the right type of TreePath based on the requested
    // traversal pattern (static or dynamic).
    template<TreePathType::Type tpType>
    struct TreePathFactory;

    // Factory for static traversal.
    template<>
    struct TreePathFactory<TreePathType::fullyStatic>
    {
      template<typename Tree>
      static TreePath<> create(const Tree& tree)
      {
        return TreePath<>();
      }
    };

    // Factory for dynamic traversal.
    template<>
    struct TreePathFactory<TreePathType::dynamic>
    {
      template<typename Tree>
      static MakeableDynamicTreePath<TreeInfo<Tree>::depth> create(const Tree& tree)
      {
        return MakeableDynamicTreePath<TreeInfo<Tree>::depth>();
      }
    };

#endif // DOXYGEN

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TREEPATH_HH
