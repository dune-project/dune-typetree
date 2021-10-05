// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TRANSFORMATION_HH
#define DUNE_TYPETREE_TRANSFORMATION_HH

#include <array>
#include <tuple>
#include <memory>
#include <utility>

#include <dune/common/hybridutilities.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/typetraits.hh>
#include <dune/typetree/typetraits.hh>
#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/typetree/utility.hh>


namespace Dune {
  namespace TypeTree {

    /** \addtogroup Transformation
     *  \ingroup TypeTree
     *  \{
     */

#ifdef DOXYGEN

    //! Register transformation descriptor to transform SourceNode with Transformation.
    /**
     * The tree transformation engine expects this function to return a struct describing
     * how to perform the Transformation for the type SourceNode, which has ImplementationTag Tag.
     * This function has to be specialized for every combination of Transformation and Tag that
     * the transformation engine should support.
     *
     * \note The arguments are given as pointers to avoid problems with incomplete types.
     *
     * \note The specialization does not have to placed in the namespace Dune::TypeTree,
     *       it can simply reside in the same namespace as either the SourceNode or the Tag.
     *
     * \note This function will never be really called, the engine only extracts the return type.
     *       It is thus not necessary to actually implement the function, it is sufficient to
     *       declare it.
     *
     * \tparam SourceNode     The type of the node in the source tree that should be transformed.
     * \tparam Transformation The type of the transformation to apply to the source node.
     * \tparam Tag            The implementation tag of the source node.
     */
    template<typename SourceNode, typename Transformation, typename Tag>
    void registerNodeTransformation(SourceNode*, Transformation*, Tag*);

#else // DOXYGEN

      /**
       * \tparam S   C++ type of source node.
       * \tparam T   Tag identifying the transformation.
       * \tparam Tag Tag identifying the source type.
       *
       * Tag may be identical for different implementation of the same concept
       * (i.e. all leaf GridFunctionSpace), but this is not required.  This
       * allows you to handle different leaf GridFunctionSpace implementation
       * differently.  Tag should be extracted from S::ImplementationTag.
       */
    template<typename S, typename T, typename Tag>
    struct LookupNodeTransformation
    {

      typedef decltype(registerNodeTransformation(declptr<S>(),declptr<T>(),declptr<Tag>())) lookup_type;

      typedef typename evaluate_if_meta_function<
        lookup_type
        >::type type;

      static_assert((!std::is_same<type,void>::value), "Unable to find valid transformation descriptor");
    };

#endif // DOXYGEN


    //! Transform a TypeTree.
    /**
     * This struct can be used to apply a transformation to a given TypeTree. It exports the type of
     * the resulting (transformed) tree and contains methods to actually transform tree instances.
     *
     * \tparam SourceTree     = The TypeTree that should be transformed.
     * \tparam Transformation = The Transformation to apply to the TypeTree.
     * \tparam Tag            = This parameter is an implementation detail and must always be set to its default value.
     * \tparam recursive      = This parameter is an implementation detail and must always be set to its default value.
     */
    template<typename SourceTree, typename Transformation, typename Tag = StartTag, bool recursive = true>
    struct TransformTree
    {

#ifndef DOXYGEN

      typedef typename LookupNodeTransformation<SourceTree,Transformation,typename SourceTree::ImplementationTag>::type NodeTransformation;

      // the type of the new tree that will result from this transformation
      typedef typename TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transformed_type transformed_type;

      // the storage type of the new tree that will result from this transformation
      typedef typename TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transformed_storage_type transformed_storage_type;

#endif // DOXYGEN

      //! The type of the transformed tree.
      typedef transformed_type type;

      typedef type Type;

      //! Apply transformation to an existing tree s.
      static transformed_type transform(const SourceTree& s, const Transformation& t = Transformation())
      {
        return TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transform(s,t);
      }

      //! Apply transformation to an existing tree s.
      static transformed_type transform(const SourceTree& s, Transformation& t)
      {
        return TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transform(s,t);
      }

      //! Apply transformation to an existing tree s.
      static transformed_type transform(std::shared_ptr<const SourceTree> sp, const Transformation& t = Transformation())
      {
        return TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transform(sp,t);
      }

      //! Apply transformation to an existing tree s.
      static transformed_type transform(std::shared_ptr<const SourceTree> sp, Transformation& t)
      {
        return TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transform(sp,t);
      }

      //! Apply transformation to storage type of an existing tree, returning a heap-allocated storage type
      //! instance of the transformed tree.
      static transformed_storage_type transform_storage(std::shared_ptr<const SourceTree> sp, const Transformation& t = Transformation())
      {
        return TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transform_storage(sp,t);
      }

