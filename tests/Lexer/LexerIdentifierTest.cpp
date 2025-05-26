#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

class LexerIdentifierTest : public LexerBaseTest {};

TEST_P(LexerIdentifierTest, TokenizeIdentifiers) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerIdentifiers,
    LexerIdentifierTest,
    testing::Values(
        // Basic ASCII identifiers
        LexerTestCase{
            .name = "BasicIdentifier",
            .fileID = 1,
            .source = "foo",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "foo"},
                {TOK_EOF, {1, 4}, ""}
            }
        },
        LexerTestCase{
            .name = "UnderscoreIdentifier",
            .fileID = 1,
            .source = "_bar123",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "_bar123"},
                {TOK_EOF, {1, 8}, ""}
            }
        },

        // Adjacent identifiers
        LexerTestCase{
            .name = "MultipleIdentifiers",
            .fileID = 1,
            .source = "foo bar",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "foo"},
                {TOK_IDENTIFIER, {1, 5}, "bar"},
                {TOK_EOF, {1, 8}, ""}
            }
        },

        // Mixed with numbers (but not starting with one)
        LexerTestCase{
            .name = "IdentifierWithNumbers",
            .fileID = 1,
            .source = "foo123",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "foo123"},
                {TOK_EOF, {1, 7}, ""}
            }
        },

        // Starts with number – should be a number, not identifier
        LexerTestCase{
            .name = "StartsWithNumber",
            .fileID = 1,
            .source = "123abc",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "123abc"},
                {TOK_EOF, {1, 7}, ""}
            }
        },

        // Unicode identifiers
        LexerTestCase{
            .name = "UnicodeIdentifier",
            .fileID = 1,
            .source = "π_value",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "π_value"},
                {TOK_EOF, {1, 8}, ""}
            }
        },
        LexerTestCase{
            .name = "UnicodeWithLetterDigits",
            .fileID = 1,
            .source = "变量123",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "变量123"},
                {TOK_EOF, {1, 6}, ""}
            }
        },

        // Invalid characters (e.g., operators or symbols in identifier)
        LexerTestCase{
            .name = "InvalidCharacterInIdentifier",
            .fileID = 1,
            .source = "foo$bar",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "foo"},
                {TOK_ERROR, {1, 4}, "$"},
                {TOK_IDENTIFIER, {1, 5}, "bar"},
                {TOK_EOF, {1, 8}, ""}
            }
        },

        // Just an underscore
        LexerTestCase{
            .name = "SingleUnderscore",
            .fileID = 1,
            .source = "_",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "_"},
                {TOK_EOF, {1, 2}, ""}
            }
        },

        // Identifiers separated by newlines
        LexerTestCase{
            .name = "IdentifiersAcrossLines",
            .fileID = 1,
            .source = "foo\nbar\nbaz",
            .expectedTokens = {
                {TOK_IDENTIFIER, {1, 1}, "foo"},
                {TOK_IDENTIFIER, {2, 1}, "bar"},
                {TOK_IDENTIFIER, {3, 1}, "baz"},
                {TOK_EOF, {3, 4}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
