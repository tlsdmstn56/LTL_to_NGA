#pragma once

#include "ltl/ltl.hpp"

namespace reader
{

/// Read an LTL formula from standard input
/// \param negation: negate the formula. Default value is for init run
/// \return	the parsed formula, or NULL on error
std::shared_ptr<ltl::ltl> read_formula(const bool negation = false)
{
    while (true)
    {
        switch (int ch = getchar())
        {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
            case '\f':
                break;
            case 't':
            case 'f':
                return ltl::ltl_constant::construct((ch == 't') == negation);
            case 'p':
                // FIXME: another approach
                if (1 == scanf("%u", &ch))
                    return ltl::ltl_atom::construct(negation, ch);
                // TODO: log
                std::cout << "Error in proposition number\n";
                return nullptr;
            case '!':
                return read_formula(!negation);
            case '&':
            case '|':
                return ltl::ltl_junct::construct((ch == '&') == negation, read_formula(negation),
                                                 read_formula(negation));
            case 'X':
                return ltl::ltl_next::construct(read_formula(negation));
            case 'U':
            case 'R':
                return ltl::ltl_until::construct((ch == 'U') == negation, read_formula(negation),
                                                 read_formula(negation));
            case EOF:
                // TODO: log
                std::cout << "ERROR: unexpected end of file while parsing formula\n";
                return nullptr;
            default:
                // TODO: log
                std::cout << "unknown character \'" << ch << "\'\n";
                return nullptr;
        }
    }
}

} // namespace reader
