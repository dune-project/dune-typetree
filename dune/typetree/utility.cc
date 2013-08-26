#include "config.h"

#include <dune/typetree/utility.hh>

namespace Dune {
  namespace TypeTree {

    namespace {
      static const shared_ptr<EmptyNode> _emptyNodePtr(make_shared<EmptyNode>());
    }

    const shared_ptr<EmptyNode>& emptyNodePtr()
    {
      return _emptyNodePtr;
    }

  } // namespace TypeTree
} // namespace Dune
