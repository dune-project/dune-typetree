#include "config.h"

#include <utility>

#include <dune/common/tuplevector.hh>
#include <dune/common/rangeutilities.hh>

#include <dune/typetree/utility.hh>
#include <dune/typetree/transformationutilities.hh>

#include "typetreetestswitch.hh"

#if TEST_TYPETREE_INVALID

int main()
{
  return 0;
}

#else

#include "typetreetestutility.hh"
#include "typetreetargetnodes.hh"



// This small helper may or may not be suited in typetraits
template<class Node, class Tag>
constexpr bool hasImplementationTag()
{
  return std::is_same<typename std::decay_t<Node>::ImplementationTag, Tag>::value;
}


// This implements a transformation where construction of PowerNodes
// and CompositeNodes is delegated to helper functions relying on a
// certain interface (template parameters and constructor). This is an
// analouge of using GenericPowerNodeTransformation and GenericCompositeNodeTransformation.
//
// Notice that the code below is not much longer than the declaration
// defining the transformation in typetreetargetnodes.hh. In contrast
// to this one it has IMHO several advantages:
//
// * More readable, easier to understand because you directly see what's happening.
//   That's not quite the case for the declarative style to define transformations.
// * Easily customizable. Since you write the transformation directly instead
//   of a declaration hooking into some abstract mechanism, you can easily adjust
//   parts of the code to your needs. E.g. instead of hard-wiring the customization
//   by tags into the interface you can switch overloads using whatever condition
//   you want.
// * Don't clutter the namespace by 'registering' transformation declarations.
//
// A fully custom transformation not relying on the Generic* interface is
// given below.
struct GenericBasedTransformation
{
  auto operator()(const SimpleLeaf& node) {
    return TargetLeaf(node, *this);
  }

  template<class Node,
    std::enable_if_t<hasImplementationTag<Node, SimplePowerTag>(), int> = 0>
  auto operator()(const Node& node) {
    return Dune::TypeTree::genericPowerNodeTransformation<TargetPower>(node, *this);
  }

  template<class Node,
    std::enable_if_t<hasImplementationTag<Node, SimpleCompositeTag>(), int> = 0>
  auto operator()(const Node& node) {
    return Dune::TypeTree::genericCompositeNodeTransformation<TargetComposite>(node, *this);
  }
};



// This implements a fully custom transformation, i.e., it calls
// PowerNode and CompositeNode constructors manually. This allows
// to use custoim constructor syntax.
struct FullyCustomTransformation
{

  auto operator()(const SimpleLeaf& node)
  {
    return TargetLeaf(node, *this);
  }

  template<class Node,
    std::enable_if_t<hasImplementationTag<Node, SimplePowerTag>(), int> = 0>
  auto operator()(const Node& node)
  {
    using TransformedChild = decltype((*this)(node.child(0)));
    using TransformedNode = TargetPower<Node, TransformedChild, Node::degree()>;
    return TransformedNode(node, *this, transformPowerNodeChildren(node, *this));
  }

  template<class Node,
    std::enable_if_t<hasImplementationTag<Node, SimpleCompositeTag>(), int> = 0>
  auto operator()(const Node& node)
  {
    return Dune::Std::apply([&](auto... transformedChildren) {
        using TransformedNode = TargetComposite<Node, typename decltype(transformedChildren)::element_type...>;
        return TransformedNode(node, *this, transformedChildren...);
      }, transformCompositeNodeChildren(node, *this));
  }
};






int main(int argc, char** argv)
{

  // basic tests

  // leaf node
  TreePrinter treePrinter;
  SimpleLeaf sl1;

  Dune::TypeTree::applyToTree(sl1,treePrinter);

  Dune::TypeTree::TransformTree<SimpleLeaf,TestTransformation>::transformed_type tl1 =
    Dune::TypeTree::TransformTree<SimpleLeaf,TestTransformation>::transform(sl1,TestTransformation());

  typedef SimplePower<SimpleLeaf,3> SP1;
  SP1 sp1_1;
  sp1_1.setChild(0,sl1);
  sp1_1.setChild(1,sl1);
  sp1_1.setChild(2,sl1);

  SimpleLeaf sl2;
  SP1 sp1_2(sl2,false);

  typedef SimpleComposite<SimpleLeafDerived,SP1,SimpleLeaf> SVC1;

  SVC1 svc1_1(SimpleLeafDerived(),sp1_2,sl1);

  Dune::TypeTree::applyToTree(sp1_1,TreePrinter());

  TestTransformation trafo;

  Dune::TypeTree::TransformTree<SP1,TestTransformation>::transformed_type tp1_1 =
    Dune::TypeTree::TransformTree<SP1,TestTransformation>::transform(sp1_1,trafo);

  GenericBasedTransformation transformation;
  auto tp1_1_x = transformation(sp1_1);

  std::cout << std::is_same<decltype(tp1_1), decltype(tp1_1_x)>::value << std::endl;

  FullyCustomTransformation customTransformation;
  auto tp1_1_y = customTransformation(sp1_1);

  std::cout << std::is_same<decltype(tp1_1), decltype(tp1_1_y)>::value << std::endl;

  Dune::TypeTree::TransformTree<SVC1,TestTransformation>::transformed_type tvc1_1 =
    Dune::TypeTree::TransformTree<SVC1,TestTransformation>::transform(svc1_1,TestTransformation());

  auto tvc1_1_x = transformation(svc1_1);

  std::cout << std::is_same<decltype(tvc1_1), decltype(tvc1_1_x)>::value << std::endl;


  Dune::TypeTree::applyToTree(svc1_1,TreePrinter());
  Dune::TypeTree::applyToTree(tvc1_1,TreePrinter());
  Dune::TypeTree::applyToTree(tvc1_1_x,TreePrinter());

  return 0;
}

#endif
