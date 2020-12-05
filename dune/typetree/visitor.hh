// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_VISITOR_HH
#define DUNE_TYPETREE_VISITOR_HH

#include <dune/typetree/treepath.hh>

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
      void pre(T&& t, TreePath treePath) const {}

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
      void in(T&& t, TreePath treePath) const {}

      //! Method for postfix tree traversal.
      /**
       * This method gets called after all children of a non-leaf node have
       * been visited.
       *
       * \param t        The node to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T, typename TreePath>
      void post(T&& t, TreePath treePath) const {}

      //! Method for leaf traversal.
      /**
       * This method gets called when encountering a leaf node within the TypeTree.
       *
       * \param t        The node to visit.
       * \param treePath The position of the node within the TypeTree.
       */
      template<typename T, typename TreePath>
      void leaf(T&& t, TreePath treePath) const {}

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
      void beforeChild(T&& t, Child&& child, TreePath treePath, ChildIndex childIndex) const {}

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
      void afterChild(T&& t, Child&& child, TreePath treePath, ChildIndex childIndex) const {}

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
      void pre(T1&& t1, T2&& t2, TreePath treePath) const {}

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
      void in(T1&& t1, T2&& t2, TreePath treePath) const {}

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
      void post(T1&& t1, T2&& t2, TreePath treePath) const {}

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
      void leaf(T1&& t1, T2&& t2, TreePath treePath) const {}

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
      void beforeChild(T1&& t1, Child1&& child1, T2&& t2, Child2&& child2, TreePath treePath, ChildIndex childIndex) const {}

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
      void afterChild(T1&& t1, Child1&& child1, T2&& t2, Child2&& child2, TreePath treePath, ChildIndex childIndex) const {}

    };

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


    namespace Detail {

      // This class tags a callback
      template<class T, class F>
      struct TaggedCallback : public F
      {
        TaggedCallback(F&& f) : F(f) {}
      };

      // Create a version of f that is tagged by T.
      // This will store a copy of f.
      template<class T, class F>
      auto taggedCallback(F&& f) {
        return TaggedCallback<T, std::decay_t<F>>(std::forward<F>(f));
      }

      // Callback tags for the different Visitor methods
      struct PreCallbackTag {};
      struct PostCallbackTag {};
      struct InCallbackTag {};
      struct LeafCallbackTag {};
      struct BeforeChildCallbackTag {};
      struct AfterChildCallbackTag {};

      // A Visitor extending a BaseVisitor by a pre() method
      // implemented by a given callback.  This stores a copy
      // of the callback.
      template<class BaseVisitor, class F>
      struct PreVisitor : public BaseVisitor
      {
        template<typename T, typename TreePath>
        void pre(T&& t, TreePath treePath) const {
          BaseVisitor::pre(t, treePath);
          callBack_(t, treePath);
        }
        F callBack_;
      };

      // Create PreVisitor by piping a BaseVisitor and a tagged callback
      template<class BaseVisitor, class F>
      auto operator|(BaseVisitor&& baseVisitor, TaggedCallback<PreCallbackTag, F>&& pre) {
        return PreVisitor<std::decay_t<BaseVisitor>, F>{std::forward<BaseVisitor>(baseVisitor), std::move(pre)};
      }

      // A Visitor extending a BaseVisitor by a post() method
      // implemented by a given callback.  This stores a copy
      // of the callback.
      template<class BaseVisitor, class F>
      struct PostVisitor : public BaseVisitor
      {
        template<typename T, typename TreePath>
        void post(T&& t, TreePath treePath) const {
          BaseVisitor::post(t, treePath);
          callBack_(t, treePath);
        }
        F callBack_;
      };

      // Create PostVisitor by piping a BaseVisitor and a tagged callback
      template<class BaseVisitor, class F>
      auto operator|(BaseVisitor&& baseVisitor, TaggedCallback<PostCallbackTag, F>&& pre) {
        return PostVisitor<std::decay_t<BaseVisitor>, F>{std::forward<BaseVisitor>(baseVisitor), std::move(pre)};
      }

      // A Visitor extending a BaseVisitor by a in() method
      // implemented by a given callback.  This stores a copy
      // of the callback.
      template<class BaseVisitor, class F>
      struct InVisitor : public BaseVisitor
      {
        template<typename T, typename TreePath>
        void in(T&& t, TreePath treePath) const {
          BaseVisitor::in(t, treePath);
          callBack_(t, treePath);
        }
        F callBack_;
      };

      // Create InVisitor by piping a BaseVisitor and a tagged callback
      template<class BaseVisitor, class F>
      auto operator|(BaseVisitor&& baseVisitor, TaggedCallback<InCallbackTag, F>&& pre) {
        return InVisitor<std::decay_t<BaseVisitor>, F>{std::forward<BaseVisitor>(baseVisitor), std::move(pre)};
      }

      // A Visitor extending a BaseVisitor by a leaf() method
      // implemented by a given callback.  This stores a copy
      // of the callback.
      template<class BaseVisitor, class F>
      struct LeafVisitor : public BaseVisitor
      {
        template<typename T, typename TreePath>
        void leaf(T&& t, TreePath treePath) const {
          BaseVisitor::leaf(t, treePath);
          callBack_(t, treePath);
        }
        F callBack_;
      };

      // Create LeafVisitor by piping a BaseVisitor and a tagged callback
      template<class BaseVisitor, class F>
      auto operator|(BaseVisitor&& baseVisitor, TaggedCallback<LeafCallbackTag, F>&& pre) {
        return LeafVisitor<std::decay_t<BaseVisitor>, F>{std::forward<BaseVisitor>(baseVisitor), std::move(pre)};
      }

      // A Visitor extending a BaseVisitor by a beforeChild() method
      // implemented by a given callback.  This stores a copy
      // of the callback.
      template<class BaseVisitor, class F>
      struct BeforeChildVisitor : public BaseVisitor
      {
        template<typename T, typename Child, typename TreePath, typename ChildIndex>
        void beforeChild(T&& t, Child&& child, TreePath treePath, ChildIndex childIndex) const {
          BaseVisitor::beforeChild(t, child, treePath, childIndex);
          callBack_(t, child, treePath, childIndex);
        }
        F callBack_;
      };

      // Create BeforeChildVisitor by piping a BaseVisitor and a tagged callback
      template<class BaseVisitor, class F>
      auto operator|(BaseVisitor&& baseVisitor, TaggedCallback<BeforeChildCallbackTag, F>&& pre) {
        return BeforeChildVisitor<std::decay_t<BaseVisitor>, F>{std::forward<BaseVisitor>(baseVisitor), std::move(pre)};
      }

      // A Visitor extending a BaseVisitor by a afterChild() method
      // implemented by a given callback.  This stores a copy
      // of the callback.
      template<class BaseVisitor, class F>
      struct AfterChildVisitor : public BaseVisitor
      {
        template<typename T, typename Child, typename TreePath, typename ChildIndex>
        void afterChild(T&& t, Child&& child, TreePath treePath, ChildIndex childIndex) const {
          BaseVisitor::afterChild(t, child, treePath, childIndex);
          callBack_(t, child, treePath, childIndex);
        }
        F callBack_;
      };

      // Create AfterChildVisitor by piping a BaseVisitor and a tagged callback
      template<class BaseVisitor, class F>
      auto operator|(BaseVisitor&& baseVisitor, TaggedCallback<AfterChildCallbackTag, F>&& pre) {
        return AfterChildVisitor<std::decay_t<BaseVisitor>, F>{std::forward<BaseVisitor>(baseVisitor), std::move(pre)};
      }

      // A dummy visitor doing nothing. In contrast to DefaultVisitor
      // this is parameterited by the traversal type
      template<class TreePathType>
      class NOPVisitor : public TreeVisitor, public TreePathType
      {
        public:
      };

    } // namespace Detail

    /**
     * \brief Create a tagged callback for implementing Visitor::pre().
     *
     * This can be chained with an existing Visitor using operator|
     */
    template<class F>
    auto pre(F&& f) {
      return Detail::taggedCallback<Detail::PreCallbackTag>(std::forward<F>(f));
    }

    /**
     * \brief Create a tagged callback for implementing Visitor::post().
     *
     * This can be chained with an existing Visitor using operator|
     */
    template<class F>
    auto post(F&& f) {
      return Detail::taggedCallback<Detail::PostCallbackTag>(std::forward<F>(f));
    }

    /**
     * \brief Create a tagged callback for implementing Visitor::in().
     *
     * This can be chained with an existing Visitor using operator|
     */
    template<class F>
    auto in(F&& f) {
      return Detail::taggedCallback<Detail::InCallbackTag>(std::forward<F>(f));
    }

    /**
     * \brief Create a tagged callback for implementing Visitor::leaf().
     *
     * This can be chained with an existing Visitor using operator|
     */
    template<class F>
    auto leaf(F&& f) {
      return Detail::taggedCallback<Detail::LeafCallbackTag>(std::forward<F>(f));
    }

    /**
     * \brief Create a tagged callback for implementing Visitor::beforeChild().
     *
     * This can be chained with an existing Visitor using operator|
     */
    template<class F>
    auto beforeChild(F&& f) {
      return Detail::taggedCallback<Detail::BeforeChildCallbackTag>(std::forward<F>(f));
    }

    /**
     * \brief Create a tagged callback for implementing Visitor::afterChild().
     *
     * This can be chained with an existing Visitor using operator|
     */
    template<class F>
    auto afterChild(F&& f) {
      return Detail::taggedCallback<Detail::AfterChildCallbackTag>(std::forward<F>(f));
    }

    /**
     * \brief Create a default Visitor doing nothing.
     *
     * This creates a Visitor with the given traversal type that
     * does nothing. The resulting Visitor can be easily extended
     * by piping tagged callbacks obtained by pre(), post(), in(),
     * leaf(), beforeChild(), afterChild() with operator|.
     */
    template<class TraversalType = DynamicTraversal>
    auto makeVisitor() {
      return Detail::NOPVisitor<TraversalType>{};
    }

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_VISITOR_HH
