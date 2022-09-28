// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

#ifndef DUNE_TYPETREE_NODETAGS_HH
#define DUNE_TYPETREE_NODETAGS_HH

namespace Dune {
  namespace TypeTree {

    /** \addtogroup Nodes
     *  \ingroup TypeTree
     *  \{
     */

    //! Tag designating a leaf node.
    struct LeafNodeTag {};

    //! Tag designating a power node.
    struct PowerNodeTag {};

    //! Tag designating a power node with runtime degree.
    struct DynamicPowerNodeTag {};

    //! Tag designating a composite node.
    struct CompositeNodeTag {};

#ifndef DOXYGEN

    //! Special tag used as start value in algorithms.
    struct StartTag {};



#endif // DOXYGEN

    //! \} group Nodes

  } // namespace TypeTree
} //namespace Dune

#endif // DUNE_TYPETREE_NODETAGS_HH
