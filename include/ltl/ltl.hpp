#pragma once

#include <cstdint>
#include <memory>
#include <utility>

// TODO: remove
#include <iostream>

namespace ltl
{

/// \class Interface of an Atomic Proposition in a LTL formula
class ltl
{
public:
    using proposition_t = std::shared_ptr<ltl>;

    enum class kind : uint8_t
    {
        undefined = 0,
        atom,
        constant,
        junct,
        next,
        until
    };

    [[nodiscard]] kind get_kind() const { return m_kind; };

    explicit ltl(const kind kind, bool not_flag)
            : m_kind(kind), m_not(not_flag) {};

    [[nodiscard]] bool is_negative() const { return m_not; };

    // FIXME: recursive approach is awful here, but I didn't have time to rework it
    [[nodiscard]] virtual proposition_t construct_negative() const = 0;

    friend bool operator== (const proposition_t& left, const proposition_t& right);

private:
    const kind m_kind {kind::undefined};
    const bool m_not{false};
};

class ltl_atom : public ltl
{
public:

    static proposition_t construct(bool is_false, uint32_t num)
    {
        return std::shared_ptr<ltl_atom>(new ltl_atom(is_false, num));
    }

    ~ltl_atom()
    {
        std::cout << "Atom distruct " << (is_negative() ? "! " : "") << m_index << "\n";
    }

    [[nodiscard]]
    proposition_t construct_negative() const final
    {
        return std::shared_ptr<ltl_atom>(new ltl_atom(!is_negative(), m_index));
    }

    friend bool operator== (const std::shared_ptr<ltl_atom>& left, const std::shared_ptr<ltl_atom>& right)
    {
        return left->m_index == right->m_index && left->is_negative() == right->is_negative();
    }

    // TODO: is this atm_size type?
    const uint32_t m_index{0};

private:
    explicit ltl_atom(bool is_negative, uint32_t index)
            : ltl(kind::atom, is_negative), m_index(index)
    {}

};

class ltl_constant : public ltl
{
public:

    static proposition_t construct(bool is_false)
    {
        return std::shared_ptr<ltl_constant>(new ltl_constant(is_false));
    }

    [[nodiscard]]
    proposition_t construct_negative() const final
    {
        return std::shared_ptr<ltl_constant>(new ltl_constant(!is_negative()));
    }

    friend bool operator== (const std::shared_ptr<ltl_constant>& left, const std::shared_ptr<ltl_constant>& right)
    {
        return left->is_negative() == right->is_negative();
    }

private:
    explicit ltl_constant(bool is_false)
            : ltl(kind::constant, is_false)
    {}
};

class ltl_junct : public ltl
{
public:

    static proposition_t construct(bool is_disjunction, proposition_t &&right, proposition_t &&left)
    {
        if (!left || !right)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner " << (left ? "" : "left") << (right ? "" : "right") <<  " formula\n";
            return nullptr;
        }

        // optimization block
        if (left->get_kind() == ltl::kind::constant)
            return left->is_negative() == is_disjunction ? right : left;
        if (right->get_kind() == ltl::kind::constant)
            return right->is_negative() == is_disjunction ? left : right;
        if (left == right)
            return left;

        return std::shared_ptr<ltl_junct>(new ltl_junct(is_disjunction, left, right));
    }

    [[nodiscard]]
    proposition_t construct_negative() const final
    {
        return std::shared_ptr<ltl_junct>(
                new ltl_junct(!is_negative(), m_left->construct_negative(), m_right->construct_negative()));
    }

    friend bool operator== (const std::shared_ptr<ltl_junct>& left, const std::shared_ptr<ltl_junct>& right)
    {
        return left->is_negative() == right->is_negative() &&
                ((left->m_left == right->m_left && left->m_right == right->m_right) ||
                (left->m_left == right->m_right && left->m_right == right->m_left));
    }

    const proposition_t m_left{nullptr};
    const proposition_t m_right{nullptr};

private:
    explicit ltl_junct(bool is_disjunction, proposition_t left, proposition_t right)
            : ltl(kind::junct, is_disjunction), m_left(std::move(left)), m_right(std::move(right))
    {
        assert(m_left && m_right && "Left and Right should be set");
    }

};

class ltl_next : public ltl
{
public:

    static proposition_t construct(proposition_t &&formula)
    {
        if (!formula)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner formula\n";
            return nullptr;
        }

        return std::shared_ptr<ltl_next>(new ltl_next(formula));
    }

    [[nodiscard]]
    proposition_t construct_negative() const final
    {
        return std::shared_ptr<ltl_next>(new ltl_next(m_formula->construct_negative()));
    }

    friend bool operator== (const std::shared_ptr<ltl_next>& left, const std::shared_ptr<ltl_next>& right)
    {
        return left->m_formula == right->m_formula;
    }

    const proposition_t m_formula{nullptr};

private:
    explicit ltl_next(proposition_t formula)
            : ltl(kind::next, false), m_formula(std::move(formula))
    {
        assert(m_formula && "Formula should be set");
    }

};

class ltl_until : public ltl
{
public:

    static proposition_t construct(bool is_release, proposition_t &&right, proposition_t &&left)
    {
        if (!left || !right)
        {
            // TODO: log
            std::cout << "ERROR: Empty inner " << (left ? "" : "left") << (right ? "" : "right") <<  " formula\n";
            return nullptr;
        }

        return std::shared_ptr<ltl_until>(new ltl_until(is_release, left, right));
    }

    [[nodiscard]]
    proposition_t construct_negative() const final
    {
        return std::shared_ptr<ltl_until>(
                new ltl_until(!is_negative(), m_left->construct_negative(), m_right->construct_negative()));
    }

    friend bool operator== (const std::shared_ptr<ltl_until>& left, const std::shared_ptr<ltl_until>& right)
    {
        return left->is_negative() == right->is_negative() &&
                ((left->m_left == right->m_left && left->m_right == right->m_right) ||
                (left->m_left == right->m_right && left->m_right == right->m_left));
    }

    const proposition_t m_left{nullptr};
    const proposition_t m_right{nullptr};

private:
    explicit ltl_until(bool is_release, proposition_t left, proposition_t right)
            : ltl(kind::until, is_release), m_left(std::move(left)), m_right(std::move(right))
    {
        assert(m_left && m_right && "Left and Right should be set");
    }

};

bool operator== (const ltl::proposition_t& left, const ltl::proposition_t& right)
{
    if (left->get_kind() == right->get_kind())
    {
        // FIXME: try to rewrite
        // C++20 style
        auto fn = [&left, &right]<typename T>() -> bool
        {
            return std::dynamic_pointer_cast<T>(left) == std::dynamic_pointer_cast<T>(right);
        };

        switch(left->get_kind())
        {
            case ltl::kind::constant:
                return fn.operator()<ltl_constant>();
            case ltl::kind::atom:
                return fn.operator()<ltl_atom>();
            case ltl::kind::junct:
                return fn.operator()<ltl_junct>();
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
