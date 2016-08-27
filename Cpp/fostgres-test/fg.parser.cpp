/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"
#include <fost/parse/json.hpp>
#include <fost/push_back>


namespace {
}


fostlib::json fg::parse(const boost::filesystem::path &filename) {
    fostlib::string code(fostlib::utf::load_file(filename));
    fostlib::json script;
    fostlib::push_back(script, "progn");
    std::size_t line = 1u;

    auto space_p = boost::spirit::chlit<wchar_t>(L' ');
    auto newline_p = boost::spirit::chlit<wchar_t>(L'\n');
    auto string_p = +(boost::spirit::anychar_p - space_p - newline_p);
    fostlib::json_string_parser json_string_p;
    fostlib::json_parser json_p;

    auto result = boost::spirit::parse(code.c_str(),
        *newline_p
        >> *(
            string_p
                [([&line, &script](auto b, auto e) {
                    fostlib::push_back(script, line, fostlib::string(b, e));
                })]
            >> *boost::spirit::chlit<wchar_t>(L' ')
            >> *(
                (
                    json_p
                        [([&line, &script](auto j) {
                            fostlib::push_back(script, line, j);
                        })]
                    | (string_p
                        [([&line, &script](auto b, auto e) {
                            fostlib::push_back(script, line, fostlib::string(b, e));
                        })] >> *space_p)
                )
            ) >> (*newline_p)
        )
            [([&line, &script](auto, auto) {
                ++line;
            })]);
    if ( not result.full ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "Parse error", script);
    }
    return script;
}

