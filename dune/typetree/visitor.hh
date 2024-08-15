// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_VISITOR_HH
#define DUNE_TYPETREE_VISITOR_HH

#include <dune/typetree/treepath.hh>
#include <dune/common/hybridutilities.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */

    //! Visitor interface and base class for TypeTree visitors.
    /**
     * DefaultVisitor defines the interface for visitors that can be applied to a TypeTree
     * using applyToTree(). Each method of the visitor is passed a node of the tree (either as
     * a mutable or a const reference, depending on the constness of the tree applyToTree() was
     * called with). The second argument is of type TreePath and denotes the exact position of the
     * node within the TypeTree, encoded as child indices starting at the root node.
     *
     * In order to create a functioning visitor, an implementation will - in addition to providing the methods
     * of this class - also have to contain the following template struct, which is used to determine
     * whether to visit a given node:
     *
     * \code
     * template<typename Node, typename Child, typename TreePath>
     * struct VisitChild
     * {
     *   static const bool value = ...; // decide whether to visit Child
     * };
     * \endcode
     *
     * For the two most common scenarios - visiting only direct children and visiting the whole tree - there
     * are mixin classes VisitDirectChildren and VisitTree and combined base classes TreeVisitor and
     * DirectChildrenVisitor. The latter two inherit from both DefaultVisitor and one of the two mixin classes
     * and can thus be used as convenient base classes.
     *
     * \note This class can also be used as a convenient base class if the implemented visitor
     *       only needs to act on some of the possible callback sites, avoiding a lot of boilerplate code.
     */
    struct DefaultVisitor
    {

      //! Method for prefix tree traversal.
      /**
       * This method gets called when first encountering a non-leaf node and
       * before visiting any of its children.
       *
       * \param t        The node to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T, typename TreePath>
      void pre(T&&, TreePath) const {}

      //! Method for infix tree traversal.
      /**
       * This method gets called BETWEEN visits of children of a non-leaf node.
       * That definition implies that this method will only be called for nodes
       * with at least two children.
       *
       * \param t        The node to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T, typename TreePath>
      void in(T&&, TreePath) const {}

      //! Method for postfix tree traversal.
      /**
       * This method gets called after all children of a non-leaf node have
       * been visited.
       *
       * \param t        The node to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T, typename TreePath>
      void post(T&&, TreePath) const {}

      //! Method for leaf traversal.
      /**
       * This method gets called when encountering a leaf node within the TypeTree.
       *
       * \param t        The node to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T, typename TreePath>
      void leaf(T&&, TreePath) const {}

      //! Method for parent-child traversal.
      /**
       * This method gets called before visiting a child node.
       *
       * \note This method gets called even if the visitor decides not to visit the child in question.
       *
       * \param t          The parent node.
       * \param child      The child node that will (potentially) be visited next.
       * \param treePath   The position of the parent node within the TypeTree.
       * \param childIndex The index of the child node in relation to the parent node.
       */
      template<typename T, typename Child, typename TreePath, typename ChildIndex>
      void beforeChild(T&&, Child&&, TreePath, ChildIndex) const {}

      //! Method for child-parent traversal.
      /**
       * This method gets called after visiting a child node.
       *
       * \note This method gets called even if the child node was not visited because the visitor
       *       chose not to do so.
       *
       * \param t          The parent node.
       * \param child      The child node that was visited last (if the visitor did not reject it).
       * \param treePath   The position of the parent node within the TypeTree.
       * \param childIndex The index of the child node in relation to the parent node.
       */
      template<typename T, typename Child, typename TreePath, typename ChildIndex>
      void afterChild(T&&, Child&&, TreePath, ChildIndex) const {}

    };


    //! Visitor interface and base class for visitors of pairs of TypeTrees.
    /**
     * DefaultPairVisitor defines the interface for visitors that can be applied to a pair of TypeTrees
     * using applyToTreePair(). Each method of the visitor is passed a node of both trees (either as
     * a mutable or a const reference, depending on the constness of the tree applyToTreePair() was
     * called with). The last argument is of type TreePath and denotes the exact position of the
     * nodes within the TypeTrees, encoded as child indices starting at the root node.
     *
     * In order to create a functioning visitor, an implementation will - in addition to providing the methods
     * of this class - also have to contain the following template struct, which is used to determine
     * whether to visit a given node:
     *
     * \code
     * template<typename Node1,
     *          typename Child1,
     *          typename Node2,
     *          typename Child2,
     *          typename TreePath>
     * struct VisitChild
     * {
     *   static const bool value = ...; // decide whether to visit Child
     * };
     * \endcode
     *
     * For the two most common scenarios - visiting only direct children and visiting the whole tree - there
     * are mixin classes VisitDirectChildren and VisitTree and combined base classes TreePairVisitor and
     * DirectChildrenPairVisitor. The latter two inherit from both DefaultVisitor and one of the two mixin classes
     * and can thus be used as convenient base classes.
     *
     * \note If your compiler does not support rvalue references, both trees must be either const or
     *       non-const. If you call applyToTreePair() with two trees of different constness, they will
     *       both be made const.
     *
     * \note This class can also be used as a convenient base class if the implemented visitor
     *       only needs to act on some of the possible callback sites, avoiding a lot of boilerplate code.
     */
    struct DefaultPairVisitor
    {

      //! Method for prefix tree traversal.
      /**
       * This method gets called when first encountering a non-leaf node and
       * before visiting any of its children.
       *
       * \param t1       The node of the first tree to visit.
       * \param t2       The node of the second tree to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T1, typename T2, typename TreePath>
      void pre(T1&&, T2&&, TreePath) const {}

      //! Method for infix tree traversal.
      /**
       * This method gets called BETWEEN visits of children of a non-leaf node.
       * That definition implies that this method will only be called for nodes
       * with at least two children.
       *
       * \param t1       The node of the first tree to visit.
       * \param t2       The node of the second tree to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T1, typename T2, typename TreePath>
      void in(T1&&, T2&&, TreePath) const {}

      //! Method for postfix traversal.
      /**
       * This method gets called after all children of a non-leaf node have
       * been visited.
       *
       * \param t1       The node of the first tree to visit.
       * \param t2       The node of the second tree to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T1, typename T2, typename TreePath>
      void post(T1&&, T2&&, TreePath) const {}

      //! Method for leaf traversal.
      /**
       * This method gets called when encountering a leaf node within the pair of TypeTrees.
       *
       * \attention Since the two TypeTrees are not required to be exactly identical,
       *            it is only guaranteed that at least one of the nodes is a leaf node,
       *            not both.
       *
       * \param t1       The node of the first tree to visit.
       * \param t2       The node of the second tree to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T1, typename T2, typename TreePath>
      void leaf(T1&&, T2&&, TreePath) const {}

      //! Method for parent-child traversal.
      /**
       * This method gets called before visiting a child node.
       *
       * \note This method gets called even if the visitor decides not to visit the child in question.
       *
       * \param t1         The node of the first tree to visit.
       * \param child1     The child of t1 to visit.
       * \param t2         The node of the second tree to visit.
       * \param child2     The child of t2 to visit.
       * \param treePath   The position of the parent nodes within the TypeTree.
       * \param childIndex The index of the child nodes in relation to the parent nodes.
       */
      template<typename T1, typename Child1, typename T2, typename Child2, typename TreePath, typename ChildIndex>
      void beforeChild(T1&&, Child1&&, T2&&, Child2&&, TreePath, ChildIndex) const {}

      //! Method for child-parent traversal.
      /**
       * This method gets called after visiting a child node.
       *
       * \note This method gets called even if the visitor decides not to visit the child in question.
       *
       * \param t1         The node of the first tree to visit.
       * \param child1     The child of t1 to visit.
       * \param t2         The node of the second tree to visit.
       * \param child2     The child of t2 to visit.
       * \param treePath   The position of the parent nodes within the TypeTree.
       * \param childIndex The index of the child nodes in relation to the parent nodes.
       */
      template<typename T1, typename Child1, typename T2, typename Child2, typename TreePath, typename ChildIndex>
      void afterChild(T1&&, Child1&&, T2&&, Child2&&, TreePath, ChildIndex) const {}

    };


    namespace Experimental {

      /**
       * @brief Hybrid visitor interface and base class for TypeTree hybrid visitors.
       *
       * DefaultHybridVisitor defines the interface for visitors that can be applied to a TypeTree
       * using hybridApplyToTree(). Each method of the visitor is passed a node of the tree (either as
       * a mutable or a const reference, depending on the constness of the tree hybridApplyToTree() was
       * called with). The second argument is of type TreePath and denotes the exact position of the
       * node within the TypeTree, encoded as child indices starting at the root node.
       *
       * An hybrid visitor is different from a plain visitor because each method receives a carried value
       * (last argument) on the node visit and is required to return a transformed value from it.
       * Transformations of the carried value type are allowed as long as they are expected on the
       * next visited node.
       *
       * In order to create a functioning visitor, an implementation will - in addition to providing the methods
       * of this class - also have to contain the following template struct, which is used to determine
       * whether to visit a given node:
       *
       * \code
       * template<typename Node, typename Child, typename TreePath>
       * struct VisitChild
       * {
       *   static const bool value = ...; // decide whether to visit Child
       * };
       * \endcode
       *
       *
       * \note This class can also be used as a convenient base class if the implemented visitor
       *       only needs to act on some of the possible callback sites, avoiding a lot of boilerplate code.
       */
      struct DefaultHybridVisitor
      {

        /**
         * \copybrief DefaultVisitor::pre
         * \copydetails DefaultVisitor::pre
         *
         * \param u        The carry value from previous visit.
         * \return         The result of applying this visitor to u.
         */
        template<typename T, typename TreePath, typename U>
        auto pre(T&&, TreePath, const U& u) const { return u;}

        /**
         * \copybrief DefaultVisitor::in
         * \copydetails DefaultVisitor::in
         *
         * \param u        The carry value from previous visit.
         * \return         The result of applying this visitor to u.
         */
        template<typename T, typename TreePath, typename U>
        auto in(T&&, TreePath, const U& u) const {return u;}

        /**
         * \copybrief DefaultVisitor::post
         * \copydetails DefaultVisitor::post
         *
         * \param u        The carry value from previous visit.
         * \return         The result of applying this visitor to u.
         */
        template<typename T, typename TreePath, typename U>
        auto post(T&&, TreePath, const U& u) const {return u;}

        /**
         * \copybrief DefaultVisitor::leaf
         * \copydetails DefaultVisitor::leaf
         *
         * \param u        The carry value from previous visit.
         * \return         The result of applying this visitor to u.
         */
        template<typename T, typename TreePath, typename U>
        auto leaf(T&&, TreePath, const U& u) const { return u;}

        /**
         * \copybrief DefaultVisitor::beforeChild
         * \copydetails DefaultVisitor::beforeChild
         *
         * \param u        The carry value from previous visit.
         * \return         The result of applying this visitor to u.
         */
        template<typename T, typename Child, typename TreePath, typename ChildIndex, typename U>
        auto beforeChild(T&&, Child&&, TreePath, ChildIndex, const U& u) const {return u;}

        /**
         * \copybrief DefaultVisitor::afterChild
         * \copydetails DefaultVisitor::afterChild
         *
         * \param u        The carry value from previous visit.
         * \return         The result of applying this visitor to u.
         */
        template<typename T, typename Child, typename TreePath, typename ChildIndex, typename U>
        auto afterChild(T&&, Child&&, TreePath, ChildIndex, const U& u) const {return u;}

      };
    } // namespace Experimental

    //! Mixin base class for visitors that only want to visit the direct children of a node.
    /**
     * This mixin class will reject all children presented to it, causing the algorithm to
     * only visit the root node and call DefaultVisitor::beforeChild() and DefaultVisitor::afterChild()
     * for its direct children.
     */
    struct VisitDirectChildren
    {

      // the little trick with the default template arguments
      // makes the class usable for both single-tree visitors
      // and visitors for pairs of trees
      //! Template struct for determining whether or not to visit a given child.
      template<typename Node1,
               typename Child1,
               typename Node2,
               typename Child2 = void,
               typename TreePath = void>
      struct VisitChild
      {
        //! Do not visit any child.
        static const bool value = false;
      };

    };


    //! Mixin base class for visitors that want to visit the complete tree.
    /**
     * This mixin class will accept all children presented to it and thus make the iterator
     * traverse the entire tree.
     */
    struct VisitTree
    {

      // the little trick with the default template arguments
      // makes the class usable for both single-tree visitors
      // and visitors for pairs of trees
      //! Template struct for determining whether or not to visit a given child.
      template<typename Node1,
               typename Child1,
               typename Node2,
               typename Child2 = void,
               typename TreePath = void>
      struct VisitChild
      {
        //! Visit any child.
        static const bool value = true;
      };

    };

    //! Mixin base class for visitors that require a static TreePath during traversal.
    /**
     * \warning Static traversal should only be used if absolutely necessary, as it tends
     *          to increase compilation times and object sizes (especially if compiling
     *          with debug information)!
     *
     * \sa DynamicTraversal
     */
    struct StaticTraversal
    {
      //! Use the static tree traversal algorithm.
      static const TreePathType::Type treePathType = TreePathType::fullyStatic;
    };

    //! Mixin base class for visitors that only need a dynamic TreePath during traversal.
    /**
     * \note Dynamic traversal is preferable to static traversal, as it causes fewer
     *       template instantiations, which improves compile time and reduces object
     *       size (especially if compiling with debug information).
     *
     * \sa StaticTraversal
     */
    struct DynamicTraversal
    {
      //! Use the dynamic tree traversal algorithm.
      static const TreePathType::Type treePathType = TreePathType::dynamic;
    };

    //! Convenience base class for visiting the entire tree.
    struct TreeVisitor
      : public DefaultVisitor
      , public VisitTree
    {};

    //! Convenience base class for visiting the direct children of a node.
    struct DirectChildrenVisitor
      : public DefaultVisitor
      , public VisitDirectChildren
    {};

    //! Convenience base class for visiting an entire tree pair.
    struct TreePairVisitor
      : public DefaultPairVisitor
      , public VisitTree
    {};

    //! Convenience base class for visiting the direct children of a node pair.
    struct DirectChildrenPairVisitor
      : public DefaultPairVisitor
      , public VisitDirectChildren
    {};

    namespace Experimental::Info {

      struct LeafCounterVisitor
        : public DefaultHybridVisitor
        , public StaticTraversal
        , public VisitTree
      {
        template<class Tree, class Child, class TreePath, class ChildIndex, class U>
        auto beforeChild(Tree&&, Child&&, TreePath, ChildIndex, U u) const {
          // in this case child index is an integral constant: forward u
          return u;
        }

        template<class Tree, class Child, class TreePath, class U>
        std::size_t beforeChild(Tree&&, Child&&, TreePath, std::size_t /*childIndex*/, U u) const {
          // in this case child index is a run-time index: cast accumulated u to std::size_t
          return std::size_t{u};
        }

        template<class Tree, class TreePath, class U>
        auto leaf(Tree&&, TreePath, U u) const
        {
          return Hybrid::plus(u,Dune::Indices::_1);
        }

      };

      struct NodeCounterVisitor
        : public LeafCounterVisitor
      {
        template<typename Tree, typename TreePath, typename U>
        auto pre(Tree&&, TreePath, U u) const {
          return Hybrid::plus(u,Indices::_1);
        }
      };

      struct DepthVisitor
        : public DefaultHybridVisitor
        , public StaticTraversal
        , public VisitTree
      {
        template<class Tree, class TreePath, class U>
        auto leaf(Tree&&, TreePath, U u) const
        {
          auto path_size = index_constant<treePathSize(TreePath{})>{};
          auto depth = Hybrid::plus(path_size,Indices::_1);
          return Hybrid::max(depth,u);
        }
      };

    //! The depth of the TypeTree.
    // result is alwayas an integral constant
      template<typename Tree>
      auto depth(const Tree& tree)
      {
        return hybridApplyToTree(tree,DepthVisitor{},Indices::_0);
      }

      //! The depth of the Tree.
      // return types is std::integral_constant.
      template<typename Tree>
      constexpr auto depth()
      {
        return decltype(hybridApplyToTree(std::declval<Tree>(),DepthVisitor{},Indices::_0)){};
      }

      //! The total number of nodes in the Tree.
      // if Tree is dynamic, return type is std::size_t, otherwise std::integral_constant.
      template<typename Tree>
      auto nodeCount(const Tree& tree)
      {
        return hybridApplyToTree(tree,NodeCounterVisitor{},Indices::_0);
      }

      //! The number of leaf nodes in the Tree.
      // if Tree is dynamic, return type is std::size_t, otherwise std::integral_constant.
      template<typename Tree>
      auto leafCount(const Tree& tree)
      {
        return hybridApplyToTree(tree,LeafCounterVisitor{},Dune::Indices::_0);
      }

      //! true if any of the nodes in the tree only has dynamic degree.
      template<typename Tree>
      constexpr bool isDynamic = std::is_same<std::size_t, decltype(leafCount(std::declval<Tree>()))>{};

    } // namespace Experimental::Info

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_VISITOR_HH
