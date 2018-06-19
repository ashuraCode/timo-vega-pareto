#ifndef PRINTER_H
#define PRINTER_H

#include <sstream>


namespace client {
namespace ast {

    struct nil;
    struct variable;
    struct function_call;
    struct variable_declaration;
    struct operation;
    struct signed_;
    struct equation;

    struct printer
    {
        std::stringstream &stream;
        typedef void result_type;

        printer(std::stringstream &_stream);

        void operator()(nil) const;
        void operator()(double n) const;
        void operator()(variable const& x) const;
        void operator()(function_call const& fc) const;
        void operator()(variable_declaration vd) const;
        void operator()(operation const& x) const;
        void operator()(signed_ const& x) const;
        void operator()(equation const& x) const;
    };

}}

#endif // PRINTER_H
