#include "config.h"

#include "typetreetestswitch.hh"

#if TEST_TYPETREE_INVALID

int main()
{
  return 0;
}

#else

#include "typetreetestutility.hh"
#include "typetreetargetnodes.hh"

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

  Dune::TypeTree::TransformTree<SVC1,TestTransformation>::transformed_type tvc1_1 =
    Dune::TypeTree::TransformTree<SVC1,TestTransformation>::transform(svc1_1,TestTransformation());

  Dune::TypeTree::applyToTree(tvc1_1,TreePrinter());

  return 0;
}

#endif
