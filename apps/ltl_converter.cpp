#include <iostream>
#include "utils/reader.hpp"
#include "ltl/closure.hpp"

int main()
{
    auto ltl = reader::read_formula();

    auto algo = ltl::converting::construct(ltl);
    auto tuple = algo->ltl_to_nga();

    std::cout << "Not end\n";

    return 0;
}