      //! Apply transformation to storage type of an existing tree, returning a heap-allocated storage type
      //! instance of the transformed tree.
      static transformed_storage_type transform_storage(std::shared_ptr<const SourceTree> sp, Transformation& t)
      {
        return TransformTree<SourceTree,Transformation,NodeTag<SourceTree>,NodeTransformation::recursive>::transform_storage(sp,t);
      }


    };

#ifndef DOXYGEN // internal per-node implementations of the transformation algorithm

    // handle a leaf node - this is easy
    template<typename S, typename T, bool recursive>
    struct TransformTree<S,T,LeafNodeTag,recursive>
    {
      // get transformed type from specification
      typedef typename LookupNodeTransformation<S,T,ImplementationTag<S>>::type NodeTransformation;

      typedef typename NodeTransformation::transformed_type transformed_type;
      typedef typename NodeTransformation::transformed_storage_type transformed_storage_type;

      // delegate instance transformation to per-node specification
      static transformed_type transform(const S& s, T& t)
      {
        return NodeTransformation::transform(s,t);
      }

      // delegate instance transformation to per-node specification
      static transformed_type transform(const S& s, const T& t)
      {
        return NodeTransformation::transform(s,t);
      }

      // delegate instance transformation to per-node specification
      static transformed_type transform(std::shared_ptr<const S> sp, T& t)
      {
        return NodeTransformation::transform(sp,t);
      }

      // delegate instance transformation to per-node specification
      static transformed_type transform(std::shared_ptr<const S> sp, const T& t)
      {
        return NodeTransformation::transform(sp,t);
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, T& t)
      {
        return NodeTransformation::transform_storage(sp,t);
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, const T& t)
      {
        return NodeTransformation::transform_storage(sp,t);
      }

    };


    // common implementation for non-recursive transformation of non-leaf nodes
    template<typename S, typename T>
    struct TransformTreeNonRecursive
    {
      // get transformed type from specification
      typedef typename LookupNodeTransformation<S,T,ImplementationTag<S>>::type NodeTransformation;

      typedef typename NodeTransformation::transformed_type transformed_type;
      typedef typename NodeTransformation::transformed_storage_type transformed_storage_type;

      // delegate instance transformation to per-node specification
      static transformed_type transform(const S& s, T& t)
      {
        return NodeTransformation::transform(s,t);
      }

      // delegate instance transformation to per-node specification
      static transformed_type transform(const S& s, const T& t)
      {
        return NodeTransformation::transform(s,t);
      }

      // delegate instance transformation to per-node specification
      static transformed_type transform(std::shared_ptr<const S> sp, T& t)
      {
        return NodeTransformation::transform(sp,t);
      }

      // delegate instance transformation to per-node specification
      static transformed_type transform(std::shared_ptr<const S> sp, const T& t)
      {
        return NodeTransformation::transform(sp,t);
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, T& t)
      {
        return NodeTransformation::transform_storage(sp,t);
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, const T& t)
      {
        return NodeTransformation::transform_storage(sp,t);
      }

    };


    namespace Impl {

      // Helper class to handle recursive power nodes
      template<class Source, class Transformation, class Tag>
      class RecursivePowerTransformTree
      {
        // We only know two types of tags!
        static_assert(std::is_same_v<Tag,PowerNodeTag> or std::is_same_v<Tag,DynamicPowerNodeTag>);

        using ChildType = typename Source::ChildType;

        // in case degree is dynamic, provid a vector correctly initialized
        template<class NodeStorage>
        static auto node_storage_provider(const std::size_t& degree)
        {
          return std::vector<NodeStorage>(degree);
        }

        // in case degree is static, provid an array
        template<class NodeStorage, class StaticIndex>
        static auto node_storage_provider(StaticIndex)
        {
          return std::array<NodeStorage,std::size_t(StaticIndex{})>();
        }

      public:
        // get transformed type from specification
        // Handling this transformation in a way that makes the per-node specification easy to write
        // is a little involved:
        // The problem is that the transformed power node must be parameterized on the transformed child
        // type. So we need to transform the child type and pass the transformed child type to an inner
        // template of the node transformation struct called result (see example of such a specification
        // further down).
        using NodeTransformation = typename LookupNodeTransformation<Source,Transformation,ImplementationTag<Source>>::type;
        using ChildNodeTransformation = typename LookupNodeTransformation<ChildType,Transformation,ImplementationTag<ChildType>>::type;

      private:
        // Since every child is same type, is enough to get transformation once
        using ChildTreeTransformation = TransformTree<ChildType,
                                                      Transformation,
                                                      NodeTag<ChildType>,
                                                      ChildNodeTransformation::recursive>;

