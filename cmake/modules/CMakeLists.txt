set(modules
  DuneTypetreeMacros.cmake
  FindCXXDecltype.cmake
  FindCXXTemplateAliases.cmake
  FindGCCTypeof.cmake
)

install(FILES ${modules} DESTINATION ${DUNE_INSTALL_MODULEDIR})
