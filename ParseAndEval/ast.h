#ifndef AST_H
#define AST_H

#include "boost_qi.h"

namespace client {
namespace ast {
    ///////////////////////////////////////////////////////////////////////////
    //  The AST
    ///////////////////////////////////////////////////////////////////////////
    struct tagged
    {
        int id; // Used to annotate the AST with the iterator position.
                // This id is used as a key to a map<int, Iterator>
                // (not really part of the AST.)
    };
    struct nil {};
    struct signed_;
    struct equation;
    struct function_call;
    struct variable_declaration;

    struct variable : tagged
    {
        variable(std::string const& name = "") : name(name) {}
        std::string name;
    };

    typedef boost::variant<
            nil
          , double
          , variable
          , boost::recursive_wrapper<signed_>
          , boost::recursive_wrapper<function_call>
          , boost::recursive_wrapper<equation>
          , boost::recursive_wrapper<variable_declaration>
        >
    operand;

    struct signed_
    {
        char sign;
        operand operand_;
    };

    struct operation
    {
        char operator_;
        operand operand_;
    };

    struct equation
    {
        operand first;
        std::list<operation> rest;
    };

    struct function_call
    {
        variable function_name;
        std::list<equation> args;
    };

    struct variable_declaration
    {
        variable var_name;
        equation rhs;
    };

    // print function for debugging
    inline std::ostream& operator<<(std::ostream& out, nil) { out << "nil"; return out; }
    inline std::ostream& operator<<(std::ostream& out, variable const& var) { out << var.name; return out; }
}}

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::signed_,
    (char, sign)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::operation,
    (char, operator_)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::equation,
    (client::ast::operand, first)
    (std::list<client::ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::function_call,
    (client::ast::variable, function_name)
    (std::list<client::ast::equation>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::variable_declaration,
    (client::ast::variable, var_name)
    (client::ast::equation, rhs)
)


#endif // AST_H
