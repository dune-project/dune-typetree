# Module that checks whether the compiler supports
# the C++20 tuple expansion statements.
#
# Sets the following variable:
# DUNE_HAVE_EXPANSION_STATEMENTS
#
# perform tests
include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
  #include <iostream>
  #include <tuple>
  int main()
  {
    auto tup = std::make_tuple(0, ‘a’, 3.14);
    for... (auto elem : tup)
      std::cout << elem << std::endl;
  }
"  DUNE_HAVE_EXPANSION_STATEMENTS
)
