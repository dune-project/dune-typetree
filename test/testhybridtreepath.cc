#include "config.h"

#include <iostream>
#include <type_traits>
#include <cassert>

#include <dune/typetree/typetree.hh>

int main(int argc, char** argv)
{

  using namespace Dune::Indices;

  {

    constexpr auto path = Dune::TypeTree::hybridTreePath(_1,3,_2,5);

    static_assert(std::is_same_v<path.element(_0),_1,"wrong entry value");
    static_assert(path.element(_0) == 1,"wrong entry value");
    static_assert(path.element(2) == 2,"wrong entry value");
    static_assert(path.element(3) == 5,"wrong entry value");

    static_assert(std::is_same_v<path[_2],_2,"wrong entry value");
    static_assert(path[_0] == 1,"wrong entry value");
    static_assert(path[2] == 2,"wrong entry value");

    assert(path.element(_0) == 1);
    assert(path.element(3) == 5);

    assert(path[_0] == 1);
    assert(path[3] == 5);

  }

  return 0;
}
