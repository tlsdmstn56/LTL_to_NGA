#include <iostream>
#include <fstream>
#include "utils/reader.hpp"
#include "ltl/closure.hpp"
#include "utils/dot_representation.hpp"

int main()
{
    // yes, let it be constant. No time to play we user
    const std::string file_path{"dot.gv"};

    const auto algo = ltl::converting::construct(reader::read_formula());
    const auto [states, dot] = dot::convert_to_dot(algo);

    {
        // save Graph to the file
        std::ofstream out_file{file_path};
        out_file << dot;
    }

    for (const auto &it : states)
        std::cout << it << "\n";

    return 0;
}
