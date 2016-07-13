// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_SIMPLETRANSFORMATIONDESCRIPTORS_HH
#define DUNE_TYPETREE_SIMPLETRANSFORMATIONDESCRIPTORS_HH

#include <array>
#include <memory>

#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/common/exceptions.hh>


namespace Dune {
  namespace TypeTree {

    /** \addtogroup Transformation
     *  \ingroup TypeTree
     *  \{
     */

    template<typename SourceNode, typename Transformation, typename TransformedNode>
    struct SimpleLeafNodeTransformation
    {

      static const bool recursive = false;

      typedef TransformedNode transformed_type;
      typedef std::shared_ptr<transformed_type> transformed_storage_type;

      static transformed_type transform(const SourceNode& s, const Transformation& t)
      {
        return transformed_type();
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t)
      {
        return std::make_shared<transformed_type>();
      }

    };


    template<typename SourceNode, typename Transformation, template<typename Child, std::size_t> class TransformedNode>
    struct SimplePowerNodeTransformation
    {

      static const bool recursive = true;

      template<typename TC>
      struct result
      {
        typedef TransformedNode<TC, StaticDegree<SourceNode>::value> type;
        typedef std::shared_ptr<type> storage_type;
        static const std::size_t degree = StaticDegree<type>::value;
      };

      template<typename TC>
      static typename result<TC>::type transform(const SourceNode& s, const Transformation& t, const std::array<std::shared_ptr<TC>,result<TC>::degree>& children)
      {
        return typename result<TC>::type(children);
      }

      template<typename TC>
      static typename result<TC>::storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t, const std::array<std::shared_ptr<TC>,result<TC>::degree>& children)
      {
        return std::make_shared<typename result<TC>::type>(children);
      }

    };


    template<typename SourceNode, typename Transformation, template<typename...> class TransformedNode>
    struct SimpleCompositeNodeTransformation
    {

      static const bool recursive = true;

      template<typename... TC>
      struct result
      {
        typedef TransformedNode<TC...> type;
        typedef std::shared_ptr<type> storage_type;
      };

      template<typename... TC>
      static typename result<TC...>::type transform(const SourceNode& s, const Transformation& t, std::shared_ptr<TC>... children)
      {
        return typename result<TC...>::type(children...);
      }

      template<typename... TC>
      static typename result<TC...>::storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t, std::shared_ptr<TC>... children)
      {
        return std::make_shared<typename result<TC...>::type>(children...);
      }

    };

    //! \} group Transformation

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_SIMPLETRANSFORMATIONDESCRIPTORS_HH
