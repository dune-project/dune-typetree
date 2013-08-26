// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_TYPETREE_EXCEPTIONS_HH
#define DUNE_TYPETREE_EXCEPTIONS_HH

#include <dune/common/exceptions.hh>

/**
 * \file
 * \brief TypeTree-specific exceptions.
 */

namespace Dune {
  namespace TypeTree {

    //! Base class for all TypeTree exceptions.
    class Exception
      : public Dune::Exception
    {};

  } // namespace TypeTree
} // namespace Dune

#endif // DUNE_TYPETREE_EXCEPTIONS_HH
