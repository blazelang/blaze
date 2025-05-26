#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

class LexerCharLiteralTest : public LexerBaseTest {};

TEST_P(LexerCharLiteralTest, TokenizeCharLiterals) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerCharLiterals,
    LexerCharLiteralTest,
    testing::Values(
        LexerTestCase{
            .name = "SimpleChar",
            .fileID = 1,
            .source = "'a'",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "'a'"},
                {TOK_EOF, {1, 4}, ""}
            }
        },

        // Escaped characters
        LexerTestCase{
            .name = "EscapedNewline",
            .fileID = 1,
            .source = "'\\n'",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "'\\n'"},
                {TOK_EOF, {1, 5}, ""}
            }
        },
        LexerTestCase{
            .name = "EscapedBackslash",
            .fileID = 1,
            .source = "'\\\\'",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "'\\\\'"},
                {TOK_EOF, {1, 5}, ""}
            }
        },

        // Hex escape
        LexerTestCase{
            .name = "HexEscape",
            .fileID = 1,
            .source = "'\\x41'",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "'\\x41'"},
                {TOK_EOF, {1, 7}, ""}
            }
        },

        // Unicode escape
        LexerTestCase{
            .name = "UnicodeEscapeShort",
            .fileID = 1,
            .source = "'\\u0041'",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "'\\u0041'"},
                {TOK_EOF, {1, 9}, ""}
            }
        },
        LexerTestCase{
            .name = "UnicodeEscapeLong",
            .fileID = 1,
            .source = "'\\U00000041'",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "'\\U00000041'"},
                {TOK_EOF, {1, 13}, ""}
            }
        },

        // Multi-character literal
        LexerTestCase{
            .name = "MultiCharLiteral",
            .fileID = 1,
            .source = "'ab'",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "'ab'"},
                {TOK_EOF, {1, 5}, ""}
            }
        },

        // Unterminated literal
        LexerTestCase{
            .name = "UnterminatedCharLiteral",
            .fileID = 1,
            .source = "'a",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "'a"},
                {TOK_EOF, {1, 3}, ""}
            }
        },

        // Empty literal
        LexerTestCase{
            .name = "EmptyCharLiteral",
            .fileID = 1,
            .source = "''",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "''"},
                {TOK_EOF, {1, 3}, ""}
            }
        },

        // Invalid escape
        LexerTestCase{
            .name = "InvalidEscape",
            .fileID = 1,
            .source = "'\\q'",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "'\\q'"},
                {TOK_EOF, {1, 5}, ""}
            }
        },

        // Character with space
        LexerTestCase{
            .name = "CharWithSpace",
            .fileID = 1,
            .source = "' '",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "' '"},
                {TOK_EOF, {1, 4}, ""}
            }
        },

        // Char with tab
        LexerTestCase{
            .name = "CharTabEscape",
            .fileID = 1,
            .source = "'\\t'",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "'\\t'"},
                {TOK_EOF, {1, 5}, ""}
            }
        },

        // Unicode character literal
        LexerTestCase{
            .name = "UnicodeCharLiteral",
            .fileID = 1,
            .source = "'ñ'",
            .expectedTokens = {
                {TOK_CHAR_LITERAL, {1, 1}, "'ñ'"},
                {TOK_EOF, {1, 4}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