        // Get transformed type of children
        using transformed_child_type = typename ChildTreeTransformation::transformed_type;
        using transformed_child_storage_type = typename ChildTreeTransformation::transformed_storage_type;
      public:
        // Apply transformation from children to current node
        using transformed_type = typename NodeTransformation::template result<transformed_child_type>::type;
        using transformed_storage_type = typename NodeTransformation::template result<transformed_child_type>::storage_type;

        // Transform an instance of source tree.
        static transformed_type transform(const Source& source, Transformation& transformation)
        {
          auto children_storage = node_storage_provider<std::shared_ptr<transformed_child_type>>(source.degree());
          for (std::size_t k = 0; k < source.degree(); ++k) {
            children_storage[k] = ChildTreeTransformation::transform_storage(source.childStorage(k),transformation);
          }
          return NodeTransformation::transform(source,transformation,children_storage);
        }

        // Transform an instance of source tree.
        static transformed_type transform(const Source& source, const Transformation& transformation)
        {
          auto children_storage = node_storage_provider<std::shared_ptr<transformed_child_type>>(source.degree());
          for (std::size_t k = 0; k < source.degree(); ++k) {
            children_storage[k] = ChildTreeTransformation::transform_storage(source.childStorage(k),transformation);
          }
          return NodeTransformation::transform(source,transformation,children_storage);
        }

        // Transform an instance of source tree.
        static transformed_type transform(std::shared_ptr<const Source> source_ptr, Transformation& transformation)
        {
          auto children_storage = node_storage_provider<std::shared_ptr<transformed_child_type>>(source_ptr->degree());
          for (std::size_t k = 0; k < source_ptr->degree(); ++k) {
            children_storage[k] = ChildTreeTransformation::transform_storage(source_ptr->childStorage(k),transformation);
          }
          return NodeTransformation::transform(source_ptr,transformation,children_storage);
        }

        // Transform an instance of source tree.
        static transformed_type transform(std::shared_ptr<const Source> source_ptr, const Transformation& transformation)
        {
          auto children_storage = node_storage_provider<std::shared_ptr<transformed_child_type>>(source_ptr->degree());
          for (std::size_t k = 0; k < source_ptr->degree(); ++k) {
            children_storage[k] = ChildTreeTransformation::transform_storage(source_ptr->childStorage(k),transformation);
          }
          return NodeTransformation::transform(source_ptr,transformation,children_storage);
        }

        // Transform an instance of source tree ptr.
        static transformed_storage_type transform_storage(std::shared_ptr<const Source> source_ptr, Transformation& transformation)
        {
          auto children_storage = node_storage_provider<transformed_child_storage_type>(source_ptr->degree());
          for (std::size_t k = 0; k < source_ptr->degree(); ++k) {
            children_storage[k] = ChildTreeTransformation::transform_storage(source_ptr->childStorage(k),transformation);
          }
          return NodeTransformation::transform_storage(source_ptr,transformation,children_storage);
        }

        // Transform an instance of source tree ptr.
        static transformed_storage_type transform_storage(std::shared_ptr<const Source> source_ptr, const Transformation& transformation)
        {
          auto children_storage = node_storage_provider<transformed_child_storage_type>(source_ptr->degree());
          for (std::size_t k = 0; k < source_ptr->degree(); ++k) {
            children_storage[k] = ChildTreeTransformation::transform_storage(source_ptr->childStorage(k),transformation);
          }
          return NodeTransformation::transform_storage(source_ptr,transformation,children_storage);
        }

      };
    } // namespace Impl

    // Recursive version of the PowerNode transformation for static nodes.
    template<typename Source, typename Transformation>
    struct TransformTree<Source,Transformation,PowerNodeTag,true>
      : public Impl::RecursivePowerTransformTree<Source,Transformation,PowerNodeTag>
    {};

    // Recursive version of the DynamicPowerNode transformation for static nodes.
    template<typename Source, typename Transformation>
    struct TransformTree<Source,Transformation,DynamicPowerNodeTag,true>
      : public Impl::RecursivePowerTransformTree<Source,Transformation,DynamicPowerNodeTag>
    {};

    // non-recursive version of the PowerNode transformation.
    template<typename S, typename T>
    struct TransformTree<S,T,PowerNodeTag,false>
      : public TransformTreeNonRecursive<S,T>
    {};

    // non-recursive version of the DynamicPowerNodeTag transformation.
    template<typename S, typename T>
    struct TransformTree<S,T,DynamicPowerNodeTag,false>
      : public TransformTreeNonRecursive<S,T>
    {};

    // helper struct that does the actual transformation for a composite node. We need this additional struct
    // to extract the template argument list with the types of all children from the node, which we cannot do
    // directly in the transformation<> template, as the type passed to transformation<> will usually be a
    // derived type and will normally have more template arguments than just the children. This declaration
    // just introduces the type of the helper struct, we always instantiate the specialization defined below;
    template<typename S, typename Children, typename T>
    struct transform_composite_node;

