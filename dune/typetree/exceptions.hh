// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception
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
