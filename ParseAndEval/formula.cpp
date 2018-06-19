#include "formula.h"
#include "compilator.h"
#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <QDebug>

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

using namespace client::code_gen;


static unsigned short constants = 1;
Formula::Formula(unsigned short stackSize)
    :   m_vStack(stackSize)
{
    clear();
    m_vVars.reserve(1024);
    m_vConst.reserve(1024);
    m_vCode.reserve(1024);
}

void Formula::op(unsigned short a)
{
    m_vCode.push_back(a);
}

void Formula::op(unsigned short a, unsigned short b)
{
    m_vCode.push_back(a);
    m_vCode.push_back(b);
}

void Formula::op(unsigned short a, double b)
{
    m_vCode.push_back(a);
    m_vCode.push_back(m_vConst.size());
    m_vConst.push_back(b);
}

void Formula::op(unsigned short a, unsigned short b, unsigned short c)
{
    m_vCode.push_back(a);
    m_vCode.push_back(b);
    m_vCode.push_back(c);
}

void Formula::clear()
{
    m_vCode.clear(); m_mVariables.clear();m_vVars.clear();m_vConst.clear();
    m_mVariables["pi"] = 0;
    m_vVars.push_back(M_PI);
}

void Formula::clearVariables(double set)
{
    for (auto &s : m_mVariablesMap) {
        s.second.get() = set;
    }
}

Formula::VariableMap &Formula::variables()
{
    return m_mVariablesMap;
}

unsigned short const* Formula::find_var(std::string const& name) const
{
    std::map<std::string, unsigned short>::const_iterator i = m_mVariables.find(name);
    if (i == m_mVariables.end())
        return 0;
    return &i->second;
}

void Formula::add_var(std::string const& name)
{
    std::size_t n = m_mVariables.size();
    if (m_mVariables.find(name)!=m_mVariables.end())
        return;
    m_mVariables[name] = n;
    m_mVariablesMap.insert(std::pair<std::string, std::reference_wrapper<double>>{name, m_vVars[n]});
    m_vVars.resize(n+1);
}

void Formula::print_variables(std::stringstream &stream)
{
    typedef std::pair<std::string, unsigned short> pair;
    unsigned short it=0;
    BOOST_FOREACH(pair const& p, m_mVariables)
    {
        if (it++>=constants)
            stream << p.first.c_str() << ": " <<  m_vStack[p.second] << "\n";
    }
}

void Formula::print_assembler(std::stringstream &stream)
{
    std::vector<unsigned short>::const_iterator pc = m_vCode.begin();

    std::vector<std::string> locals(m_mVariables.size());
    typedef std::pair<std::string, int> pair;
    stream << "section .data\n";
    BOOST_FOREACH(pair const& p, m_mVariables)
    {
        locals[p.second] = p.first;
        stream << "\tlocal\t"
            << p.first.c_str() << ", @" << p.second << "\n";
    }

    stream << "\n" << "_start:" << "\n";

    std::map<std::size_t, std::string> lines;

    while (pc != m_vCode.end())
    {
        std::string line;
        std::size_t address = pc - m_vCode.begin();

        switch (*pc++)
        {
            case op_neg:
                line += "\t_neg";
                break;

            case op_not:
                line += "\t_not";
                break;

            case op_add:
                line += "\t_add";
                break;

            case op_sub:
                line += "\t_sub";
                break;

            case op_mul:
                line += "\t_mul";
                break;

            case op_div:
                line += "\t_div";
                break;

            case op_load:
                line += "\t_load\t";
                line += boost::lexical_cast<std::string>(locals[*pc++]);
                break;

            case op_store:
                line += "\t_store\t";
                line += boost::lexical_cast<std::string>(locals[*pc++]);
                break;

            case op_double:
                line += "\t_double\t";
                line += boost::lexical_cast<std::string>(m_vConst[*pc++]);
                break;

            case op_stk_adj:
                line += "\t_stk_adj\t";
                line += boost::lexical_cast<std::string>(*pc++);
                break;

            case op_exp  :
                line += "\t_exp";
                break;
            case op_exp2 :
                line += "\t_exp2";
                break;
            case op_expm1:
                line += "\t_expm1";
                break;
            case op_log  :
                line += "\t_log";
                break;
            case op_log10:
                line += "\t_log10";
                break;
            case op_log2 :
                line += "\t_log2";
                break;
            case op_log1p:
                line += "\t_log1p";
                break;
            case op_pow  :
                line += "\t_pow";
                break;
            case op_sqrt :
                line += "\t_sqrt";
                break;
            case op_cbrt :
                line += "\t_cbrt";
                break;
            case op_hypot:
                line += "\t_hypot";
                break;
            case op_sin  :
                line += "\t_sin";
                break;
            case op_cos  :
                line += "\t_cos";
                break;
            case op_tan  :
                line += "\t_tan";
                break;
            case op_sinr  :
                line += "\t_sinr";
                break;
            case op_cosr  :
                line += "\t_cosr";
                break;
            case op_tanr  :
                line += "\t_tanr";
                break;
            case op_asin :
                line += "\t_asin";
                break;
            case op_acos :
                line += "\t_acos";
                break;
            case op_atan :
                line += "\t_atan";
                break;
            case op_asinr :
                line += "\t_asinr";
                break;
            case op_acosr :
                line += "\t_acosr";
                break;
            case op_atanr :
                line += "\t_atanr";
                break;
            case op_atan2:
                line += "\t_atan2";
                break;
            case op_sinh :
                line += "\t_sinh";
                break;
            case op_cosh :
                line += "\t_cosh";
                break;
            case op_tanh :
                line += "\t_tanh";
                break;
            case op_asinh:
                line += "\t_asinh";
                break;
            case op_acosh:
                line += "\t_acosh";
                break;
            case op_atanh:
                line += "\t_atanh";
                break;
        }
        lines[address] = line;
    }

    for (auto &s : lines) {
        stream << s.second.c_str() << "\n";
    }
}

