#pragma once

#include "ltl/ltl.hpp"

#include <vector>
#include <set>
#include <map>
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

    static std::shared_ptr<converting> construct(ltl::node_t&& formula);

    [[maybe_unused, nodiscard]]
    ltl::node_t get_ltl_formula() const;

    [[maybe_unused, nodiscard]]
    const state_t& get_closure() const;

    [[nodiscard]]
    const state_t& get_concrete_state(size_t index) const;
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
               std::map<size_t, indexes_container_t>> get_automaton_representation() const;

    constexpr static bool implication(bool a, bool b);
    static bool is_in(const state_t &bunch, const ltl::node_t &node);

private:
    explicit converting(ltl::node_t&& formula);

    static bool z1_rule(const state_t &s, const ltl::node_t &node);

    static bool r1_rule(const state_t &s, const state_t &sd, const ltl::node_t &node);
    static bool r2_rule(const state_t &s, const state_t &sd, const ltl::node_t &node);
    /// rules R1-R2 + modified with neg
    static bool satisfies_r_rules(const state_t &s, const state_t &sd, const ltl::node_t &node);

    /// \brief Initial state calculation. Save in @m_A_0_indexes
    /// \param formula: LTL-formula from input
    void detect_initial_states(const ltl::node_t &formula);

    void generate_atomic_plurality();
    /// \note @pos and @neg size should be equal
    static void recursive_brute_force(std::vector<state_t> &states, const state_t &pos, const state_t &neg,
                                      state_t curr = {}, size_t i = 0);

    static bool conjunction_rule(const state_t &atomic, const ltl::node_t &node);
    static bool until_rules(const state_t &atomic, const ltl::node_t &node);
    static bool satisfies_atomic_rules(const state_t &atomic, const ltl::node_t &node);

    void fill_closure(const ltl::node_t& formula);
    void add_to_closure(const ltl::node_t& formula);

    /// \brief Algorithm implementing
    void ltl_to_nga();


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
