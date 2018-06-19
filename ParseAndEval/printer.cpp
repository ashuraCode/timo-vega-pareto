#include "printer.h"
#include "ast.h"


using namespace client::ast;


printer::printer(std::stringstream &_stream)
    : stream(_stream)
{

}

void printer::operator()(nil) const {

}

void printer::operator()(double n) const {
    stream << n;
}

void printer::operator()(variable const& x) const
{
    stream << x.name.c_str();
}

void printer::operator()(function_call const& fc) const
{
    stream << fc.function_name.name.c_str() << "( ";
    for (auto s = fc.args.begin();;) {
        (*this)(*s);
        if ((++s)!=fc.args.end())
            stream << " , ";
        else
            break;
    }
    stream << " )";
}

void printer::operator()(variable_declaration vd) const {
    stream << vd.var_name.name.c_str() << " = ";
    (*this)(vd.rhs);
    stream << ";\n";
}

void printer::operator()(operation const& x) const
{
    switch (x.operator_)
    {
        case '+': stream << "+"; break;
        case '-': stream << "-"; break;
        case '*': stream << "*"; break;
        case '/': stream << "/"; break;
        case '^': stream << "^"; break;
        case '=': stream << "="; break;
    }

    stream << "(";
    boost::apply_visitor(*this, x.operand_);
    stream << ")";
}

void printer::operator()(signed_ const& x) const
{
    switch (x.sign)
    {
        case '-': stream << "-"; break;
        case '+': stream << "+"; break;
    }
    boost::apply_visitor(*this, x.operand_);
}

void printer::operator()(equation const& x) const
{
    boost::apply_visitor(*this, x.first);
    BOOST_FOREACH(operation const& oper, x.rest)
    {
        (*this)(oper);
    }
}
