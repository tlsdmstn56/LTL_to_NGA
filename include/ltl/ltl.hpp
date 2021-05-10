#pragma once

#include <cstdint>
#include <memory>
#include <utility>

namespace ltl
{

/// \class Interface of an Atomic Proposition in a LTL formula
class ltl
{
public:

    enum class kind : uint8_t
    {
        atom = 0,
        constant,
        junct,
        next,
        until
    };

    [[nodiscard]] virtual kind get_kind() const = 0;

    explicit ltl(bool not_flag)
            : m_not(not_flag) {};

    [[nodiscard]] bool is_negative() const { return m_not; };

    [[nodiscard]] virtual std::shared_ptr<ltl> construct_negative() const = 0;

    friend bool operator== (const std::shared_ptr<ltl>& left, const std::shared_ptr<ltl>& right)
    {
        assert(left->get_kind() != right->get_kind() && "Shouldn't be called");
        return false;
    }

private:

    const bool m_not{false};
};

class ltl_atom : public ltl
{
public:
    [[nodiscard]] kind get_kind() const final { return kind::atom; }

    static std::shared_ptr<ltl> construct(bool is_false, uint32_t num)
    {
        return std::shared_ptr<ltl_atom>(new ltl_atom(is_false, num));
    }

    ~ltl_atom()
    {
        std::cout << "Atom distruct " << m_index << "\n";
    }

    [[nodiscard]]
    std::shared_ptr<ltl> construct_negative() const final
    {
        return std::shared_ptr<ltl_atom>(new ltl_atom(!is_negative(), m_index + 100));
    }

    friend bool operator== (const std::shared_ptr<ltl_atom>& left, const std::shared_ptr<ltl_atom>& right)
    {
        return left->m_index == right->m_index && left->is_negative() == right->is_negative();
    }

private:
    explicit ltl_atom(bool is_negative, uint32_t index)
            : ltl(is_negative), m_index(index)
    {}

    // TODO: is this atm_size type?
    const uint32_t m_index{0};
};

class ltl_constant : public ltl
{
public:
    [[nodiscard]] kind get_kind() const final { return kind::constant; }

    static std::shared_ptr<ltl> construct(bool is_false)
    {
        return std::shared_ptr<ltl_constant>(new ltl_constant(is_false));
    }

    [[nodiscard]]
    std::shared_ptr<ltl> construct_negative() const final
    {
        return std::shared_ptr<ltl_constant>(new ltl_constant(!is_negative()));
    }

    friend bool operator== (const std::shared_ptr<ltl_constant>& left, const std::shared_ptr<ltl_constant>& right)
    {
        return left->is_negative() == right->is_negative();
    }

private:
    explicit ltl_constant(bool is_false)
            : ltl(is_false)
    {}
};

class ltl_junct : public ltl
{
public:
    [[nodiscard]] kind get_kind() const final { return kind::junct; }

    static std::shared_ptr<ltl> construct(bool is_disjunction, std::shared_ptr<ltl> &&right, std::shared_ptr<ltl> &&left)
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
    std::shared_ptr<ltl> construct_negative() const final
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

private:
    explicit ltl_junct(bool is_disjunction, std::shared_ptr<ltl> left, std::shared_ptr<ltl> right)
            : ltl(is_disjunction), m_left(std::move(left)), m_right(std::move(right))
    {
        assert(m_left && m_right && "Left and Right should be set");
    }

    const std::shared_ptr<ltl> m_left{nullptr};
    const std::shared_ptr<ltl> m_right{nullptr};
};

class ltl_next : public ltl
{
public:
    [[nodiscard]] kind get_kind() const final { return kind::next; }

    static std::shared_ptr<ltl> construct(std::shared_ptr<ltl> &&formula)
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
    std::shared_ptr<ltl> construct_negative() const final
    {
        return std::shared_ptr<ltl_next>(new ltl_next(m_formula->construct_negative()));
    }

    friend bool operator== (const std::shared_ptr<ltl_next>& left, const std::shared_ptr<ltl_next>& right)
    {
        return left->m_formula == right->m_formula;
    }

private:
    explicit ltl_next(std::shared_ptr<ltl> formula)
            : ltl(false), m_formula(std::move(formula))
    {
        assert(m_formula && "Formula should be set");
    }

    const std::shared_ptr<ltl> m_formula{nullptr};
};

class ltl_until : public ltl
{
public:
    [[nodiscard]] kind get_kind() const final { return kind::until; }

    static std::shared_ptr<ltl> construct(bool is_release, std::shared_ptr<ltl> &&right, std::shared_ptr<ltl> &&left)
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
    std::shared_ptr<ltl> construct_negative() const final
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

private:
    explicit ltl_until(bool is_release, std::shared_ptr<ltl> left, std::shared_ptr<ltl> right)
            : ltl(is_release), m_left(std::move(left)), m_right(std::move(right))
    {
        assert(m_left && m_right && "Left and Right should be set");
    }

    const std::shared_ptr<ltl> m_left{nullptr};
    const std::shared_ptr<ltl> m_right{nullptr};
};

} // namespace ltl
