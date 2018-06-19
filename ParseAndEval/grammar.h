#ifndef GRAMMAR_H
#define GRAMMAR_H

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#include "error_handler.h"


namespace client
{
    ///////////////////////////////////////////////////////////////////////////////
    //  Our calculator grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct calculator : qi::grammar<Iterator, ast::equation(), ascii::space_type>
    {
        calculator() : calculator::base_type(expr)
        {
            qi::char_type char_;
            qi::double_type double_;
            qi::_2_type _2;
            qi::_3_type _3;
            qi::_4_type _4;
            qi::raw_type raw;
            qi::lexeme_type lexeme;
            qi::alpha_type alpha;
            qi::alnum_type alnum;

            using qi::on_error;
            using qi::fail;

            expr =
                expression.alias()
                ;

            expression =
                term
                >> *(   (char_('+') > term)
                    |   (char_('-') > term)
                    )
                ;

            term =
                power
                >> *(   (char_('*') > power)
                    |   (char_('/') > power)
                    )
                ;

            power =
                factor
                >> *(   (char_('^') > factor)
                    )
                ;

            factor =
                    double_
                |   function_call
                |   variable_declaration
                |   identifier
                |   '(' > expression > ')'
                |   (char_('-') > factor)
                |   (char_('+') > factor)
                ;


            identifier =
                    raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

            function_call =
                    (identifier >> '(')
                >   argument_list
                >   ')'
                ;

            argument_list = (expr % ',');

            variable_declaration =
                    (identifier >> '=')
                >    expr
                >   ';'
                ;

            // Debugging and error handling and reporting support.
//            BOOST_SPIRIT_DEBUG_NODE(expr);
//            BOOST_SPIRIT_DEBUG_NODE(expression);
//            BOOST_SPIRIT_DEBUG_NODE(term);
//            BOOST_SPIRIT_DEBUG_NODE(power);
//            BOOST_SPIRIT_DEBUG_NODE(variable_declaration);
//            BOOST_SPIRIT_DEBUG_NODE(factor);
//            BOOST_SPIRIT_DEBUG_NODE(identifier);
//            BOOST_SPIRIT_DEBUG_NODE(function_call);
//            BOOST_SPIRIT_DEBUG_NODE(argument_list);

            // Error handling
            on_error<fail>(expression, error_handler(_4, _3, _2));
        }

        qi::rule<Iterator, ast::equation(), ascii::space_type> expr;
        qi::rule<Iterator, ast::equation(), ascii::space_type> expression;
        qi::rule<Iterator, ast::equation(), ascii::space_type> term;
        qi::rule<Iterator, ast::equation(), ascii::space_type> power;
        qi::rule<Iterator, ast::variable_declaration(), ascii::space_type> variable_declaration;
        qi::rule<Iterator, ast::operand(), ascii::space_type> factor;
        qi::rule<Iterator, std::string(), ascii::space_type> identifier;

        qi::rule<Iterator, ast::function_call(), ascii::space_type > function_call;
        qi::rule<Iterator, std::list<ast::equation>(), ascii::space_type > argument_list;
    };
}

#endif // GRAMMAR_H
