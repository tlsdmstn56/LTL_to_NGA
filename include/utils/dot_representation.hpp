#pragma once

#include "ltl/closure.hpp"

#include <optional>

namespace dot
{

namespace
{

inline std::string get_node_label_text(const size_t index)
{
    return "a" + std::to_string(index);
}

std::string create_node_style(const std::string &formula, const std::string& finals, const bool is_initial)
{
    std::string finals_label_text;
    if (!finals.empty())
        finals_label_text = R"(<font color="Maroon" point-size="10">)" + finals + "</font>";

    return std::string{"[shape="} + (finals_label_text.empty() ? "circle" : "doublecircle") +
            (is_initial ? ",fillcolor=bisque,style=filled" : "") +
            ",label=<" + formula + "<br />" + finals_label_text + ">];";
}

// print all states with its indexes
std::vector<std::string> generate_states_map(const std::shared_ptr<ltl::converting> &algo,
                                             const ltl::converting::indexes_container_t &states)
{
    auto generate_state_full_name = [&algo](const size_t index) -> std::string
    {
        std::string full_name;
        for (const auto &node : algo->get_concrete_state(index))
        {
            if (!full_name.empty())
                full_name += "; ";
            full_name += node->to_string();
        }

        return "{" + full_name + "}";
    };

    std::vector<std::string> states_map_str;
    states_map_str.reserve(states.size());
    for (const auto index : states)
        states_map_str.emplace_back(get_node_label_text(index) + " = " + generate_state_full_name(index));

    return std::move(states_map_str);
}

std::string generate_nodes(const ltl::converting::indexes_container_t &states,
                           const ltl::converting::indexes_container_t &initials,
                           const std::map<size_t, ltl::converting::indexes_container_t> &final_sets)
{
    std::string nodes;
    for (const auto &index : states)
    {
        auto get_finals_label = [&final_sets](const size_t index) -> std::string
        {
            std::string final_label;
            for (const auto &[num, value] : final_sets)
            {
                if (value.contains(index))
                {
                    if (!final_label.empty())
                        final_label += ", ";
                    final_label += std::to_string(num);
                }
            }
            if (!final_label.empty())
                return "F:{" + final_label + "}";

            return "";
        };

        nodes += std::to_string(index) + create_node_style(get_node_label_text(index), get_finals_label(index),
                                                           initials.contains(index));
    }

    return std::move(nodes);
}

std::string generate_edges(const ltl::converting::table_t &transitions)
{
    std::string edges;
    for (const auto &[index, value] : transitions)
    {
        auto create_label = [](const std::set<ltl::ltl_atom::index_atom_t> &alph) -> std::string
        {
            const std::string empty_edge_style{"[style=dotted,label=<&#8709;>;];"};

            std::string alph_str;
            for (const auto it : alph)
            {
                if (!alph_str.empty())
                    alph_str += ",";
                alph_str += "p" + std::to_string(it);
            }

            return (alph.empty() ? empty_edge_style : ("[label=\"\\{" + alph_str + "\\}\"];"));
        };

        std::string next_states_str;
        for (const auto &it : value.second)
        {
            if (!next_states_str.empty())
                next_states_str += ",";
            next_states_str += std::to_string(it);
        }

        edges += std::to_string(index) + "->{" + next_states_str + "}" + create_label(value.first);
    }

    return std::move(edges);
}

} // namespace anonymous

/// \param simplify: do not print formula inside circles
/// \return first element is a state representation and the second is a dot-language graph
std::pair<std::vector<std::string>, std::string> convert_to_dot(const std::shared_ptr<ltl::converting>& algo)
{
    const auto &[states, _, transitions, initials, final_sets] = algo->get_automaton_representation();

    const std::string header = R"(splines="polyline";rankdir=LR;label=")" + algo->get_ltl_formula()->to_string() +
                               R"(";labelloc="t";fontsize=30;fontcolor=gray;)";
    const std::string nodes = generate_nodes(states, initials, final_sets);
    const std::string edges = generate_edges(transitions);

    return {
        generate_states_map(algo, states),
        "digraph Automaton {" + header + nodes + edges + "}"
    };
}

} // namespace dot
