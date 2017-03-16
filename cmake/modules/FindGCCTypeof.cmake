# Module that checks whether the compiler supports
# the GCC extension __typeof__, which can serve as
# a limited fallback for decltype on some older compilers.
#
# Sets the following variable:
# HAVE_GCC___TYPEOF__
#
# perform tests
include(CheckCXXSourceCompiles)

# deprecation warning
message(DEPRECATION "FindGCCTypeof is deprecated and will be removed after the release of dune-typetree 2.6")

check_cxx_source_compiles("
  template<typename A, typename B>
  struct check_equal;

  template<typename A>
  struct check_equal<A,A>
  {
    static const int result = 0;
  };

  struct A {};

  A foo();

  int main(void){
      return check_equal<__typeof__(foo()),A>::result;
  }"
  HAVE_GCC___TYPEOF__)

# Report success correctly in summary
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  "GCCTypeof"
  DEFAULT_MSG
  HAVE_STD_DECLTYPE
)
