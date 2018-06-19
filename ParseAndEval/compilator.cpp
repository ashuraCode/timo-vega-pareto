#include "compilator.h"

using namespace client::ast;
using namespace client::code_gen;
typedef client::code_gen::Formula::byte_code byte_code;


const std::map<std::string, byte_code> compilator::funkcje =
{
    { "exp",		byte_code::op_exp   },
    { "exp2",		byte_code::op_exp2  },
    { "expm1",		byte_code::op_expm1 },
    { "log",		byte_code::op_log   },
    { "log10",		byte_code::op_log10 },
    { "log2",		byte_code::op_log2  },
    { "log1p",		byte_code::op_log1p },
    { "pow",		byte_code::op_pow   },
    { "sqrt",		byte_code::op_sqrt  },
    { "cbrt",		byte_code::op_cbrt  },
    { "hypot",		byte_code::op_hypot },
    { "sin",		byte_code::op_sin   },
    { "cos",		byte_code::op_cos   },
    { "tan",		byte_code::op_tan   },
    { "sinr",		byte_code::op_sinr   },
    { "cosr",		byte_code::op_cosr   },
    { "tanr",		byte_code::op_tanr   },
    { "asin",		byte_code::op_asin  },
    { "acos",		byte_code::op_acos  },
    { "atan",		byte_code::op_atan  },
    { "asinr",		byte_code::op_asinr  },
    { "acosr",		byte_code::op_acosr  },
    { "atanr",		byte_code::op_atanr  },
    { "atan2",		byte_code::op_atan2 },
    { "sinh",		byte_code::op_sinh  },
    { "cosh",		byte_code::op_cosh  },
    { "tanh",		byte_code::op_tanh  },
    { "asinh",		byte_code::op_asinh },
    { "acosh",		byte_code::op_acosh },
    { "atanh",		byte_code::op_atanh }

};

compilator::compilator(Formula &prog) : program(prog)
{

}

void compilator::operator()(nil) const {
    BOOST_ASSERT(0);
}

void compilator::operator()(double x) const {
    program.op(byte_code::op_double, x);
}

void compilator::operator()(variable x) const {
    program.add_var(x.name);
    unsigned short const* p = program.find_var(x.name);
    program.op(byte_code::op_load, (unsigned short)*p);
}

void compilator::operator()(function_call fc) const {
    for (auto s : fc.args) {
        (*this)(s);
    }
    program.op(funkcje.at(fc.function_name.name));
}

void compilator::operator()(variable_declaration vd) const {
    program.add_var(vd.var_name.name);
    unsigned short const* p = program.find_var(vd.var_name.name);
    program.op(byte_code::op_load, *p);
    (*this)(vd.rhs);
    program.op(byte_code::op_store, *p);
}

void compilator::operator()(operation const& x) const
{
    boost::apply_visitor(*this, x.operand_);

    switch (x.operator_)
    {
        case '+': program.op(byte_code::op_add); break;
        case '-': program.op(byte_code::op_sub); break;
        case '*': program.op(byte_code::op_mul); break;
        case '/': program.op(byte_code::op_div); break;
        case '^': program.op(byte_code::op_pow); break;

        default: BOOST_ASSERT(0);
    }
}

void compilator::operator()(signed_ const& x) const
{
    boost::apply_visitor(*this, x.operand_);
    switch (x.sign)
    {
        case '-': program.op(byte_code::op_neg); break;
        case '+': break;
        default: BOOST_ASSERT(0);
    }
}

void compilator::operator()(equation const& x) const
{
    boost::apply_visitor(*this, x.first);
    BOOST_FOREACH(operation const& oper, x.rest)
    {
        (*this)(oper);
    }
}
