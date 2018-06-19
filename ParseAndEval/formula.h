#ifndef FORMULA_H
#define FORMULA_H

#include <map>
#include <vector>
#include <functional>


namespace client {
namespace ast {
class compilator;
}}


namespace client {
namespace code_gen {

class Formula
{
public:
    typedef std::map<std::string, std::reference_wrapper<double>> VariableMap;

    enum byte_code
    {
        op_neg,         //  negate the top stack entry
        op_add,         //  add top two stack entries
        op_sub,         //  subtract top two stack entries
        op_mul,         //  multiply top two stack entries
        op_div,         //  divide top two stack entries

        op_plus,
        op_minus,

        op_not,         //  boolean negate the top stack entry
        op_positive,         //  boolean negate the top stack entry

        op_load,        //  load a variable
        op_store,       //  store a variable

        op_double,         //  push constant integer into the stack

        op_stk_adj,     // adjust the stack (for args and locals)

        op_exp  ,
        op_exp2 ,
        op_expm1,
        op_log  ,
        op_log10,
        op_log2 ,
        op_log1p,
        op_pow  ,
        op_sqrt ,
        op_cbrt ,
        op_hypot,
        op_sin  ,
        op_cos  ,
        op_tan  ,
        op_sinr  ,
        op_cosr  ,
        op_tanr  ,
        op_asin ,
        op_acos ,
        op_atan ,
        op_atan2,
        op_asinr ,
        op_acosr ,
        op_atanr ,
        op_sinh ,
        op_cosh ,
        op_tanh ,
        op_asinh,
        op_acosh,
        op_atanh
    };


public:
    Formula(unsigned short stackSize = 16384);

    void clear();
    void clearVariables(double set = 0.0);

    std::vector<unsigned short> const& operator()() const { return m_vCode; }

    VariableMap &variables();

    void print_variables(std::stringstream &stream);
    void print_assembler(std::stringstream &stream);

    int eval();


private:
    friend class client::ast::compilator; // friend class forward declaration (elaborated class name)

    void op(unsigned short a);
    void op(unsigned short a, unsigned short b);
    void op(unsigned short a, double b);
    void op(unsigned short a, unsigned short b, unsigned short c);

    unsigned short const* find_var(std::string const& name) const;
    void add_var(std::string const& name);

    std::map<std::string, unsigned short> m_mVariables;
    std::map<std::string, std::reference_wrapper<double>> m_mVariablesMap;
    std::vector<unsigned short> m_vCode;
    std::vector<double> m_vStack;
    std::vector<double> m_vVars;
    std::vector<double> m_vConst;
};

}}

#endif // FORMULA_H
