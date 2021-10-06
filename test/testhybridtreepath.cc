#include "config.h"

#include <iostream>
#include <type_traits>
#include <cassert>

#include <dune/common/version.hh>
#include <dune/common/test/testsuite.hh>

#include <dune/typetree/typetree.hh>

int main(int argc, char** argv)
{

  using namespace Dune::Indices;
  Dune::TestSuite suite("Check HybridTypeTree()");

  {

    constexpr auto path = Dune::TypeTree::hybridTreePath(_1,3,_2,5);

    static_assert(std::is_same<std::decay_t<decltype(path.element(_0))>,std::decay_t<decltype(_1)>>{},"wrong entry value");
    static_assert(path.element(_0) == 1,"wrong entry value");

    static_assert(std::is_same<std::decay_t<decltype(path[_2])>,std::decay_t<decltype(_2)>>{},"wrong entry value");
    static_assert(path[_0] == 1,"wrong entry value");

    suite.check(path.element(_0) == 1);
    suite.check(path.element(3) == 5);

    suite.check(path[_0] == 1);
    suite.check(path[3] == 5);

    suite.check(back(path) == 5);
    static_assert(back(push_back(path, _3)) == 3);
    suite.check(back(push_back(path, 3)) == 3);
    static_assert(back(pop_back(path)) == 2);
    suite.check(back(pop_back(path)) == 2);
    suite.check(back(pop_back(pop_back(path))) == 3);

    static_assert(front(path) == 1);
    static_assert(front(push_front(path,_0)) == 0);
    suite.check(front(push_front(path,0)) == 0);
    suite.check(front(pop_front(path)) == 3);
    static_assert(front(pop_front(pop_front(path))) == 2);
    suite.check(front(pop_front(pop_front(path))) == 2);

  }

  return suite.exit();
}