    // specialized version of the helper struct which extracts the template argument list with the children from
    // its second template parameter, which has to be CompositeNode::ChildTypes. Apart from that, the struct is
    // similar to the one for a PowerNode, but it obviously delegates transformation of the children to the TMP.
    template<typename S, typename T, typename... C>
    struct transform_composite_node<S,std::tuple<C...>,T>
    {

      // transformed type, using the same nested struct trick as the PowerNode
      typedef ImplementationTag<S> Tag;
      typedef typename LookupNodeTransformation<S,T,Tag>::type NodeTransformation;
      typedef typename NodeTransformation::template result<typename TransformTree<C,
                                                                                  T,
                                                                                  NodeTag<C>,
                                                                                  LookupNodeTransformation<C,T,ImplementationTag<C>>::type::recursive
                                                                                  >::transformed_type...
                                                           >::type transformed_type;

      typedef typename NodeTransformation::template result<typename TransformTree<C,
                                                                                  T,
                                                                                  NodeTag<C>,
                                                                                  LookupNodeTransformation<C,T,ImplementationTag<C>>::type::recursive
                                                                                  >::transformed_type...
                                                           >::storage_type transformed_storage_type;

      // Retrieve the transformation descriptor for the child with index i.
      // This little helper improves really improves the readability of the
      // transformation functions.
      template<std::size_t i>
      struct ChildTransformation
        : public TransformTree<typename S::template Child<i>::Type,
                               T,
                               NodeTag<typename S::template Child<i>::Type>,
                               LookupNodeTransformation<
                                 typename S::template Child<i>::Type,
                                 T,
                                 ImplementationTag<typename S::template Child<i>::Type>
                                 >::type::recursive
                               >
      {};

      template<std::size_t i, typename Tuple, typename Value>
      static void setElement(Tuple& tuple, Value&& value)
      {
        std::get<i>(tuple) = std::forward<Value>(value);
      }

      template<typename Trafo, std::size_t... i>
      static transformed_type transform(const S& s, Trafo&& t, std::index_sequence<i...> indices)
      {
        std::tuple<typename ChildTransformation<i>::transformed_storage_type...> storage;
        Dune::Hybrid::Impl::evaluateFoldExpression<int>({(setElement<i>(storage, ChildTransformation<i>::transform_storage(s.template childStorage<i>(), std::forward<Trafo>(t))),0)...});
        return NodeTransformation::transform(s, std::forward<Trafo>(t), std::get<i>(storage)...);
      }

      template<typename Trafo, std::size_t... i>
      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, Trafo&& t, std::index_sequence<i...> indices)
      {
        std::tuple<typename ChildTransformation<i>::transformed_storage_type...> storage;
        Dune::Hybrid::Impl::evaluateFoldExpression<int>({(setElement<i>(storage, ChildTransformation<i>::transform_storage(sp->template childStorage<i>(), std::forward<Trafo>(t))),0)...});
        return NodeTransformation::transform_storage(sp, std::forward<Trafo>(t), std::get<i>(storage)...);
      }
    };


    // the specialization of transformation<> for the CompositeNode. This just extracts the
    // CompositeNode::ChildTypes member and forwards to the helper struct
    template<typename S, typename T>
    struct TransformTree<S,T,CompositeNodeTag,true>
    {

    private:

      typedef typename S::ChildTypes ChildTypes;

      static auto child_indices()
      {
        return std::make_index_sequence<S::degree()>();
      }

    public:

      typedef typename transform_composite_node<S,ChildTypes,T>::transformed_type transformed_type;
      typedef typename transform_composite_node<S,ChildTypes,T>::transformed_storage_type transformed_storage_type;

      static transformed_type transform(const S& s, T& t)
      {
        return transform_composite_node<S,ChildTypes,T>::transform(s,t,child_indices());
      }

      static transformed_type transform(const S& s, const T& t)
      {
        return transform_composite_node<S,ChildTypes,T>::transform(s,t,child_indices());
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, T& t)
      {
        return transform_composite_node<S,ChildTypes,T>::transform_storage(sp,t,child_indices());
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const S> sp, const T& t)
      {
        return transform_composite_node<S,ChildTypes,T>::transform_storage(sp,t,child_indices());
      }

    };

    // non-recursive version of the CompositeNode transformation.
    template<typename S, typename T>
    struct TransformTree<S,T,CompositeNodeTag,false>
      : public TransformTreeNonRecursive<S,T>
    {};

#endif // DOXYGEN

    //! \} group Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TRANSFORMATION_HH
