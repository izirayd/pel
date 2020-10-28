#include <pel.hpp>

void test_1()
{
    pel::pel_parser_t pel_parser;

    pel_parser.code = R"(
  
        group space   : { " " }  = execute, ignore;
        group symbol  : { ";," } = execute, split;
        group word    : { !space and !symbol } = execute, glue;

        type a : { "a", ";" } = execute;
    )";

    pel_parser.compile();

    pel_parser.push_text("a;");
    pel_parser.push_text("b;");

    pel_parser.run();

    pel_parser.get_ast();

    pel_parser.clear();
    pel_parser.clear_pe();
}

int main() 
{
    test_1();
}