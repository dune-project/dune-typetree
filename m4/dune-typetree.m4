# Additional checks needed to build the module
AC_DEFUN([DUNE_TYPETREE_CHECKS],
[
  AC_REQUIRE([RVALUE_REFERENCES_CHECK])
  AC_REQUIRE([VARIADIC_TEMPLATES_CHECK])
  AC_REQUIRE([VARIADIC_CONSTRUCTOR_SFINAE_CHECK])
  AC_REQUIRE([TEMPLATE_ALIASES_CHECK])
  AC_REQUIRE([INITIALIZER_LIST_CHECK])
  AC_REQUIRE([CXX11_DECLTYPE_CHECK])
  AC_REQUIRE([GCC___TYPEOF___CHECK])
])

# Additional checks needed to find the module
AC_DEFUN([DUNE_TYPETREE_CHECK_MODULE],[
  AC_MSG_NOTICE([Searching for dune-typetree...])
  DUNE_CHECK_MODULES([dune-typetree], [typetree/utility.hh],[dnl
    return !Dune::TypeTree::emptyNodePtr();
  ])
])
