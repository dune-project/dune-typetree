// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifndef DUNE_TYPETREE_TRAVERSAL_HH
#define DUNE_TYPETREE_TRAVERSAL_HH

#if HAVE_RVALUE_REFERENCES
#include <utility>
#endif

#include <dune/typetree/nodetags.hh>
#include <dune/typetree/treepath.hh>
#include <dune/typetree/visitor.hh>
#include <dune/typetree/applytochildrensingletree.hh>

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Tree Traversal
     *  \ingroup TypeTree
     *  \{
     */


#ifndef DOXYGEN // these are all internals and not public API. Only access is using applyToTree().


    // This struct is the core of the algorithm. While this specialization simply serves as the starting point
    // of the traversal and takes care of some setup work, the struct has to be specialized for each TreeType node type it
    // should support.
    // The first parameter specifies the kind of TreePath (dynamic/static) to use, the second one is the tag of the node type
    // and the third one must always be specialized as true, as a value of false means the node should in fact not be visited.
    // That case is already handled by a specialization of the struct.
    template<TreePathType::Type tpType, bool doApply>
    struct ApplyToTree<tpType,StartTag,doApply>
    {

      template<typename Node, typename Visitor>
      static void apply(Node&& node, Visitor&& visitor)
      {
        ApplyToTree<tpType,NodeTag<Node>>::apply(std::forward<Node>(node),
                                                 std::forward<Visitor>(visitor),
                                                 TreePathFactory<tpType>::create(node).mutablePath());
      }

    };


    // Do not visit nodes the visitor is not interested in
    template<TreePathType::Type tpType, typename NodeTag>
    struct ApplyToTree<tpType,NodeTag,false>
    {

      // we won't do anything with the objects, so having them all const
      // works fine.
      template<typename Node, typename Visitor, typename TreePath>
      static void apply(const Node& node, const Visitor& visitor, TreePath treePath)
      {}

    };



    // ********************************************************************************
    // LeafNode
    // ********************************************************************************

    // LeafNode - just call the leaf() callback
    template<TreePathType::Type tpType>
    struct ApplyToTree<tpType,LeafNodeTag,true>
    {

      template<typename N, typename V, typename TreePath>
      static void apply(N&& n, V&& v, TreePath tp)
      {
        v.leaf(std::forward<N>(n),tp.view());
      }

    };



    // ********************************************************************************
    // PowerNode
    // ********************************************************************************

    // Traverse PowerNode statically - in this case, we simply use the
    // generic child traversal algorithm
    template<>
    struct ApplyToTree<TreePathType::fullyStatic,PowerNodeTag,true>
      : public ApplyToGenericCompositeNode
    {
    };

    // Traverse PowerNode dynamically. Here, we exploit the fact that is possible
    // to do the child traversal using runtime iteration, as that saves a lot of
    // template instantiations.
    template<>
    struct ApplyToTree<TreePathType::dynamic,PowerNodeTag,true>
    {

      template<typename N, typename V, typename TreePath>
      static void apply(N&& n, V&& v, TreePath tp)
      {
        // first encounter of this node
        v.pre(std::forward<N>(n),tp.view());

        // strip types of possible references
        typedef typename std::remove_reference<N>::type Node;
        typedef typename std::remove_reference<V>::type Visitor;

        // get child type
        typedef typename Node::template Child<0>::Type C;

        // Do we have to visit the children? As the TreePath is dynamic, it does not
        // contain any information that could be evaluated at compile time, so we only
        // have to query the visitor once.
        const bool visit = Visitor::template VisitChild<Node,C,typename TreePath::ViewType>::value;

        // iterate over children
        for (std::size_t k = 0; k < degree(n); ++k)
          {
            // always call beforeChild(), regardless of the value of visit
            v.beforeChild(std::forward<N>(n),n.child(k),tp.view(),k);

            // update TreePath
            tp.push_back(k);

            // descend to child
            ApplyToTree<Visitor::treePathType,NodeTag<C>,visit>::apply(n.child(k),std::forward<V>(v),tp);

            // restore TreePath
            tp.pop_back();

            // always call afterChild(), regardless of the value of visit
            v.afterChild(std::forward<N>(n),n.child(k),tp.view(),k);

            // if this is not the last child, call infix callback
            if (k < degree(n) - 1)
              v.in(std::forward<N>(n),tp.view());
          }

        // node is done - call postfix callback
        v.post(std::forward<N>(n),tp.view());
      }

    };



    // ********************************************************************************
    // CompositeNode
    // ********************************************************************************

    // Traverse CompositeNode - just forward to the generic algorithm
    template<TreePathType::Type treePathType>
    struct ApplyToTree<treePathType,CompositeNodeTag,true>
      : public ApplyToGenericCompositeNode
    {
    };

#endif // DOXYGEN

    namespace Detail {

      template<class PreFunc, class LeafFunc, class PostFunc>
      struct CallbackVisitor :
        public TypeTree::TreeVisitor,
        public TypeTree::DynamicTraversal
      {
        public:
        CallbackVisitor(PreFunc& preFunc, LeafFunc& leafFunc, PostFunc& postFunc) :
          preFunc_(preFunc),
          leafFunc_(leafFunc),
          postFunc_(postFunc)
        {}

        template<typename Node, typename TreePath>
        void pre(Node&& node, TreePath treePath)
        {
          preFunc_(node, treePath);
        }

        template<typename Node, typename TreePath>
        void leaf(Node&& node, TreePath treePath)
        {
          leafFunc_(node, treePath);
        }

        template<typename Node, typename TreePath>
        void post(Node&& node, TreePath treePath)
        {
          postFunc_(node, treePath);
        }

      private:
        PreFunc& preFunc_;
        LeafFunc& leafFunc_;
        PostFunc& postFunc_;
      };

      template<class PreFunc, class LeafFunc, class PostFunc>
      auto callbackVisitor(PreFunc& preFunc, LeafFunc& leafFunc, PostFunc& postFunc)
      {
        return CallbackVisitor<PreFunc, LeafFunc, PostFunc>(preFunc, leafFunc, postFunc);
      }

    } // namespace Detail


    // ********************************************************************************
    // Public Interface
    // ********************************************************************************

    //! Apply visitor to TypeTree.
    /**
     * \code
     #include <dune/typetree/traversal.hh>
     * \endcode
     * This function applies the given visitor to the given tree. Both visitor and tree may be const
     * or non-const (if the compiler supports rvalue references, they may even be a non-const temporary).
     *
     * \note The visitor must implement the interface laid out by DefaultVisitor (most easily achieved by
     *       inheriting from it) and specify the required type of tree traversal (static or dynamic) by
     *       inheriting from either StaticTraversal or DynamicTraversal.
     *
     * \param tree    The tree the visitor will be applied to.
     * \param visitor The visitor to apply to the tree.
     */
    template<typename Tree, typename Visitor>
    void applyToTree(Tree&& tree, Visitor&& visitor)
    {
      ApplyToTree<std::remove_reference<Visitor>::type::treePathType>::apply(std::forward<Tree>(tree),
                                                                             std::forward<Visitor>(visitor));
    }

    /**
     * \brief Traverse tree and visit each node
     *
     * All passed callback functions are called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param preFunc This function is called for each inner node before visiting its children
     * \param leafFunc This function is called for each leaf node
     * \param postFunc This function is called for each inner node after visiting its children
     */
    template<class Tree, class PreFunc, class LeafFunc, class PostFunc>
    void forEachNode(Tree&& tree, PreFunc&& preFunc, LeafFunc&& leafFunc, PostFunc&& postFunc)
    {
      applyToTree(tree, Detail::callbackVisitor(preFunc, leafFunc, postFunc));
    }

    /**
     * \brief Traverse tree and visit each node
     *
     * All passed callback functions are called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param innerFunc This function is called for each inner node before visiting its children
     * \param leafFunc This function is called for each leaf node
     */
    template<class Tree, class InnerFunc, class LeafFunc>
    void forEachNode(Tree&& tree, InnerFunc&& innerFunc, LeafFunc&& leafFunc)
    {
      auto nop = [](auto&&... args) {};
      forEachNode(tree, innerFunc, leafFunc, nop);
    }

    /**
     * \brief Traverse tree and visit each node
     *
     * The passed callback function is called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param nodeFunc This function is called for each node
     */
    template<class Tree, class NodeFunc>
    void forEachNode(Tree&& tree, NodeFunc&& nodeFunc)
    {
      forEachNode(tree, nodeFunc, nodeFunc);
    }

    /**
     * \brief Traverse tree and visit each leaf node
     *
     * The passed callback function is called with the
     * node and corresponding treepath as arguments.
     *
     * \param tree The tree to traverse
     * \param leafFunc This function is called for each leaf node
     */
    template<class Tree, class LeafFunc>
    void forEachLeafNode(Tree&& tree, LeafFunc&& leafFunc)
    {
      auto nop = [](auto&&... args) {};
      forEachNode(tree, nop, leafFunc, nop);
    }

    //! \} group Tree Traversal

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_TRAVERSAL_HH
