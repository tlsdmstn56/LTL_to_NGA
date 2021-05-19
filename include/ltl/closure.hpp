#pragma once

#include "ltl/ltl.hpp"

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <tuple>

namespace ltl
{

class converting
{
public:
    using state_t = std::vector<ltl::node_t>;
    using indexes_container_t = std::set<size_t>;
    /// \brief key : is an index of At -> value : first -- alph, second -- next states indexes of At
    using table_t = std::map<size_t, std::pair<std::set<ltl_atom::index_atom_t>, indexes_container_t>>;

    static std::shared_ptr<converting> construct(ltl::node_t&& formula)
    {
        return std::shared_ptr<converting>(new converting(std::move(formula)));
    }

    [[maybe_unused, nodiscard]]
    ltl::node_t get_ltl_formula() const { return m_formula; }

    [[maybe_unused, nodiscard]]
    inline const state_t& get_closure() const { return m_closure; }

    [[nodiscard]]
    inline const state_t& get_concrete_state(const size_t index) const { return m_At[index]; }
    /// \brief Get data related to the resulted Automaton
    /// [0] A - atomic plurality indexes that represents Automaton states
    /// [1] AP - atomic propositions indexes used in LTL-formula
    /// [2] f - transition table of state's index (key) : pair (value) of a alphabet (first) to get state's index (second)
    /// [3] A_0 - atomic plurality indexes that represents Automaton initial states
    /// [4] F - plurality of a final states pluralities where key is a queue number of Until operator in closer
    ///                                             with value of an appropriate final state indexes plurality for it
    /// \return A, AP, f, A_0, F
    [[nodiscard]]
    std::tuple<indexes_container_t, std::set<ltl_atom::index_atom_t>, table_t, indexes_container_t,
               std::map<size_t, indexes_container_t>> get_automaton_representation() const
    {
        return std::make_tuple(m_A, ap, m_table, m_A_0, m_F);
    }

    constexpr static bool implication(const bool a, const bool b) { return !a || b; }
    constexpr static bool is_in(const state_t &bunch, const ltl::node_t &node)
    {
        return std::any_of(bunch.begin(), bunch.end(), [&node](const ltl::node_t &it) -> bool { return it == node; });
    }

private:
    explicit converting(ltl::node_t&& formula) : m_formula(formula)
    {
        fill_closure(m_formula);
        generate_atomic_plurality();
        detect_initial_states(m_formula);

        ltl_to_nga();
    }

    static bool z1_rule(const state_t &s, const ltl::node_t &node)
    {
        if (node->get_kind() != ltl::kind::until)
            return false;

        const bool is_node_in_s = is_in(s, node);
        const bool is_b_in_s = is_in(s, std::dynamic_pointer_cast<ltl_until>(node)->m_right);

        return implication(is_node_in_s, is_b_in_s);
    }

    static bool r1_rule(const state_t &s, const state_t &sd, const ltl::node_t &node)
    {
        if (node->get_kind() != ltl::kind::next)
            return true;

        // whether negation
        const bool is_node_in_s = is_in(s, node);;

        // is a in sd
        const bool is_a_in_sd = is_in(sd, std::dynamic_pointer_cast<ltl_next>(node)->m_xformula);

        return is_node_in_s == is_a_in_sd;
    }
    static bool r2_rule(const state_t &s, const state_t &sd, const ltl::node_t &node)
    {
        if (node->get_kind() != ltl::kind::until)
            return true;

        // whether negation
        const bool is_node_in_s = is_in(s, node);

        const auto &node_s_until = std::dynamic_pointer_cast<ltl_until>(node);

        const bool is_b_in_s = is_in(s, node_s_until->m_right);
        const bool is_a_in_s = is_in(s, node_s_until->m_left);
        const bool is_node_s_in_sd = is_in(sd, node_s_until);

        const bool rule = is_b_in_s || (is_a_in_s && is_node_s_in_sd);

        return is_node_in_s == rule;
    }
    /// rules R1-R2 + modified with neg
    static bool satisfies_r_rules(const state_t &s, const state_t &sd, const ltl::node_t &node)
    {
        if (const auto kind = node->get_kind(); kind == ltl::kind::negation)
        {
            return satisfies_r_rules(s, sd, std::dynamic_pointer_cast<ltl_negation>(node)->m_negformula);
        }
        else if (kind == ltl::kind::next)
        {
            /// rule R1: Xa in s = a in sd
            return r1_rule(s, sd, node);
        }
        else if (kind == ltl::kind::until)
        {
            /// rule R2: (a U b) in s = b in s OR (a in s AND (a U b) in sd)
            return r2_rule(s, sd, node);
        }

        return true;
    }

