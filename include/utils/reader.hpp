#pragma once

#include "ltl/ltl.hpp"

namespace reader
{

/// Read an LTL formula from standard input
/// \return	the parsed formula, or NULL on error
std::shared_ptr<ltl::ltl> read_formula()
{
    int ch;
    while ((ch = getchar()) == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v' || ch == '\f');
    switch (ch)
    {
        case 't':
            return ltl::ltl_one::construct();
        case 'p':
            // TODO: another approach
            if (scanf("%u", &ch) == 1)
                return ltl::ltl_atom::construct(ch);
            assert(!"Error in proposition number");
            return nullptr;
        case '!':
            return ltl::ltl_negation::construct(read_formula());
        case '^':
            return ltl::ltl_conjunction::construct(read_formula(), read_formula());
        case 'X':
            return ltl::ltl_next::construct(read_formula());
        case 'U':
            return ltl::ltl_until::construct(read_formula(), read_formula());
        case EOF:
            assert(!"ERROR: unexpected end of file while parsing formula");
            return nullptr;
        default:
            assert(!"unknown character" && static_cast<char>(ch));
            return nullptr;
    }

    return nullptr;
}

} // namespace reader
