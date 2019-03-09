/* begin dune-typetree
   put the definitions for config.h specific to
   your project here. Everything above will be
   overwritten
*/
/* begin private */
/* Name of package */
#define PACKAGE "@DUNE_MOD_NAME@"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "@DUNE_MAINTAINER@"

/* Define to the full name of this package. */
#define PACKAGE_NAME "@DUNE_MOD_NAME@"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "@DUNE_MOD_NAME@ @DUNE_MOD_VERSION@"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "@DUNE_MOD_NAME@"

/* Define to the home page for this package. */
#define PACKAGE_URL "@DUNE_MOD_URL@"

/* Define to the version of this package. */
#define PACKAGE_VERSION "@DUNE_MOD_VERSION@"

/* end private */

/* Define to the version of dune-typetree */
#define DUNE_TYPETREE_VERSION "${DUNE_TYPETREE_VERSION}"

/* Define to the major version of dune-typetree */
#define DUNE_TYPETREE_VERSION_MAJOR ${DUNE_TYPETREE_VERSION_MAJOR}

/* Define to the minor version of dune-typetree */
#define DUNE_TYPETREE_VERSION_MINOR ${DUNE_TYPETREE_VERSION_MINOR}

/* Define to the revision of dune-typetree */
#define DUNE_TYPETREE_VERSION_REVISION ${DUNE_TYPETREE_VERSION_REVISION}

/* some detected compiler features may be used in dune-typree */
#cmakedefine DUNE_HAVE_CXX_FOLD_EXPRESSIONS 1
#cmakedefine DUNE_HAVE_CXX_CONSTEXPR_IF 1
#cmakedefine DUNE_HAVE_EXPANSION_STATEMENTS 1

/* end dune-typetree */