    /// \brief Initial state calculation. Save in @m_A_0_indexes
    /// \param formula: LTL-formula from input
    void detect_initial_states(const ltl::node_t &formula)
    {
        m_A_0.clear();
        for (size_t i = 0; i < m_At.size(); ++i)
        {
            if (is_in(m_At[i], formula))
                m_A_0.insert(i);
        }
    }

    void generate_atomic_plurality()
    {
        state_t neg_storage{m_closure};
        /// rule 1 prediction
        for (ltl::node_t &it : neg_storage)
            it = ltl_negation::construct(std::move(it));

        recursive_brute_force(m_At, m_closure, neg_storage);
    }
    /// \note @pos and @neg size should be equal
    static void recursive_brute_force(std::vector<state_t> &states, const state_t &pos, const state_t &neg,
                                      state_t curr = {}, size_t i = 0)
    {
        if (pos.size() != neg.size())
            return;

        // initialization of input default parameter
        if (i == 0)
            curr = state_t{pos.size(), nullptr};

        if (i == pos.size())
        {
            if (std::all_of(curr.begin(), curr.end(),
                            [&](const ltl::node_t &node) -> bool
                            { return satisfies_atomic_rules(curr, node); }))
            {
                states.emplace_back(curr);
            }

            return;
        }

        curr[i] = pos[i];
        recursive_brute_force(states, pos, neg, curr, i+1);

        curr[i] = neg[i];
        recursive_brute_force(states, pos, neg, curr, i+1);
    }

    static bool conjunction_rule(const state_t &atomic, const ltl::node_t &node)
    {
        if (node->get_kind() != ltl::kind::conjunction)
            return true;

        /// \note this gives us an understanding of whether there was a negation before the node
        // whether negation
        const bool is_node_in_atomic = is_in(atomic, node);

        const auto node_conjunction = std::dynamic_pointer_cast<ltl_conjunction>(node);

        const bool is_a_in_atomic = is_in(atomic, node_conjunction->m_left);
        const bool is_b_in_atomic = is_in(atomic, node_conjunction->m_right);

        const bool rule = is_a_in_atomic && is_b_in_atomic;

        return is_node_in_atomic == rule;
    }
    static bool until_rules(const state_t &atomic, const ltl::node_t &node)
    {
        if (node->get_kind() != ltl::kind::until)
            return true;

        const auto &node_until = std::dynamic_pointer_cast<ltl_until>(node);

        const bool is_node_in_atomic = is_in(atomic, node_until);
        const bool is_a_in_atomic = is_in(atomic, node_until->m_left);
        const bool is_b_in_atomic = is_in(atomic, node_until->m_right);

        const bool rule_3 = implication(is_node_in_atomic && !is_b_in_atomic, is_a_in_atomic);
        const bool rule_4 = implication(is_b_in_atomic, is_node_in_atomic);

        return rule_3 && rule_4;
    }
    static bool satisfies_atomic_rules(const state_t &atomic, const ltl::node_t &node)
    {
        if (const auto kind = node->get_kind(); kind == ltl::kind::negation)
        {
            return satisfies_atomic_rules(atomic, std::dynamic_pointer_cast<ltl_negation>(node)->m_negformula);
        }
        else if (kind == ltl::kind::conjunction)
        {
            /// rule 2
            return conjunction_rule(atomic, node);
        }
        else if (kind == ltl::kind::until)
        {
            /// rule 3-4
            return until_rules(atomic, node);
        }

        return true;
    }

