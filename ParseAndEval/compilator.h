#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "formula.h"
#include "error_handler.h"


namespace client { namespace ast
{

    ///////////////////////////////////////////////////////////////////////////
    //  The AST evaluator
    ///////////////////////////////////////////////////////////////////////////
    struct compilator
    {
        typedef void result_type;
        client::code_gen::Formula &program;

        compilator(client::code_gen::Formula &prog);

        void operator()(nil) const;
        void operator()(double n) const;
        void operator()(variable n) const;
        void operator()(function_call fc) const;

        void operator()(variable_declaration vd) const;

        void operator()(operation const& x) const;

        void operator()(signed_ const& x) const;

        void operator()(equation const& x) const;

    private:
        static const std::map<std::string, client::code_gen::Formula::byte_code> funkcje;
    };

}}
#endif // EVAL_H
