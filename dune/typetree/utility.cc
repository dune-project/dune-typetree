#include "config.h"

#include <dune/typetree/utility.hh>

namespace Dune {
  namespace TypeTree {

    namespace {
      static const std::shared_ptr<EmptyNode> _emptyNodePtr(std::make_shared<EmptyNode>());
    }

    const std::shared_ptr<EmptyNode>& emptyNodePtr()
    {
      return _emptyNodePtr;
    }

  } // namespace TypeTree
} // namespace Dune
