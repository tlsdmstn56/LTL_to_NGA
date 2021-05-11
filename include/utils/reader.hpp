#pragma once

#include "ltl/ltl.hpp"

namespace reader
{

/// Read an LTL formula from standard input
/// \return	the parsed formula, or NULL on error
std::shared_ptr<ltl::ltl> read_formula()
{
    int ch;
    while ((ch = getchar ()) == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v' || ch == '\f');
    switch (ch)
    {
        case 't':
            return ltl::ltl_one::construct();
        case 'p':
            // FIXME: another approach
            if (1 == scanf("%u", &ch))
                return ltl::ltl_atom::construct(ch);
            // TODO: log
            std::cout << "Error in proposition number\n";
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
            // TODO: log
            std::cout << "ERROR: unexpected end of file while parsing formula\n";
            return nullptr;
        default:
            // TODO: log
            std::cout << "unknown character \'" << ch << "\'\n";
            return nullptr;
    }

    return nullptr;
}

} // namespace reader
