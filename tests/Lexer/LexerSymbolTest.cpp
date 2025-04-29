#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

TEST_P(LexerBaseTest, TokenizeSymbols) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerSymbolTest,
    LexerBaseTest,
    testing::Values(
        // Basic symbols
        LexerTestCase{
            .name = "SingleSymbols",
            .fileID = 1,
            .source = ", : ; . ~ ( ) { } [ ] ? = + - * / % & | ^ < > !",
            .expectedTokens = {
                {TOK_COMMA, {1, 1}, ","},
                {TOK_COLON, {1, 3}, ":"},
                {TOK_SEMICOLON, {1, 5}, ";"},
                {TOK_DOT, {1, 7}, "."},
                {TOK_BITWISE_NOT, {1, 9}, "~"},
                {TOK_LPAREN, {1, 11}, "("},
                {TOK_RPAREN, {1, 13}, ")"},
                {TOK_LBRACE, {1, 15}, "{"},
                {TOK_RBRACE, {1, 17}, "}"},
                {TOK_LBRACKET, {1, 19}, "["},
                {TOK_RBRACKET, {1, 21}, "]"},
                {TOK_TERNARY_CONDITIONAL, {1, 23}, "?"},
                {TOK_ASSIGN, {1, 25}, "="},
                {TOK_PLUS, {1, 27}, "+"},
                {TOK_MINUS, {1, 29}, "-"},
                {TOK_MULTIPLY, {1, 31}, "*"},
                {TOK_DIVIDE, {1, 33}, "/"},
                {TOK_MODULO, {1, 35}, "%"},
                {TOK_BITWISE_AND, {1, 37}, "&"},
                {TOK_BITWISE_OR, {1, 39}, "|"},
                {TOK_XOR_ASSIGN, {1, 41}, "^"},
                {TOK_LESS_THAN, {1, 43}, "<"},
                {TOK_GREATER_THAN, {1, 45}, ">"},
                {TOK_LOGICAL_NOT, {1, 47}, "!"},
                {TOK_EOF, {1, 48}, ""}
            }
        },

        // Compound symbols (two characters)
        LexerTestCase{
            .name = "CompoundSymbols",
            .fileID = 2,
            .source = "== += ++ -= -- *= /= %= &= && |= || ^= <= << >= >> ",
            .expectedTokens = {
                {TOK_EQUAL, {1, 1}, "=="},
                {TOK_PLUS_ASSIGN, {1, 4}, "+="},
                {TOK_INCREMENT, {1, 7}, "++"},
                {TOK_MINUS_ASSIGN, {1, 10}, "-="},
                {TOK_DECREMENT, {1, 13}, "--"},
                {TOK_MULTIPLY_ASSIGN, {1, 16}, "*="},
                {TOK_DIVIDE_ASSIGN, {1, 19}, "/="},
                {TOK_MODULO_ASSIGN, {1, 22}, "%="},
                {TOK_AND_ASSIGN, {1, 25}, "&="},
                {TOK_LOGICAL_AND, {1, 28}, "&&"},
                {TOK_OR_ASSIGN, {1, 31}, "|="},
                {TOK_LOGICAL_OR, {1, 34}, "||"},
                {TOK_XOR_ASSIGN, {1, 37}, "^="},
                {TOK_LESS_EQUAL, {1, 40}, "<="},
                {TOK_LEFT_SHIFT, {1, 43}, "<<"},
                {TOK_GREATER_EQUAL, {1, 46}, ">="},
                {TOK_RIGHT_SHIFT, {1, 49}, ">>"},
                {TOK_EOF, {1, 52}, ""}
            }
        },

        // Compound symbols (three characters)
        LexerTestCase{
            .name = "TripleCompoundSymbols",
            .fileID = 3,
            .source = "<<= >>=",
            .expectedTokens = {
                {TOK_LEFT_SHIFT_ASSIGN, {1, 1}, "<<="},
                {TOK_RIGHT_SHIFT_ASSIGN, {1, 5}, ">>="},
                {TOK_EOF, {1, 8}, ""}
            }
        },

        // Greedy matching test
        LexerTestCase{
            .name = "GreedyMatching",
            .fileID = 4,
            .source = "< << <<= > >> >>=",
            .expectedTokens = {
                {TOK_LESS_THAN, {1, 1}, "<"},
                {TOK_LEFT_SHIFT, {1, 3}, "<<"},
                {TOK_LEFT_SHIFT_ASSIGN, {1, 6}, "<<="},
                {TOK_GREATER_THAN, {1, 10}, ">"},
                {TOK_RIGHT_SHIFT, {1, 12}, ">>"},
                {TOK_RIGHT_SHIFT_ASSIGN, {1, 15}, ">>="},
                {TOK_EOF, {1, 18}, ""}
            }
        },

        // Mixed symbols
        LexerTestCase{
            .name = "MixedSymbols",
            .fileID = 5,
            .source = "=>(){}<>=!==++-*/%",
            .expectedTokens = {
                {TOK_ASSIGN, {1, 1}, "="},
                {TOK_GREATER_THAN, {1, 2}, ">"},
                {TOK_LPAREN, {1, 3}, "("},
                {TOK_RPAREN, {1, 4}, ")"},
                {TOK_LBRACE, {1, 5}, "{"},
                {TOK_RBRACE, {1, 6}, "}"},
                {TOK_LESS_THAN, {1, 7}, "<"},
                {TOK_GREATER_EQUAL, {1, 8}, ">="},
                {TOK_LOGICAL_NOT, {1, 10}, "!"},
                {TOK_EQUAL, {1, 11}, "=="},
                {TOK_INCREMENT, {1, 13}, "++"},
                {TOK_MINUS, {1, 15}, "-"},
                {TOK_MULTIPLY, {1, 16}, "*"},
                {TOK_DIVIDE, {1, 17}, "/"},
                {TOK_MODULO, {1, 18}, "%"},
                {TOK_EOF, {1, 19}, ""}
            }
        },

        // Error case - Invalid symbol
        LexerTestCase{
            .name = "InvalidSymbol",
            .fileID = 6,
            .source = "@ # $",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "@"},
                {TOK_ERROR, {1, 3}, "#"},
                {TOK_ERROR, {1, 5}, "$"},
                {TOK_EOF, {1, 6}, ""}
            }
        },

        // Empty source
        LexerTestCase{
            .name = "EmptySource",
            .fileID = 7,
            .source = "",
            .expectedTokens = {
                {TOK_EOF, {1, 1}, ""}
            }
        },

        // Complex combinations
        LexerTestCase{
            .name = "ComplexCombinations",
            .fileID = 8,
            .source = "+=-->&<<=||&&==!=++--",
            .expectedTokens = {
                {TOK_PLUS_ASSIGN, {1, 1}, "+="},
                {TOK_DECREMENT, {1, 3}, "--"},
                {TOK_GREATER_THAN, {1, 5}, ">"},
                {TOK_BITWISE_AND, {1, 6}, "&"},
                {TOK_LEFT_SHIFT_ASSIGN, {1, 7}, "<<="},
                {TOK_LOGICAL_OR, {1, 10}, "||"},
                {TOK_LOGICAL_AND, {1, 12}, "&&"},
                {TOK_EQUAL, {1, 14}, "=="},
                {TOK_LOGICAL_NOT, {1, 16}, "!"},
                {TOK_ASSIGN, {1, 17}, "="},
                {TOK_INCREMENT, {1, 18}, "++"},
                {TOK_DECREMENT, {1, 20}, "--"},
                {TOK_EOF, {1, 22}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
