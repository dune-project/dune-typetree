# Module that checks whether the compiler supports
# the C++17 constexpr if.
#
# Sets the following variable:
# DUNE_HAVE_CXX_CONSTEXPR_IF
#
# perform tests
include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
  template <int I>
  auto f()
  {
    if constexpr(I == 0)
      return double{1.0};
    else
      return int{0};
  }
  int main()
  {
    return f<1>();
  }
"  DUNE_HAVE_CXX_CONSTEXPR_IF
)
