#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

class LexerCommentTest : public LexerBaseTest {};

TEST_P(LexerCommentTest, TokenizeComments) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerComments,
    LexerCommentTest,
    testing::Values(
        // Basic Line Comment
        LexerTestCase{
            .name = "LineComment",
            .fileID = 1,
            .source = "// this is a line comment\n",
            .expectedTokens = {
                {TOK_EOF, {2, 1}, ""}
            }
        },

        // Basic Block Comment
        LexerTestCase{
            .name = "BlockComment",
            .fileID = 1,
            .source = "/* this is a block comment */",
            .expectedTokens = {
                {TOK_EOF, {1, 30}, ""}
            }
        },

        // Nested block comment (if supported)
        LexerTestCase{
            .name = "NestedBlockComment",
            .fileID = 1,
            .source = "/* outer /* inner */ outer again */",
            .expectedTokens = {
                {TOK_EOF, {1, 36}, ""}
            }
        },

        // Doc Comment Line Outer (///)
        LexerTestCase{
            .name = "DocCommentLineOuter",
            .fileID = 1,
            .source = "/// this is a doc line outer comment\n",
            .expectedTokens = {
                {TOK_DOC_COMMENT_LINE_OUTER, {1, 1}, "/// this is a doc line outer comment"},
                {TOK_EOF, {2, 1}, ""}
            }
        },

        // Doc Comment Line Inner (!)
        LexerTestCase{
            .name = "DocCommentLineInner",
            .fileID = 1,
            .source = "//! inner doc line comment\n",
            .expectedTokens = {
                {TOK_DOC_COMMENT_LINE_INNER, {1, 1}, "//! inner doc line comment"},
                {TOK_EOF, {2, 1}, ""}
            }
        },

        // Doc Comment Block Outer (/** */)
        LexerTestCase{
            .name = "DocCommentBlockOuter",
            .fileID = 1,
            .source = "/** block outer doc comment */",
            .expectedTokens = {
                {TOK_DOC_COMMENT_BLOCK_OUTER, {1, 1}, "/** block outer doc comment */"},
                {TOK_EOF, {1, 31}, ""}
            }
        },

        // Doc Comment Block Inner (/*! */)
        LexerTestCase{
            .name = "DocCommentBlockInner",
            .fileID = 1,
            .source = "/*! block inner doc comment */",
            .expectedTokens = {
                {TOK_DOC_COMMENT_BLOCK_INNER, {1, 1}, "/*! block inner doc comment */"},
                {TOK_EOF, {1, 31}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
