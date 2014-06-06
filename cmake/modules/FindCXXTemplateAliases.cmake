# Module that checks whether the compiler supports
# C++11 template aliases.
#
# Sets the following variable:
# HAVE_TEMPLATE_ALIASES
#
# perform tests
include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
  template<typename T, typename U>
  struct A
  {};

  template<typename T>
  using A1 = A<T,int>;

  template<typename U>
  using A2 = A<int,U>;

  template<typename T, typename U>
  struct assert_equal;

  template<typename T>
  struct assert_equal<T,T>
  {};

  int main()
  {
    assert_equal<A1<int>,A2<int> >();
    assert_equal<A<bool,int>,A1<bool> >();
    return 0;
  }"
  HAVE_TEMPLATE_ALIASES)

# Report success correctly in summary
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  "CXXTemplateAliases"
  DEFAULT_MSG
  HAVE_TEMPLATE_ALIASES
)
