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

/* Define to 1 if std::initializer_list is supported. */
#cmakedefine HAVE_INITIALIZER_LIST 1

/* Define to 1 if template aliases are supported. */
#cmakedefine HAVE_TEMPLATE_ALIASES 1

/* Define to 1 if decltype if supported. */
#cmakedefine HAVE_STD_DECLTYPE 1

/* Define to 1 if GCC's __typeof__ extension is supported. */
#cmakedefine HAVE_GCC___TYPEOF__ 1

/* end dune-typetree */
