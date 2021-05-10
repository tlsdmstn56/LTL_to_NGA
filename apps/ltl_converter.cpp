#include <iostream>
#include "utils/reader.hpp"
#include "ltl/closure.hpp"

int main()
{
    auto ltl = reader::read_formula();

    ltl::closure::construct(ltl);

    std::cout << "Not end\n";

    return 0;
}