    void fill_closure(const ltl::node_t& formula)
    {
        switch (formula->get_kind())
        {
            case ltl::kind::negation:
                fill_closure(std::dynamic_pointer_cast<ltl_negation>(formula)->m_negformula);
                return;

            case ltl::kind::one:
                break;
            case ltl::kind::atom:
            {
                // AP saving
                ap.insert(std::dynamic_pointer_cast<ltl_atom>(formula)->m_index);
                break;
            }
            case ltl::kind::conjunction:
            {
                fill_closure(std::dynamic_pointer_cast<ltl_conjunction>(formula)->m_left);
                fill_closure(std::dynamic_pointer_cast<ltl_conjunction>(formula)->m_right);
                break;
            }
            case ltl::kind::next:
            {
                fill_closure(std::dynamic_pointer_cast<ltl_next>(formula)->m_xformula);
                break;
            }
            case ltl::kind::until:
            {
                fill_closure(std::dynamic_pointer_cast<ltl_until>(formula)->m_left);
                fill_closure(std::dynamic_pointer_cast<ltl_until>(formula)->m_right);
                break;
            }
            default:
                assert("Shouldn't happen - we must cover all cases");
                break;
        }

        add_to_closure(formula);
    }
    void add_to_closure(const ltl::node_t& formula)
    {
        for (const auto &it : m_closure)
            if (it == formula)
                return;

        m_closure.emplace_back(formula);
    }

    /// \brief Algorithm implementing
    void ltl_to_nga()
    {
        indexes_container_t C_indexes = m_A_0;
        while (!C_indexes.empty())
        {
            const size_t s_index = *C_indexes.begin();
            C_indexes.erase(s_index);
            const state_t &s = m_At[s_index];

            m_A.insert(s_index);

            size_t i = 0;
            for (const ltl::node_t &alpha : m_closure)
            {
                /// rule Z1
                if (z1_rule(s, alpha))
                    m_F[i].insert(s_index);
                // till next until
                if (alpha->get_kind() == ltl::kind::until)
                    ++i;
            }

            indexes_container_t next_states_indexes{};
            for (size_t sd_index = 0; sd_index < m_At.size(); ++sd_index)
            {
                /// rule R1-R2
                if (std::all_of(s.begin(), s.end(),
                                [&](const ltl::node_t &node) -> bool
                                { return satisfies_r_rules(s, m_At[sd_index], node); }))
                {
                    next_states_indexes.insert(sd_index);
                    if (!std::any_of(m_A.begin(), m_A.end(),
                                     [&sd_index](const size_t &it) -> bool
                                     { return it == sd_index; }))
                        C_indexes.insert(sd_index);
                }
            }

            if (!next_states_indexes.empty())
            {
                // collect all atomic propositions that become curves
                std::set<ltl_atom::index_atom_t> s_proposition{};
                for (const auto &node : s)
                    if (node->get_kind() == ltl::kind::atom)
                        s_proposition.insert(std::dynamic_pointer_cast<ltl_atom>(node)->m_index);

                assert(m_table[s_index].second.empty() && "Should be empty according to the algorithm");
                m_table[s_index] = std::make_pair(std::move(s_proposition), std::move(next_states_indexes));
            }
        }
    }


    /// \brief Store LTL-formula from input
    const ltl::node_t m_formula;
    /// \brief Closure of LTL-formula
    std::vector<ltl::node_t> m_closure{};
    /// \brief Atomic plurality of LTL-formula
    std::vector<std::vector<ltl::node_t>> m_At{};

    /// Automaton representation

    /// \brief All states in automaton indexes
    indexes_container_t m_A{};
    /// \brief All Atomic Propositions in LTL-formula
    std::set<ltl_atom::index_atom_t> ap{};
    /// \brief Transition table via indexes
    table_t m_table{};
    /// \brief Initial states indexes
    indexes_container_t m_A_0{};
    /// \brief Final states plurality of pluralities
    std::map<size_t, indexes_container_t> m_F;
};

} // namespace ltl
