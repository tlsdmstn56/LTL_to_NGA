#include <iostream>
#include "utils/reader.hpp"
#include "ltl/closure.hpp"

int main()
{
    auto algo = ltl::converting::construct(reader::read_formula());
    auto automaton = algo->get_automaton_representation();

    // print all states with its indexes
    for (const auto initial_index : std::get<0>(automaton))
    {
        std::cout << initial_index << ": { ";
        for (const auto &node : algo->get_concrete_state(initial_index))
            std::cout << node->to_string() << "; ";
        std::cout << "}\n";
    }

    return 0;
}
