#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <string>

// TODO: remove
#include <iostream>

namespace ltl
{

/// \class Interface of an Atomic Proposition in a LTL formula
class ltl
{
public:
    using node_t = std::shared_ptr<ltl>;

    enum class kind : uint8_t
    {
        undefined = 0,
        one,
        atom,
        negation,
        conjunction,
        next,
        until
    };

    [[nodiscard]] kind get_kind() const { return m_kind; };

    friend bool operator== (const node_t& left, const node_t& right);

    [[nodiscard]] virtual std::string to_string() const = 0;

protected:
    explicit ltl(const kind kind) : m_kind(kind) {};

private:
    const kind m_kind {kind::undefined};
};

class ltl_one : public ltl
{
public:

    static node_t construct()
    {
        return std::shared_ptr<ltl_one>(new ltl_one());
    }

    friend bool operator== (const std::shared_ptr<ltl_one>& left, const std::shared_ptr<ltl_one>& right)
    {
        return true;
    }

    [[nodiscard]] std::string to_string() const final
    {
        return "true";
    }

private:
    explicit ltl_one()
            : ltl(kind::one)
    {}
};

class ltl_atom : public ltl
{
public:

    static node_t construct(uint32_t index)
    {
        return std::shared_ptr<ltl_atom>(new ltl_atom(index));
    }

    friend bool operator== (const std::shared_ptr<ltl_atom>& left, const std::shared_ptr<ltl_atom>& right)
    {
        return left->m_index == right->m_index;
    }

    [[nodiscard]] std::string to_string() const final
    {
        return "p" + std::to_string(m_index);
    }

    // TODO: is this atm_size type?
    const uint32_t m_index{0};

private:
    explicit ltl_atom(uint32_t index)
            : ltl(kind::atom), m_index(index)
    {}

};

class ltl_negation : public ltl
{
public:

    static node_t construct(node_t &&formula)
    {
        //assert(formula && "Empty inner formula");
        if (!formula)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner formula\n";
            return nullptr;
        }

        // optimization block with negative child
        if (formula->get_kind() == ltl::kind::negation)
            return std::dynamic_pointer_cast<ltl_negation>(formula)->m_negformula;

        return std::shared_ptr<ltl_negation>(new ltl_negation(formula));
    }

    friend bool operator== (const std::shared_ptr<ltl_negation>& left, const std::shared_ptr<ltl_negation>& right)
    {
        return left->m_negformula == right->m_negformula;
    }

    [[nodiscard]] std::string to_string() const final
    {
        return "! " + m_negformula->to_string();
    }

    const node_t m_negformula{nullptr};

private:
    explicit ltl_negation(node_t formula)
            : ltl(kind::negation), m_negformula(std::move(formula))
    {
        assert(m_negformula && "Formula should be set");
        assert(m_negformula->get_kind() != ltl::kind::negation && "Inner formula can't be negative");
    }
};

class ltl_conjunction : public ltl
{
public:

    static node_t construct(node_t &&left, node_t &&right)
    {
        //assert(left && right && "Empty inner formula");
        if (!left || !right)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner " << (left ? "" : "left") << (right ? "" : "right") <<  " formula\n";
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

    friend bool operator== (const std::shared_ptr<ltl_conjunction>& left, const std::shared_ptr<ltl_conjunction>& right)
    {
        return ((left->m_left == right->m_left && left->m_right == right->m_right) ||
                (left->m_left == right->m_right && left->m_right == right->m_left));
    }

    [[nodiscard]] std::string to_string() const final
    {
        return "^ " + m_left->to_string() + " " + m_right->to_string();
    }

    const node_t m_left{nullptr};
    const node_t m_right{nullptr};

private:
    explicit ltl_conjunction(node_t left, node_t right)
            : ltl(kind::conjunction), m_left(std::move(left)), m_right(std::move(right))
    {
        assert(m_left && m_right && "Left and Right should be set");
    }

};

class ltl_next : public ltl
{
public:

    static node_t construct(node_t &&xformula)
    {
        //assert(xformula && "Empty inner formula");
        if (!xformula)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner formula\n";
            return nullptr;
        }

        return std::shared_ptr<ltl_next>(new ltl_next(xformula));
    }

    friend bool operator== (const std::shared_ptr<ltl_next>& left, const std::shared_ptr<ltl_next>& right)
    {
        return left->m_xformula == right->m_xformula;
    }

    [[nodiscard]] std::string to_string() const final
    {
        return "X " + m_xformula->to_string();
    }

    const node_t m_xformula{nullptr};

private:
    explicit ltl_next(node_t formula)
            : ltl(kind::next), m_xformula(std::move(formula))
    {
        assert(m_xformula && "Formula should be set");
    }

};

class ltl_until : public ltl
{
public:

    static node_t construct(node_t &&left, node_t &&right)
    {
        //assert(left && right && "Empty inner formula");
        if (!left || !right)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner " << (left ? "" : "left") << (right ? "" : "right") <<  " formula\n";
            return nullptr;
        }

        return std::shared_ptr<ltl_until>(new ltl_until(left, right));
    }

    friend bool operator== (const std::shared_ptr<ltl_until>& left, const std::shared_ptr<ltl_until>& right)
    {
        return left->m_left == right->m_left && left->m_right == right->m_right;
    }

    [[nodiscard]] std::string to_string() const final
    {
        return "U " + m_left->to_string() + " " + m_right->to_string();
    }

    const node_t m_left{nullptr};
    const node_t m_right{nullptr};

private:
    explicit ltl_until(node_t left, node_t right)
            : ltl(kind::until), m_left(std::move(left)), m_right(std::move(right))
    {
        assert(m_left && m_right && "Left and Right should be set");
    }

};

bool operator== (const ltl::node_t& left, const ltl::node_t& right)
{
    if (left->get_kind() == right->get_kind())
    {
        // TODO: try to rewrite
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
                assert("Incorrect proposition during comparing");
        }
    }

    assert(left->get_kind() != right->get_kind() && "Shouldn't be called");
    return false;
}

} // namespace ltl
