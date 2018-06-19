#ifndef BOOST_QI_H
#define BOOST_QI_H

// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Define this to enable debugging
#define BOOST_SPIRIT_QI_DEBUG

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debugging
//#define BOOST_SPIRIT_QI_DEBUG
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
# pragma warning(disable: 4345)
#endif


#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/foreach.hpp>

#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>

#endif // BOOST_QI_H
