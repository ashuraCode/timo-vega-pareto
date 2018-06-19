#include "evaluator.h"
#include "grammar.h"
#include "compilator.h"
#include <boost/algorithm/string.hpp>
#include <sstream>


Evaluator::Evaluator()
{

}

std::string Evaluator::compile(std::list<std::string> &equations)
{
    // Definicja typów dla uproszczenia
    typedef std::string::const_iterator iterator_type;
    typedef client::calculator<iterator_type> calculator;
    typedef client::ast::equation ast_equation;
    typedef client::ast::printer ast_print;
    typedef client::ast::compilator ast_compilator;


    std::stringstream codeStream;
    std::stringstream progStream;

    m_Formula.clear();
    m_form.clear();
    m_code.clear();

    ast_print print(codeStream);    // Prints the program
    ast_compilator eval(m_Formula);   // Evaluates the program


    try {
        for (auto sInstr : equations) {
            if (sInstr.empty()) continue;

            std::string str = sInstr;
            boost::algorithm::trim_left(str);
            boost::algorithm::trim_right(str);

            if (std::find(str.begin(), str.end(), '=') != std::end(str)) {
                if (str.back()!=';')
                    str.push_back(';');
            } else {
                if (str.back()==';')
                    str.pop_back();
            }

            calculator calc;                // Our grammar
            ast_equation equation;          // Our program (AST)
            std::string::const_iterator iter = str.begin();
            std::string::const_iterator end = str.end();
            boost::spirit::ascii::space_type space;
            bool r = phrase_parse(iter, end, calc, space, equation);

            if (r && iter == end)
            {
                print(equation);
                eval(equation);
            }
            else
            {
                std::string rest(iter, end);
                return rest;
            }
        }
    } catch (std::string &err) {
        return err;
    } catch (...){}

    m_Formula.print_assembler(progStream);
    m_code = progStream.str();
    m_form = codeStream.str();

    return "";
}

std::string Evaluator::vars()
{
    std::stringstream varStream;
    varStream.precision(20);

    m_Formula.print_variables(varStream);

    return varStream.str();
}

std::string &Evaluator::getEquation()
{
    return m_form;
}

std::string &Evaluator::getAsmCode()
{
    return m_code;
}

client::code_gen::Formula &Evaluator::getFormula()
{
    return m_Formula;
}
