// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_GENERICTRANSFORMATIONDESCRIPTORS_HH
#define DUNE_TYPETREE_GENERICTRANSFORMATIONDESCRIPTORS_HH

#include <dune/typetree/nodetags.hh>
#include <dune/typetree/powercompositenodetransformationtemplates.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>
#include <dune/common/array.hh>
#include <dune/common/tuples.hh>


namespace Dune {
  namespace TypeTree {

    /** \addtogroup Transformation
     *  \ingroup TypeTree
     *  \{
     */

    template<typename SourceNode, typename Transformation, typename TransformedNode>
    struct GenericLeafNodeTransformation
    {

      static const bool recursive = false;

      typedef TransformedNode transformed_type;
      typedef shared_ptr<transformed_type> transformed_storage_type;

      static transformed_type transform(const SourceNode& s, const Transformation& t)
      {
        return transformed_type(s,t);
      }

      static transformed_type transform(shared_ptr<const SourceNode> s, const Transformation& t)
      {
        return transformed_type(s,t);
      }

      static transformed_storage_type transform_storage(shared_ptr<const SourceNode> s, const Transformation& t)
      {
        return make_shared<transformed_type>(s,t);
      }

    };


    template<typename SourceNode, typename Transformation, template<typename Child> class TransformedNodeTemplate>
    struct TemplatizedGenericPowerNodeTransformation
    {

      static const bool recursive = true;

      template<typename TC>
      struct result
      {
        typedef typename TransformedNodeTemplate<TC>::type type;
        typedef shared_ptr<type> storage_type;
      };

      template<typename TC>
      static typename result<TC>::type transform(const SourceNode& s, const Transformation& t, const array<shared_ptr<TC>,result<TC>::type::CHILDREN>& children)
      {
        return typename result<TC>::type(s,t,children);
      }

      template<typename TC>
      static typename result<TC>::type transform(shared_ptr<const SourceNode> s, const Transformation& t, const array<shared_ptr<TC>,result<TC>::type::CHILDREN>& children)
      {
        return typename result<TC>::type(s,t,children);
      }

      template<typename TC>
      static typename result<TC>::storage_type transform_storage(shared_ptr<const SourceNode> s, const Transformation& t, const array<shared_ptr<TC>,result<TC>::type::CHILDREN>& children)
      {
        return make_shared<typename result<TC>::type>(s,t,children);
      }

    };


    template<typename SourceNode, typename Transformation, template<typename,typename,std::size_t> class TransformedNode>
    struct GenericPowerNodeTransformation
      : public TemplatizedGenericPowerNodeTransformation<SourceNode,
                                                         Transformation,
                                                         GenericPowerNodeTransformationTemplate<SourceNode,
                                                                                                Transformation,
                                                                                                TransformedNode>::template result
                                                         >
    {};


    template<typename SourceNode, typename Transformation, template<typename...> class TransformedNodeTemplate>
    struct TemplatizedGenericCompositeNodeTransformation
    {

      static const bool recursive = true;

      template<typename... TC>
      struct result
      {
        typedef typename TransformedNodeTemplate<TC...>::type type;
        typedef shared_ptr<type> storage_type;
      };

      template<typename... TC>
      static typename result<TC...>::type transform(const SourceNode& s, const Transformation& t, shared_ptr<TC>... children)
      {
        return typename result<TC...>::type(s,t,children...);
      }

      template<typename... TC>
      static typename result<TC...>::type transform(shared_ptr<const SourceNode> s, const Transformation& t, shared_ptr<TC>... children)
      {
        return typename result<TC...>::type(s,t,children...);
      }

      template<typename... TC>
      static typename result<TC...>::storage_type transform_storage(shared_ptr<const SourceNode> s, const Transformation& t, shared_ptr<TC>... children)
      {
        return make_shared<typename result<TC...>::type>(s,t,children...);
      }

    };


    template<typename SourceNode, typename Transformation, template<typename,typename...> class TransformedNode>
    struct GenericCompositeNodeTransformation
      : public TemplatizedGenericCompositeNodeTransformation<SourceNode,
                                                             Transformation,
                                                                     GenericCompositeNodeTransformationTemplate<SourceNode,
                                                                                                                Transformation,
                                                                                                                TransformedNode>::template result
                                                             >
    {};

    //! \} group Transformation

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_GENERICTRANSFORMATIONDESCRIPTORS_HH
