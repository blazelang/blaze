#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

class LexerStringLiteralTest : public LexerBaseTest {};

TEST_P(LexerStringLiteralTest, TokenizeStringLiterals) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerStringLiterals,
    LexerStringLiteralTest,
    testing::Values(
        LexerTestCase{
            .name = "EmptyInput",
            .fileID = 1,
            .source = "",
            .expectedTokens = {
                {TOK_EOF, {1, 1}, ""}
            }
        },

        LexerTestCase{
            .name = "EmptyString",
            .fileID = 1,
            .source = "\"\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"\""},
                {TOK_EOF, {1, 3}, ""}
            }
        },

        LexerTestCase{
            .name = "SimpleString",
            .fileID = 1,
            .source = "\"hello\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"hello\""},
                {TOK_EOF, {1, 8}, ""}
            }
        },

        LexerTestCase{
            .name = "StringWithSpaces",
            .fileID = 1,
            .source = "\"hello world\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"hello world\""},
                {TOK_EOF, {1, 14}, ""}
            }
        },

        LexerTestCase{
            .name = "StringWithEscapedQuote",
            .fileID = 1,
            .source = "\"he\\\"llo\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"he\\\"llo\""},
                {TOK_EOF, {1, 10}, ""}
            }
        },

        LexerTestCase{
            .name = "StringWithEscapeSequences",
            .fileID = 1,
            .source = "\"line1\\nline2\\tend\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"line1\\nline2\\tend\""},
                {TOK_EOF, {1, 20}, ""}
            }
        },

        LexerTestCase{
            .name = "UnterminatedString",
            .fileID = 1,
            .source = "\"unterminated",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "\"unterminated"},
                {TOK_EOF, {1, 14}, ""}
            }
        },

        LexerTestCase{
            .name = "UnescapedNewlineInString",
            .fileID = 1,
            .source = "\"hello\nworld\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"hello\nworld\""},
                {TOK_EOF, {2, 8}, ""}
            }
        },

        LexerTestCase{
            .name = "MultipleStringLiterals",
            .fileID = 1,
            .source = "\"foo\" \"bar\"",
            .expectedTokens = {
                {TOK_STRING_LITERAL, {1, 1}, "\"foo\""},
                {TOK_STRING_LITERAL, {1, 7}, "\"bar\""},
                {TOK_EOF, {1, 12}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
