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

  { // test the operator== for HybridTreePath

    using Dune::TypeTree::hybridTreePath;

    suite.check(hybridTreePath(1,2,3) != hybridTreePath(1,2));
    suite.check(hybridTreePath(1,2,3) == hybridTreePath(1u,2u,3u));
    suite.check(hybridTreePath(1,2,3) != hybridTreePath(3,2,1));
    suite.check(hybridTreePath(1,2,3) == hybridTreePath(_1,_2,_3));
    suite.check(hybridTreePath(1,2,3) != hybridTreePath(_3,_2,_1));
    suite.check(hybridTreePath(_1,_2,_3) != hybridTreePath(_3,_2,_1));

    // check whether comparison can be used in constexpr context
    static_assert(hybridTreePath(_1,_2,_3) == hybridTreePath(_1,_2,_3));
    static_assert(hybridTreePath(_1,_2,_3) != hybridTreePath(_3,_2,_1));

    auto a = hybridTreePath(std::integral_constant<int,0>{}, std::integral_constant<int,1>{});
    auto b = hybridTreePath(std::integral_constant<std::size_t,0>{}, std::integral_constant<std::size_t,1>{});
    static_assert(decltype(a == b)::value);

    /* Note: It is not possible to check mixed integral constant arguments with
       the purely static overload of operator==

    auto c = hybridTreePath(std::integral_constant<std::size_t,0>{}, std::integral_constant<int,3>{});
    static_assert(decltype(a != c)::value);
    */
  }

  return suite.exit();
}
