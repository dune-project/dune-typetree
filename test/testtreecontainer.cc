#include "config.h"

#include "typetreetestutility.hh"


#include <dune/typetree/traversal.hh>
#include <dune/typetree/treecontainer.hh>

#include <dune/common/test/testsuite.hh>


struct NameVisitor
  : public Dune::TypeTree::TreeVisitor
  , public Dune::TypeTree::DynamicTraversal
{
  std::string s;

  template<typename T, typename TreePath>
  void pre(const T& node, TreePath) { s += node.name(); s += "<"; }

  template<typename T, typename TreePath>
  void leaf(const T& node, TreePath) { s += node.name(); s += ","; }

  template<typename T, typename TreePath>
  void post(const T& node, TreePath) { s += ">"; }
};

template<class Tree>
std::string treeName(const Tree& tree)
{
  NameVisitor nameVisitor;
  Dune::TypeTree::applyToTree(tree, nameVisitor);
  return nameVisitor.s;
}

template<class F>
bool notThrown(F&& f)
{
  try {
    f();
    return true;
  }
  catch(...) {}
  return false;
}

template <class Value, class Tree>
using UniformTreeMatrix
  = Dune::TypeTree::UniformTreeContainer<
      Dune::TypeTree::UniformTreeContainer<Value,Tree>,Tree>;

template<class Tree, class Value>
Dune::TestSuite checkTreeContainer(const Tree& tree, const Value& value)
{
  Dune::TestSuite test(treeName(tree));

  // construct a container using a factory function
  auto container = Dune::TypeTree::makeTreeContainer<Value>(tree);

  // copy construct the container
  auto container2{container};
  auto container3{container};

  // copy-assign the container
  container2 = container;

  // move-construct the container
  auto container4{std::move(container2)};

  // move-assign the container
  container4 = std::move(container3);

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& node, auto treePath) {
      test.check(notThrown([&]() {
        container[treePath] = value;
      })) << "Assigning desired value to tree container entry failed";
    });

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& node, auto treePath) {
      test.check(container[treePath] == value)
        << "Value in tree container does not match assigned value";
    });

  // default construct a container
  decltype(container) container5{};
  container5.resize(tree);

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& node, auto treePath) {
      test.check(notThrown([&]() {
        container5[treePath] = value;
      })) << "Assigning desired value to tree container entry failed";
    });

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& node, auto treePath) {
      test.check(container5[treePath] == value)
        << "Value in tree container does not match assigned value";
    });


  // default construct a container with size information from tree
  decltype(container) container6{tree};

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& node, auto treePath) {
      test.check(notThrown([&]() {
        container6[treePath] = value;
      })) << "Assigning desired value to tree container entry failed";
    });

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& node, auto treePath) {
      test.check(container6[treePath] == value)
        << "Value in tree container does not match assigned value";
    });


  // construct a matrix-like container
  auto matrix = Dune::TypeTree::makeTreeContainer(tree,
    [&](auto const&) { return Dune::TypeTree::makeTreeContainer<Value>(tree); });

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& rowNode, auto rowTreePath) {
    Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& colNode, auto colTreePath) {
      test.check(notThrown([&]() {
        matrix[rowTreePath][colTreePath] = value;
      })) << "Assigning desired value to tree matrix-container entry failed";
    });
  });

  Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& rowNode, auto rowTreePath) {
    Dune::TypeTree::forEachLeafNode(tree, [&] (auto&& colNode, auto colTreePath) {
      test.check(matrix[rowTreePath][colTreePath] == value)
        << "Value in tree matrix-container does not match assigned value";
    });
  });

  return test;
}



int main(int argc, char** argv)
{

  Dune::TestSuite test;

  int v1 = 42;
  std::vector<double> v2{1,2,3,4};

  using SL1 = SimpleLeaf;
  SL1 sl1;
  test.subTest(checkTreeContainer(sl1, v1));
  test.subTest(checkTreeContainer(sl1, v2));

  using SP1 = SimplePower<SimpleLeaf,3>;
  SP1 sp1(sl1, sl1, sl1);
  test.subTest(checkTreeContainer(sp1, v1));
  test.subTest(checkTreeContainer(sp1, v2));

  using SL2 = SimpleLeaf;
  using SP2 = SimplePower<SimpleLeaf,2>;
  using SC1 = SimpleComposite<SL1,SP1,SP2>;
  SL2 sl2;
  SP2 sp2(sl2,sl2);
  SC1 sc1_1(sl1,sp1,sp2);
  test.subTest(checkTreeContainer(sc1_1, v1));
  test.subTest(checkTreeContainer(sc1_1, v2));

  test.report();

  return test.exit();
}
