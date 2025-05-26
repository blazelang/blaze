#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

class LexerKeywordTest : public LexerBaseTest {};

TEST_P(LexerKeywordTest, TokenizeKeywords) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerKeywords,
    LexerKeywordTest,
    testing::Values(
        LexerTestCase{
            .name = "AllKeywords",
            .fileID = 1,
            .source =
                "let const fn return if elif else while break continue for "
                "true false enum null import export "
                "u8 u16 u32 u64 u128 i8 i16 i32 i64 i128 "
                "f16 f32 f64 char string bool void",
            .expectedTokens = {
                {TOK_LET,      {1, 1},   "let"},
                {TOK_CONST,    {1, 5},   "const"},
                {TOK_FN,       {1, 11},  "fn"},
                {TOK_RETURN,   {1, 14},  "return"},
                {TOK_IF,       {1, 21},  "if"},
                {TOK_ELIF,     {1, 24},  "elif"},
                {TOK_ELSE,     {1, 29},  "else"},
                {TOK_WHILE,    {1, 34},  "while"},
                {TOK_BREAK,    {1, 40},  "break"},
                {TOK_CONTINUE, {1, 46},  "continue"},
                {TOK_FOR,      {1, 55},  "for"},
                {TOK_TRUE,     {1, 59},  "true"},
                {TOK_FALSE,    {1, 64},  "false"},
                {TOK_ENUM,     {1, 70},  "enum"},
                {TOK_NULL,     {1, 75},  "null"},
                {TOK_IMPORT,   {1, 80},  "import"},
                {TOK_EXPORT,   {1, 87},  "export"},

                {TOK_U8,       {1, 94},  "u8"},
                {TOK_U16,      {1, 97},  "u16"},
                {TOK_U32,      {1, 101}, "u32"},
                {TOK_U64,      {1, 105}, "u64"},
                {TOK_U128,     {1, 109}, "u128"},
                {TOK_I8,       {1, 114}, "i8"},
                {TOK_I16,      {1, 117}, "i16"},
                {TOK_I32,      {1, 121}, "i32"},
                {TOK_I64,      {1, 125}, "i64"},
                {TOK_I128,     {1, 129}, "i128"},

                {TOK_F16,      {1, 134}, "f16"},
                {TOK_F32,      {1, 138}, "f32"},
                {TOK_F64,      {1, 142}, "f64"},
                {TOK_CHAR,     {1, 146}, "char"},
                {TOK_STRING,   {1, 151}, "string"},
                {TOK_BOOL,     {1, 158}, "bool"},
                {TOK_VOID,     {1, 163}, "void"},

                {TOK_EOF,      {1, 167}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