int Formula::eval()
{
    std::vector<unsigned short>::const_iterator pc = m_vCode.begin();
    std::vector<double>::iterator stack_ptr = m_vStack.begin();

    while (pc != m_vCode.end())
    {
        BOOST_ASSERT(pc != m_vCode.end());

        switch (*pc++)
        {
            case op_neg:
                stack_ptr[-1] = -stack_ptr[-1];
                break;

            case op_not:
                stack_ptr[-1] = !bool(stack_ptr[-1]);
                break;

            case op_add:
                --stack_ptr;
                stack_ptr[-1] += stack_ptr[0];
                break;

            case op_sub:
                --stack_ptr;
                stack_ptr[-1] -= stack_ptr[0];
                break;

            case op_mul:
                --stack_ptr;
                stack_ptr[-1] *= stack_ptr[0];
                break;

            case op_div:
                --stack_ptr;
                stack_ptr[-1] /= stack_ptr[0];
                break;

            case op_load:
                *stack_ptr++ = m_vVars[*pc++];
                break;

            case op_store:
                --stack_ptr;
                m_vVars[*pc++] = stack_ptr[0];
                break;

            case op_double:
                *stack_ptr++ = m_vConst[*pc++];
                break;

            case op_stk_adj:
                stack_ptr = m_vStack.begin() + *pc++;
                break;

            case op_exp  :
                stack_ptr[-1] = exp(stack_ptr[-1]);
                break;

            case op_exp2 :
                stack_ptr[-1] = exp2(stack_ptr[-1]);
                break;

            case op_expm1:
                stack_ptr[-1] = expm1(stack_ptr[-1]);
                break;

            case op_log  :
                stack_ptr[-1] = log(stack_ptr[-1]);
                break;

            case op_log10:
                stack_ptr[-1] = log10(stack_ptr[-1]);
                break;

            case op_log2 :
                stack_ptr[-1] = log2(stack_ptr[-1]);
                break;

            case op_log1p:
                stack_ptr[-1] = log1p(stack_ptr[-1]);
                break;

            case op_pow  :
                --stack_ptr;
                stack_ptr[-1] = pow(stack_ptr[-1], stack_ptr[0]);
                break;

            case op_sqrt :
                stack_ptr[-1] = sqrt(stack_ptr[-1]);
                break;

            case op_cbrt :
                stack_ptr[-1] = cbrt(stack_ptr[-1]);
                break;

            case op_hypot:
                --stack_ptr;
                stack_ptr[-1] = hypot(stack_ptr[-1], stack_ptr[0]);
                break;

            case op_sin  :
                stack_ptr[-1] = sin(stack_ptr[-1] * (M_PI / 180.0));
                break;

            case op_cos  :
                stack_ptr[-1] = cos(stack_ptr[-1] * (M_PI / 180.0));
                break;

            case op_tan  :
                stack_ptr[-1] = tan(stack_ptr[-1] * (M_PI / 180.0));
                break;

            case op_sinr  :
                stack_ptr[-1] = sin(stack_ptr[-1]);
                break;

            case op_cosr  :
                stack_ptr[-1] = cos(stack_ptr[-1]);
                break;

            case op_tanr  :
                stack_ptr[-1] = tan(stack_ptr[-1]);
                break;

            case op_asin :
                stack_ptr[-1] = asin(stack_ptr[-1]) * (180.0 / M_PI);
                break;

            case op_acos :
                stack_ptr[-1] = acos(stack_ptr[-1]) * (180.0 / M_PI);
                break;

            case op_atan :
                stack_ptr[-1] = atan(stack_ptr[-1]) * (180.0 / M_PI);
                break;

            case op_asinr :
                stack_ptr[-1] = asin(stack_ptr[-1]);
                break;

            case op_acosr :
                stack_ptr[-1] = acos(stack_ptr[-1]);
                break;

            case op_atanr :
                stack_ptr[-1] = atan(stack_ptr[-1]);
                break;

            case op_atan2:
                --stack_ptr;
                stack_ptr[-1] = atan2(stack_ptr[-1], stack_ptr[0]);
                break;

            case op_sinh :
                stack_ptr[-1] = sinh(stack_ptr[-1]);
                break;

            case op_cosh :
                stack_ptr[-1] = cosh(stack_ptr[-1]);
                break;

            case op_tanh :
                stack_ptr[-1] = tanh(stack_ptr[-1]);
                break;

            case op_asinh:
                stack_ptr[-1] = asinh(stack_ptr[-1]);
                break;

            case op_acosh:
                stack_ptr[-1] = acosh(stack_ptr[-1]);
                break;

            case op_atanh:
                stack_ptr[-1] = atanh(stack_ptr[-1]);
                break;
        }
    }
    return -1;
}
