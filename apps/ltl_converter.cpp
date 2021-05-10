#include <iostream>
#include "utils/reader.hpp"

int main()
{
    auto ltl = reader::read_formula();

    std::cout << "Not end\n";

    return 0;
}
