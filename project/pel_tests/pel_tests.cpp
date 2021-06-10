#include <pel.hpp>

void test_1()
{
    pel::pel_parser_t pel_parser;

    pel_parser.code = R"(
  
        group space   : { " " }  = exists, ignore;
        group symbol  : { ";," } = exists, split;
        group word    : { !space and !symbol } = exists, glue;

        type a : { "a", ";" } = exists;
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