# Module that checks whether the compiler supports
# the C++17 fold expressions.
#
# Sets the following variable:
# DUNE_HAVE_CXX_FOLD_EXPRESSIONS
#
# perform tests
include(CheckCXXSourceCompiles)

# fold expressions (a + ...)
check_cxx_source_compiles("
  template <class... Args>
  int f(Args... args)
  {
    return (args + ...);
  }
  int main()
  {
    f(0,1,2,3,4,5);
  }
"  DUNE_HAVE_CXX_FOLD_EXPRESSIONS
)
