#pragma once

#include "ltl/ltl.hpp"

#include <vector>

// TODO: remove iostream
#include <iostream>

namespace ltl
{

//class atomic_plurality

class closure
{
public:
    using closure_data_t = std::vector<std::pair<ltl::proposition_t, ltl::proposition_t>>;

    ltl::proposition_t find(const ltl::proposition_t& copy)
    {
        for (const auto &[left, right] : m_storage)
        {
            if (left == copy)
                return left;
            if (right == copy)
                return right;
        }

        return nullptr;
    }
    ltl::proposition_t insert(const ltl::proposition_t &&copy)
    {
        auto from_storage = find(copy);
        if (from_storage)
            return from_storage;

        //m_storage.emplace_back(std::make_pair(copy, copy->construct_negative(shared_from_this())));

        return copy;
    }

    // find, insert
    static void construct(const ltl::proposition_t& formula)
    {
        closure_data_t storage{};
        process_formula(formula, storage);

        std::cout << storage.size() << " <-size\n";
    }

private:
    closure() = default;

    static void process_formula(const ltl::proposition_t& formula, closure_data_t& storage)
    {
        switch (formula->get_kind())
        {
            case ltl::kind::junct:
                process_formula(std::dynamic_pointer_cast<ltl_junct>(formula)->m_left, storage);
                process_formula(std::dynamic_pointer_cast<ltl_junct>(formula)->m_right, storage);

                break;
            case ltl::kind::next:
                process_formula(std::dynamic_pointer_cast<ltl_next>(formula)->m_formula, storage);

                break;
            case ltl::kind::until:
                process_formula(std::dynamic_pointer_cast<ltl_until>(formula)->m_left, storage);
                process_formula(std::dynamic_pointer_cast<ltl_until>(formula)->m_right, storage);

                break;
            case ltl::kind::atom:
            case ltl::kind::constant:
                break;
            default:
                assert("Incorrect proposition during closure creating");
        }

        for (const auto &it : storage)
            if (it.first == formula || it.second == formula)
                // already exists
                return;

        // adding new formula pair
        storage.emplace_back(std::make_pair(formula, formula->construct_negative()));
    }

    /// \brief Closure of a provided LTL formula that contains its negative formula in each unique pair of a set
    closure_data_t m_storage{};
};

} // namespace ltl
