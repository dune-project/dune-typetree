// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_POWERCOMPOSITENODETRANSFORMATIONTEMPLATES_HH
#define DUNE_TYPETREE_POWERCOMPOSITENODETRANSFORMATIONTEMPLATES_HH

#include <cstddef>

#include <dune/typetree/nodeinterface.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Transformation
     *  \ingroup TypeTree
     *  \{
     */

    template<typename SourceNode, typename Transformation, template<typename,typename,std::size_t> class TransformedNode>
    struct GenericPowerNodeTransformationTemplate
    {
      template<typename TC>
      struct result
      {
        typedef TransformedNode<SourceNode,TC,StaticDegree<SourceNode>::value> type;
      };
    };


    template<typename SourceNode, typename Transformation, template<typename,typename> class TransformedNode>
    struct GenericDynamicPowerNodeTransformationTemplate
    {
      template<typename TC>
      struct result
      {
        typedef TransformedNode<SourceNode,TC> type;
      };
    };

    template<typename SourceNode, typename Transformation, template<typename,typename...> class TransformedNode>
    struct GenericCompositeNodeTransformationTemplate
    {
      template<typename... TC>
      struct result
      {
        typedef TransformedNode<SourceNode,TC...> type;
      };
    };

    //! \} group Transformation

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_POWERCOMPOSITENODETRANSFORMATIONTEMPLATES_HH
