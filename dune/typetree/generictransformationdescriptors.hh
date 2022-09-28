// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_GENERICTRANSFORMATIONDESCRIPTORS_HH
#define DUNE_TYPETREE_GENERICTRANSFORMATIONDESCRIPTORS_HH

#include <array>
#include <memory>

#include <dune/typetree/nodeinterface.hh>
#include <dune/typetree/nodetags.hh>
#include <dune/typetree/powercompositenodetransformationtemplates.hh>
#include <dune/common/exceptions.hh>


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
      typedef std::shared_ptr<transformed_type> transformed_storage_type;

      static transformed_type transform(const SourceNode& s, const Transformation& t)
      {
        return transformed_type(s,t);
      }

      static transformed_type transform(std::shared_ptr<const SourceNode> s, const Transformation& t)
      {
        return transformed_type(s,t);
      }

      static transformed_storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t)
      {
        return std::make_shared<transformed_type>(s,t);
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
        typedef std::shared_ptr<type> storage_type;
        static const std::size_t degree = StaticDegree<type>::value;
      };

      template<typename TC>
      static typename result<TC>::type transform(const SourceNode& s, const Transformation& t, const std::array<std::shared_ptr<TC>,result<TC>::degree>& children)
      {
        return typename result<TC>::type(s,t,children);
      }

      template<typename TC>
      static typename result<TC>::type transform(std::shared_ptr<const SourceNode> s, const Transformation& t, const std::array<std::shared_ptr<TC>,result<TC>::degree>& children)
      {
        return typename result<TC>::type(s,t,children);
      }

      template<typename TC>
      static typename result<TC>::storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t, const std::array<std::shared_ptr<TC>,result<TC>::degree>& children)
      {
        return std::make_shared<typename result<TC>::type>(s,t,children);
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


    template<typename SourceNode, typename Transformation, template<typename Child> class TransformedNodeTemplate>
    struct TemplatizedGenericDynamicPowerNodeTransformation
    {

      static const bool recursive = true;

      template<typename TC>
      struct result
      {
        typedef typename TransformedNodeTemplate<TC>::type type;
        typedef std::shared_ptr<type> storage_type;
      };

      template<typename TC>
      static typename result<TC>::type transform(const SourceNode& s, const Transformation& t, const std::vector<std::shared_ptr<TC>>& children)
      {
        return typename result<TC>::type(s,t,children);
      }

      template<typename TC>
      static typename result<TC>::type transform(std::shared_ptr<const SourceNode> s, const Transformation& t, const std::vector<std::shared_ptr<TC>>& children)
      {
        return typename result<TC>::type(s,t,children);
      }

      template<typename TC>
      static typename result<TC>::storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t, const std::vector<std::shared_ptr<TC>>& children)
      {
        return std::make_shared<typename result<TC>::type>(s,t,children);
      }

    };


    template<typename SourceNode, typename Transformation, template<typename,typename> class TransformedNode>
    struct GenericDynamicPowerNodeTransformation
      : public TemplatizedGenericDynamicPowerNodeTransformation<SourceNode,
                                                         Transformation,
                                                         GenericDynamicPowerNodeTransformationTemplate<SourceNode,
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
        typedef std::shared_ptr<type> storage_type;
      };

      template<typename... TC>
      static typename result<TC...>::type transform(const SourceNode& s, const Transformation& t, std::shared_ptr<TC>... children)
      {
        return typename result<TC...>::type(s,t,children...);
      }

      template<typename... TC>
      static typename result<TC...>::type transform(std::shared_ptr<const SourceNode> s, const Transformation& t, std::shared_ptr<TC>... children)
      {
        return typename result<TC...>::type(s,t,children...);
      }

      template<typename... TC>
      static typename result<TC...>::storage_type transform_storage(std::shared_ptr<const SourceNode> s, const Transformation& t, std::shared_ptr<TC>... children)
      {
        return std::make_shared<typename result<TC...>::type>(s,t,children...);
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
