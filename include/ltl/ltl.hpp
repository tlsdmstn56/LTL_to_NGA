#pragma once

#include <cstdint>
#include <memory>
#include <string>

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
    explicit ltl(kind kind);

private:
    const kind m_kind {kind::undefined};
};

class ltl_one : public ltl
{
public:

    static node_t construct();

    friend bool operator== (const std::shared_ptr<ltl_one>& left, const std::shared_ptr<ltl_one>& right);

    [[nodiscard]] std::string to_string() const final;

private:
    explicit ltl_one();
};

class ltl_atom : public ltl
{
public:
    using index_atom_t = uint32_t;

    static node_t construct(index_atom_t index);

    friend bool operator== (const std::shared_ptr<ltl_atom>& left, const std::shared_ptr<ltl_atom>& right);

    [[nodiscard]] std::string to_string() const final;

    const index_atom_t m_index{0};

private:
    explicit ltl_atom(index_atom_t index);
};

class ltl_negation : public ltl
{
public:

    static node_t construct(node_t &&formula);

    friend bool operator== (const std::shared_ptr<ltl_negation>& left, const std::shared_ptr<ltl_negation>& right);

    [[nodiscard]] std::string to_string() const final;

    const node_t m_negformula{nullptr};

private:
    explicit ltl_negation(node_t formula);
};

class ltl_conjunction : public ltl
{
public:

    static node_t construct(node_t &&left, node_t &&right);

    friend bool operator== (const std::shared_ptr<ltl_conjunction>& left, const std::shared_ptr<ltl_conjunction>& right);

    [[nodiscard]] std::string to_string() const final;

    const node_t m_left{nullptr};
    const node_t m_right{nullptr};

private:
    explicit ltl_conjunction(node_t left, node_t right);
};

class ltl_next : public ltl
{
public:

    static node_t construct(node_t &&xformula);

    friend bool operator== (const std::shared_ptr<ltl_next>& left, const std::shared_ptr<ltl_next>& right);

    [[nodiscard]] std::string to_string() const final;

    const node_t m_xformula{nullptr};

private:
    explicit ltl_next(node_t formula);
};

class ltl_until : public ltl
{
public:

    static node_t construct(node_t &&left, node_t &&right);

    friend bool operator== (const std::shared_ptr<ltl_until>& left, const std::shared_ptr<ltl_until>& right);

    [[nodiscard]] std::string to_string() const final;

    const node_t m_left{nullptr};
    const node_t m_right{nullptr};

private:
    explicit ltl_until(node_t left, node_t right);
};

bool operator== (const ltl::node_t& left, const ltl::node_t& right);

} // namespace ltl
