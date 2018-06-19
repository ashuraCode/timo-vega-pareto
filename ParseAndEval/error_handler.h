#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "ast.h"


namespace client
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    using boost::phoenix::function;

    ///////////////////////////////////////////////////////////////////////////////
    //  Our error handler
    ///////////////////////////////////////////////////////////////////////////////
    struct error_handler_
    {
        template <typename, typename, typename>
        struct result { typedef void type; };

        template <typename Iterator>
        void operator()(
            qi::info const& what
          , Iterator err_pos, Iterator last) const
        {
            std::cout
                << "Error! Expecting "
                << what                         // what failed?
                << " here: \""
                << std::string(err_pos, last)   // iterators to error-pos, end
                << "\""
                << std::endl
            ;
        }
    };

    function<error_handler_> const error_handler = error_handler_();

}

#endif // ERROR_HANDLER_H
