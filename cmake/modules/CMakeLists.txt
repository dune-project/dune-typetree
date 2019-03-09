set(modules
  CXXConstexprIf.cmake
  CXXExpansionStatements.cmake
  CXXFoldExpressions.cmake
  DuneTypetreeMacros.cmake
  FindCXXDecltype.cmake
  FindCXXTemplateAliases.cmake
  FindGCCTypeof.cmake
)

install(FILES ${modules} DESTINATION ${DUNE_INSTALL_MODULEDIR})
