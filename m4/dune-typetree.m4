# Additional checks needed to build the module
AC_DEFUN([DUNE_TYPETREE_CHECKS],
[
  AC_REQUIRE([TEMPLATE_ALIASES_CHECK])
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
