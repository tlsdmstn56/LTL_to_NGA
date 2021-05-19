#pragma once

#include "ltl/closure.hpp"

namespace dot
{

/// \param simplify: do not print formula inside circles
/// \return first element is a state representation and the second is a dot-language graph
std::pair<std::vector<std::string>, std::string> convert_to_dot(const std::shared_ptr<ltl::converting>& algo);

} // namespace dot
