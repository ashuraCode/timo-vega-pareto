#ifndef Evaluator_H
#define Evaluator_H

#include <list>
#include "printer.h"
#include "formula.h"


class Evaluator
{
public:
    Evaluator();

    std::string compile(std::list<std::string> &equations);
    std::string vars();
    std::string &getEquation();
    std::string &getAsmCode();

    client::code_gen::Formula &getFormula();

private:
    std::string m_form;
    std::string m_code;

    client::code_gen::Formula m_Formula;
};

#endif // Evaluator_H
