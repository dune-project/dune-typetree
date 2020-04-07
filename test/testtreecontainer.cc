#include "config.h"

#include "typetreetestutility.hh"


#include <dune/typetree/traversal.hh>
#include <dune/typetree/treecontainer.hh>

#include <dune/common/test/testsuite.hh>


template<class Tree>
std::string treeName(const Tree& tree)
{
  std::string s;
  Dune::TypeTree::forEachNode(tree,
      [&](auto&& node, auto&& path){ s += node.name(); s += "<"; },
      [&](auto&& node, auto&& path){ s += node.name(); s += ","; },
      [&](auto&& node, auto&& path){ s += ">"; });
  return s;
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

  // contruct a container using a factory function
  auto container = Dune::TypeTree::makeTreeContainer<Value>(tree);

  // default construct a container
  Dune::TypeTree::UniformTreeContainer<Value,Tree> container2{};

  // copy construct the container
  auto container3{container};

  // copy-assign the container
  container3 = container;

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

  // construct a matrix-like container
  auto matrix = Dune::TypeTree::makeTreeContainer(tree,
    [&](auto const&) { return Dune::TypeTree::makeTreeContainer<Value>(tree); });

  // default construct matrix-like container
  UniformTreeMatrix<Value,Tree> matrix2{};

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
