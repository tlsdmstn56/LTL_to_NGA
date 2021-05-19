#include "ltl/ltl.hpp"

#include <utility>

// TODO: make a common storage for identical ltl-objects

namespace ltl
{

ltl::ltl(const kind kind)
        : m_kind(kind)
{}

ltl::node_t ltl_one::construct()
{
    return std::shared_ptr<ltl_one>(new ltl_one());
}

bool operator== (const std::shared_ptr<ltl_one>& left, const std::shared_ptr<ltl_one>& right)
{
    return true;
}

std::string ltl_one::to_string() const
{
    return "true";
}

ltl_one::ltl_one()
        : ltl(kind::one)
{}

ltl::node_t ltl_atom::construct(const index_atom_t index)
{
    return std::shared_ptr<ltl_atom>(new ltl_atom(index));
}

bool operator== (const std::shared_ptr<ltl_atom>& left, const std::shared_ptr<ltl_atom>& right)
{
    return left->m_index == right->m_index;
}

std::string ltl_atom::to_string() const
{
    return "p" + std::to_string(m_index);
}

ltl_atom::ltl_atom(const index_atom_t index)
        : ltl(kind::atom), m_index(index)
{}

ltl::node_t ltl_negation::construct(node_t &&formula)
{
    if (!formula)
    {
        assert(!"Empty inner formula");
        return nullptr;
    }

    // optimization block with negative child
    if (formula->get_kind() == ltl::kind::negation)
        return std::dynamic_pointer_cast<ltl_negation>(formula)->m_negformula;

    return std::shared_ptr<ltl_negation>(new ltl_negation(formula));
}

bool operator== (const std::shared_ptr<ltl_negation>& left, const std::shared_ptr<ltl_negation>& right)
{
    return left->m_negformula == right->m_negformula;
}

std::string ltl_negation::to_string() const
{
    return "! " + m_negformula->to_string();
}

ltl_negation::ltl_negation(node_t formula)
        : ltl(kind::negation), m_negformula(std::move(formula))
{
    assert(m_negformula && "Formula should be set");
    assert(m_negformula->get_kind() != ltl::kind::negation && "Inner formula can't be negative");
}

ltl::node_t ltl_conjunction::construct(node_t &&left, node_t &&right)
{
    if (!left || !right)
    {
        assert(left && right && "Empty inner formula");
        return nullptr;
    }

    // optimization block with true/false child
    if (left->get_kind() == ltl::kind::one)
        return right;
    if (right->get_kind() == ltl::kind::one)
        return left;
    if ((left->get_kind() == ltl::kind::negation &&
         std::dynamic_pointer_cast<ltl_negation>(left)->m_negformula->get_kind() == ltl::kind::one) ||
        (right->get_kind() == ltl::kind::negation &&
         std::dynamic_pointer_cast<ltl_negation>(right)->m_negformula->get_kind() == ltl::kind::one))
    {
        return ltl_negation::construct(ltl_one::construct());
    }
    // optimization block with equal children
    if (left == right)
        return left;

    return std::shared_ptr<ltl_conjunction>(new ltl_conjunction(left, right));
}

bool operator== (const std::shared_ptr<ltl_conjunction>& left, const std::shared_ptr<ltl_conjunction>& right)
{
    return ((left->m_left == right->m_left && left->m_right == right->m_right) ||
            (left->m_left == right->m_right && left->m_right == right->m_left));
}

std::string ltl_conjunction::to_string() const
{
    return "^ " + m_left->to_string() + " " + m_right->to_string();
}

ltl_conjunction::ltl_conjunction(node_t left, node_t right)
        : ltl(kind::conjunction), m_left(std::move(left)), m_right(std::move(right))
{
    assert(m_left && m_right && "Left and Right should be set");
}

ltl::node_t ltl_next::construct(node_t &&xformula)
{
    if (!xformula)
    {
        assert(xformula && "Empty inner formula");
        return nullptr;
    }

    return std::shared_ptr<ltl_next>(new ltl_next(xformula));
}

bool operator== (const std::shared_ptr<ltl_next>& left, const std::shared_ptr<ltl_next>& right)
{
    return left->m_xformula == right->m_xformula;
}

std::string ltl_next::to_string() const
{
    return "X " + m_xformula->to_string();
}

ltl_next::ltl_next(node_t formula)
        : ltl(kind::next), m_xformula(std::move(formula))
{
    assert(m_xformula && "Formula should be set");
}

ltl::node_t ltl_until::construct(node_t &&left, node_t &&right)
{
    if (!left || !right)
    {
        assert(left && right && "Empty inner formula");
        return nullptr;
    }

    return std::shared_ptr<ltl_until>(new ltl_until(left, right));
}

bool operator== (const std::shared_ptr<ltl_until>& left, const std::shared_ptr<ltl_until>& right)
{
    return left->m_left == right->m_left && left->m_right == right->m_right;
}

std::string ltl_until::to_string() const
{
    return "U " + m_left->to_string() + " " + m_right->to_string();
}

ltl_until::ltl_until(node_t left, node_t right)
        : ltl(kind::until), m_left(std::move(left)), m_right(std::move(right))
{
    assert(m_left && m_right && "Left and Right should be set");
}


bool operator== (const ltl::node_t& left, const ltl::node_t& right)
{
    if (left->get_kind() == right->get_kind())
    {
        // TODO: try to simplify
        // C++20 style
        auto fn = [&left, &right]<typename T>() -> bool
        {
            return std::dynamic_pointer_cast<T>(left) == std::dynamic_pointer_cast<T>(right);
        };

        switch(left->get_kind())
        {
            case ltl::kind::one:
                return fn.operator()<ltl_one>();
            case ltl::kind::atom:
                return fn.operator()<ltl_atom>();
            case ltl::kind::negation:
                return fn.operator()<ltl_negation>();
            case ltl::kind::conjunction:
                return fn.operator()<ltl_conjunction>();
            case ltl::kind::next:
                return fn.operator()<ltl_next>();
            case ltl::kind::until:
                return fn.operator()<ltl_until>();
            default:
                assert(!"Incorrect proposition during comparing");
        }
    }

    assert(left->get_kind() != right->get_kind() && "Shouldn't be called");
    return false;
}

} // namespace ltl
