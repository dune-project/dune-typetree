set(modules
  DuneTypeTreeMacros.cmake
  FindCXXDecltype.cmake
  FindCXXInitializerList.cmake
  FindCXXTemplateAliases.cmake
  FindGCCTypeof.cmake
  FindPackageMultipass.cmake
  ResolveCompilerPaths.cmake)
install(FILES ${modules} DESTINATION ${DUNE_INSTALL_MODULEDIR})
