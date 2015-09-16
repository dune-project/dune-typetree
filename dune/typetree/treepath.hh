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

     //! Returns the size (number of components) of the given `TreePath`.
    template<std::size_t... i>
    constexpr std::size_t treePathSize(const TreePath<i...>&)
    {
      return sizeof...(i);
    }

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


    //! A hybrid version of TreePath that supports both compile time and run time indices.
    /**
     * A `HybridTreePath` supports storing a combination of run time and compile time indices.
     * This makes it possible to store the tree path to a tree node inside the tree node itself,
     * even if the path contains one or more `PowerNode`s, where each child must have exactly the
     * same type. At the same time, as much information as possible is kept accessible at compile
     * time, allowing for more efficient algorithms.
     *
     */
    template<typename... T>
    class HybridTreePath
    {

    public:

      //! An `index_sequence` for the entries in this `HybridTreePath`.
      using index_sequence = Std::index_sequence_for<T...>;

      //! Default constructor
      constexpr HybridTreePath()
      {}

      //! Constructor from a `std::tuple`
      constexpr HybridTreePath(std::tuple<T...> t)
        : _data(t)
      {}

      //! Constructor from arguments
      template<typename... U, typename std::enable_if<(sizeof...(T) > 0 && sizeof...(U) == sizeof...(T)),bool>::type = true>
      constexpr HybridTreePath(U... t)
        : _data(t...)
      {}

      //! Returns an index_sequence for enumerating the components of this HybridTreePath.
      constexpr static index_sequence enumerate()
      {
        return {};
      }

#ifndef DOXYGEN

      // I can't be bothered to make all the external accessors friends of HybridTreePath,
      // so we'll only hide the data tuple from the user in Doxygen.

      using Data = std::tuple<T...>;
      Data _data;

#endif // DOXYGEN

    };


    //! Constructs a new `HybridTreePath` from the given indices.
    /**
     * This function returns a new `HybridTreePath` with the given index values. It exists
     * mainly to avoid having to manually specify the exact type of the new object.
     */
    template<typename... T>
    constexpr HybridTreePath<T...> hybridTreePath(const T&... t)
    {
      return HybridTreePath<T...>(t...);
    }


    //! Returns the size (number of components) of the given `HybridTreePath`.
    template<typename... T>
    constexpr std::size_t treePathSize(const HybridTreePath<T...>&)
    {
      return sizeof...(T);
    }

    //! Returns a copy of the i-th element of the `HybridTreePath`.
    /**
     * As `HybridTreePath` instances should not be mutated after their creation, this function
     * returns a copy of the value. As values are either `std::integral_constant` or `std::size_t`, that's
     * just as cheap as returning a reference.
     *
     * The index for the entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::TypeTree::Indices;
     * // the following two lines are equivalent
     * std::cout << treePathEntry<2>(tp) << std::endl;
     * std::cout << treePathEntry(tp,_2) << std::endl;
     * \endcode
     *
     */
    template<std::size_t i, typename... T>
    auto treePathEntry(const HybridTreePath<T...>& tp, index_constant<i> = {})
      -> typename std::decay<decltype(std::get<i>(tp._data))>::type
    {
      return std::get<i>(tp._data);
    }

    //! Returns the index value of the i-th element of the `HybridTreePath`.
    /**
     * This function always returns the actual index value, irrespective of whether the
     * entry is a compile time index or a run time index.
     *
     * The index for the entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::TypeTree::Indices;
     * // the following two lines are equivalent
     * std::cout << treePathIndex<2>(tp) << std::endl;
     * std::cout << treePathIndex(tp,_2) << std::endl;
     * \endcode
     *
     */
    template<std::size_t i,typename... T>
    std::size_t treePathIndex(const HybridTreePath<T...>& tp, index_constant<i> = {})
    {
      return std::get<i>(tp._data);
    }

    //! Returns a copy of the last element of the `HybridTreePath`.
    /**
     * As `HybridTreePath` instances should not be mutated after their creation, this function
     * returns a copy of the value. As values are either `std::integral_constant` or `std::size_t`, that's
     * just as cheap as returning a reference.
     */
    template<typename... T, typename std::enable_if<(sizeof...(T) > 0),bool>::type = true>
    auto back(const HybridTreePath<T...>& tp)
      -> decltype(treePathEntry<treePathSize(tp)-1>(tp))
    {
      return treePathEntry<treePathSize(tp)-1>(tp);
    }

    //! Returns the index value of the last element of the `HybridTreePath`.
    template<typename... T, typename std::enable_if<(sizeof...(T) > 0),bool>::type = true>
    std::size_t backIndex(const HybridTreePath<T...>& tp)
    {
      return treePathEntry<treePathSize(tp)-1>(tp);
    }

    //! Returns a copy of the first element of the `HybridTreePath`.
    /**
     * As `HybridTreePath` instances should not be mutated after their creation, this function
     * returns a copy of the value. As values are either `std::integral_constant` or `std::size_t`, that's
     * just as cheap as returning a reference.
     */
    template<typename... T>
    auto front(const HybridTreePath<T...>& tp)
      -> decltype(treePathEntry<0>(tp))
    {
      return treePathEntry<0>(tp);
    }

    //! Returns the index value of the first element of the `HybridTreePath`.
    template<typename... T>
    std::size_t frontIndex(const HybridTreePath<T...>& tp)
    {
      return treePathEntry<0>(tp);
    }

    //! Appends a run time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the run time index `i` appended.
     */
    template<typename... T>
    HybridTreePath<T...,std::size_t> push_back(const HybridTreePath<T...>& tp, std::size_t i)
    {
      return HybridTreePath<T...,std::size_t>(std::tuple_cat(tp._data,std::make_tuple(i)));
    }

    //! Appends a compile time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the compile time index `i` appended.
     *
     * The value for the new entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::TypeTree::Indices;
     * // the following two lines are equivalent
     * auto tp_a = push_back<1>(tp);
     * auto tp_b = push_back(tp,_1);
     * \endcode
     *
     */
    template<std::size_t i, typename... T>
    HybridTreePath<T...,index_constant<i>> push_back(const HybridTreePath<T...>& tp, index_constant<i> i_ = {})
    {
      return HybridTreePath<T...,index_constant<i> >(std::tuple_cat(tp._data,std::make_tuple(i_)));
    }

    //! Prepends a run time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the run time index `i` prepended.
     */
    template<typename... T>
    HybridTreePath<std::size_t,T...> push_front(const HybridTreePath<T...>& tp, std::size_t element)
    {
      return HybridTreePath<std::size_t,T...>(std::tuple_cat(std::make_tuple(element),tp._data));
    }

    //! Prepends a compile time index to a `HybridTreePath`.
    /**
     * This function returns a new `HybridTreePath` with the compile time index `i` prepended.
     *
     * The value for the new entry can be passed either as a template parameter or as an `index_constant`:
     *
     * \code{.cc}
     * auto tp = hybridTreePath(1,2,3,4);
     * using namespace Dune::TypeTree::Indices;
     * // the following two lines are equivalent
     * auto tp_a = push_front<1>(tp);
     * auto tp_b = push_front(tp,_1);
     * \endcode
     *
     */
    template<std::size_t i, typename... T>
    HybridTreePath<index_constant<i>,T...> push_front(const HybridTreePath<T...>& tp, index_constant<i> _i = {})
    {
      return HybridTreePath<index_constant<i>,T...>(std::tuple_cat(std::make_tuple(_i),tp._data));
    }

#ifndef DOXYGEN

    namespace impl {

      // end of recursion
      template<std::size_t i, typename... T>
      typename std::enable_if<
        (i == sizeof...(T))
        >::type
      print_hybrid_tree_path(std::ostream& os, const HybridTreePath<T...>& tp, index_constant<i> _i)
      {}

      // print current entry and recurse
      template<std::size_t i, typename... T>
      typename std::enable_if<
        (i < sizeof...(T))
        >::type
      print_hybrid_tree_path(std::ostream& os, const HybridTreePath<T...>& tp, index_constant<i> _i)
      {
        os << treePathIndex(tp,_i) << " ";
        print_hybrid_tree_path(os,tp,index_constant<i+1>{});
      }

    } // namespace impl

#endif // DOXYGEN

    //! Dumps a `HybridTreePath` to a stream.
    template<typename... T>
    std::ostream& operator<<(std::ostream& os, const HybridTreePath<T...>& tp)
    {
      os << "HybridTreePath< ";
      impl::print_hybrid_tree_path(os, tp, index_constant<0>{});
      os << ">";
      return os;
    }

    //! \} group TypeTree

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TREEPATH_HH
