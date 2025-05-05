// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception
#include "config.h"

#include <iostream>
#include <type_traits>
#include <cassert>
#include <sstream>

#include <dune/common/version.hh>
#include <dune/common/indices.hh>
#include <dune/common/test/testsuite.hh>

#include <dune/typetree/typetree.hh>

int main(int argc, char** argv)
{

  using namespace Dune::Indices;
  Dune::TestSuite suite("Check HybridTypeTree()");

  {
    auto root = Dune::TypeTree::hybridTreePath();
    constexpr auto path = Dune::TypeTree::hybridTreePath(_1,3,_2,5);

    static_assert(std::is_same<std::decay_t<decltype(path.element(_0))>,std::decay_t<decltype(_1)>>{},"wrong entry value");
    static_assert(path.element(_0) == 1,"wrong entry value");

    static_assert(std::is_same<std::decay_t<decltype(path[_2])>,std::decay_t<decltype(_2)>>{},"wrong entry value");
    static_assert(path[_0] == 1,"wrong entry value");

    static_assert(std::is_same<std::decay_t<decltype(path.element(_1))>,std::size_t>{},"wrong entry type");
    static_assert(std::is_same<std::decay_t<decltype(path.element(_3))>,std::size_t>{},"wrong entry type");
    static_assert(std::is_same<std::decay_t<decltype(path[_1])>,std::size_t>{},"wrong entry type");
    static_assert(std::is_same<std::decay_t<decltype(path[_3])>,std::size_t>{},"wrong entry type");

    suite.check(path.element(_0) == 1);
    suite.check(path.element(_1) == 3);
    suite.check(path.element(_2) == 2);
    suite.check(path.element(_3) == 5);

    suite.check(path.element(1) == 3);
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

    suite.check(back(accumulate_back(path, 3)) == 8);
    suite.check(back(accumulate_back(path, _3)) == 8);
    static_assert(back(accumulate_back(pop_back(path), _3)) == _5);

    suite.check(front(accumulate_front(path, 3)) == 4);
    static_assert(front(accumulate_front(path, _3)) == _4);

    static_assert(pop_front(pop_front(pop_front(pop_front(path)))).size() == 0);
    static_assert(reverse(Dune::TypeTree::hybridTreePath()).size() == 0);

    constexpr auto rpath = reverse(path);
    suite.check(rpath[_0] == 5);
    suite.check(rpath[3] == 1);


    static_assert(join(root, path) == join(path, root));
    constexpr auto jpath = join(path, rpath);
    static_assert(jpath.max_size() == 8);
    static_assert(jpath == Dune::TypeTree::hybridTreePath(_1,3,_2,5,5,_2,3,_1));
    static_assert(join(path, Dune::TypeTree::hybridTreePath(5,_2), Dune::TypeTree::hybridTreePath(3, _1)) == Dune::TypeTree::hybridTreePath(_1,3,_2,5,5,_2,3,_1));
    suite.check(jpath[_3] == 5);
    suite.check(jpath[_4] == 5);
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

  {
    using namespace Dune::TypeTree;

    auto path1 = 1_tp;
    static_assert(path1[_0] == 1);

    auto path123 = 123_tp;
    static_assert(path123[_0] == 123);
  }

  {
    using namespace Dune::TypeTree;
    using namespace Dune::Indices;
    auto tp = Dune::TypeTree::treePath(0, _1, 2, _3);

    auto os = std::stringstream();
    os << tp;
  }

  {
    using namespace Dune::TypeTree;
    using namespace Dune::Indices;
    auto tp = Dune::TypeTree::treePath(0, _1, 2, _3);

    suite.check(tp.get<0>() == 0);
    suite.check(tp.get<1>() == 1);
    suite.check(tp.get<2>() == 2);
    suite.check(tp.get<3>() == 3);

    auto [a, b, c, d] = tp;

    suite.check(a == 0);
    suite.check(b == 1);
    suite.check(c == 2);
    suite.check(d == 3);

    suite.check(std::is_same_v<decltype(a), std::size_t>);
    suite.check(std::is_same_v<decltype(b), Dune::index_constant<1>>);
    suite.check(std::is_same_v<decltype(c), std::size_t>);
    suite.check(std::is_same_v<decltype(d), Dune::index_constant<3>>);
  }

  return suite.exit();
}
