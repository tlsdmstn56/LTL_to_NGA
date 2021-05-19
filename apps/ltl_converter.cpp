#include "utils/reader.hpp"
#include "ltl/closure.hpp"
#include "utils/dot_representation.hpp"

#include <iostream>
#include <fstream>

/// \brief Program entrance
/// Read LTL-formula in Polish notation from standard input
/// Transform LTL-formula to the automaton and save its dot-representation into the @file_path
/// Print detailed explanation of the each a_i state into the standard output
/// \return 0 on success
int main()
{
    // yes, let it be constant. No time to play with user
    const std::string file_path{"dot.gv"};

    const auto algo = ltl::converting::construct(reader::read_formula());
    const auto [states, dot] = dot::convert_to_dot(algo);

    {
        // save Graph to the file
        std::ofstream out_file{file_path};
        out_file << dot;
    }

    // print detailed explanation of the states (atomic plurality for each a_i)
    for (const auto &it : states)
        std::cout << it << "\n";

    return 0;
}
