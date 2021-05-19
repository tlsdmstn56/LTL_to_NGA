#pragma once

#include "ltl/ltl.hpp"

namespace reader
{

/// Read an LTL formula from standard input
/// \return	the parsed formula, or NULL on error
std::shared_ptr<ltl::ltl> read_formula();

} // namespace reader
