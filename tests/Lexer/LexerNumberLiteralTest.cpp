#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

TEST_P(LexerBaseTest, TokenizeNumberLiterals) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerNumberLiteralTest,
    LexerBaseTest,
    testing::Values(
    // Basic integer literals
        LexerTestCase{
            .name = "BasicIntegerLiterals",
            .fileID = 1,
            .source = "0 1 123 9876543210",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0"},
                {TOK_INTEGER_LITERAL, {1, 3}, "1"},
                {TOK_INTEGER_LITERAL, {1, 5}, "123"},
                {TOK_INTEGER_LITERAL, {1, 9}, "9876543210"},
                {TOK_EOF, {1, 19}, ""}
            }
        },

        // Basic float literals
        LexerTestCase{
            .name = "BasicFloatLiterals",
            .fileID = 1,
            .source = "3.14 0.5 1.0 123.456",
            .expectedTokens = {
                {TOK_FLOAT_LITERAL, {1, 1}, "3.14"},
                {TOK_FLOAT_LITERAL, {1, 6}, "0.5"},
                {TOK_FLOAT_LITERAL, {1, 10}, "1.0"},
                {TOK_FLOAT_LITERAL, {1, 14}, "123.456"},
                {TOK_EOF, {1, 21}, ""}
            }
        },

        // Scientific notation
        LexerTestCase{
            .name = "ScientificNotation",
            .fileID = 1,
            .source = "1e10 1E10 1.5e10 1.5E10 1e+10 1e-10 1.5e+10 1.5e-10",
            .expectedTokens = {
                {TOK_FLOAT_LITERAL, {1, 1}, "1e10"},
                {TOK_FLOAT_LITERAL, {1, 6}, "1E10"},
                {TOK_FLOAT_LITERAL, {1, 11}, "1.5e10"},
                {TOK_FLOAT_LITERAL, {1, 18}, "1.5E10"},
                {TOK_FLOAT_LITERAL, {1, 25}, "1e+10"},
                {TOK_FLOAT_LITERAL, {1, 31}, "1e-10"},
                {TOK_FLOAT_LITERAL, {1, 37}, "1.5e+10"},
                {TOK_FLOAT_LITERAL, {1, 45}, "1.5e-10"},
                {TOK_EOF, {1, 52}, ""}
            }
        },

        // Hexadecimal literals
        LexerTestCase{
            .name = "HexadecimalLiterals",
            .fileID = 1,
            .source = "0x0 0X0 0xF 0XF 0xabc 0XDEF 0xaBcDeF 0x123456789ABCDEF",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0x0"},
                {TOK_INTEGER_LITERAL, {1, 5}, "0X0"},
                {TOK_INTEGER_LITERAL, {1, 9}, "0xF"},
                {TOK_INTEGER_LITERAL, {1, 13}, "0XF"},
                {TOK_INTEGER_LITERAL, {1, 17}, "0xabc"},
                {TOK_INTEGER_LITERAL, {1, 23}, "0XDEF"},
                {TOK_INTEGER_LITERAL, {1, 29}, "0xaBcDeF"},
                {TOK_INTEGER_LITERAL, {1, 38}, "0x123456789ABCDEF"},
                {TOK_EOF, {1, 55}, ""}
            }
        },

        // Binary literals
        LexerTestCase{
            .name = "BinaryLiterals",
            .fileID = 1,
            .source = "0b0 0B0 0b1 0B1 0b101010 0B111000",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0b0"},
                {TOK_INTEGER_LITERAL, {1, 5}, "0B0"},
                {TOK_INTEGER_LITERAL, {1, 9}, "0b1"},
                {TOK_INTEGER_LITERAL, {1, 13}, "0B1"},
                {TOK_INTEGER_LITERAL, {1, 17}, "0b101010"},
                {TOK_INTEGER_LITERAL, {1, 26}, "0B111000"},
                {TOK_EOF, {1, 34}, ""}
            }
        },

        // Octal literals
        LexerTestCase{
            .name = "OctalLiterals",
            .fileID = 1,
            .source = "0o0 0O0 0o7 0O7 0o123456 0O765432",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0o0"},
                {TOK_INTEGER_LITERAL, {1, 5}, "0O0"},
                {TOK_INTEGER_LITERAL, {1, 9}, "0o7"},
                {TOK_INTEGER_LITERAL, {1, 13}, "0O7"},
                {TOK_INTEGER_LITERAL, {1, 17}, "0o123456"},
                {TOK_INTEGER_LITERAL, {1, 26}, "0O765432"},
                {TOK_EOF, {1, 34}, ""}
            }
        },

        // Integer literals with digit separators
        LexerTestCase{
            .name = "IntegerWithDigitSeparators",
            .fileID = 1,
            .source = "1_000 1_000_000 1_2_3_4_5",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "1_000"},
                {TOK_INTEGER_LITERAL, {1, 7}, "1_000_000"},
                {TOK_INTEGER_LITERAL, {1, 17}, "1_2_3_4_5"},
                {TOK_EOF, {1, 26}, ""}
            }
        },

        // Float literals with digit separators
        LexerTestCase{
            .name = "FloatWithDigitSeparators",
            .fileID = 1,
            .source = "3.1_4 0.5_5 1_000.0_001",
            .expectedTokens = {
                {TOK_FLOAT_LITERAL, {1, 1}, "3.1_4"},
                {TOK_FLOAT_LITERAL, {1, 7}, "0.5_5"},
                {TOK_FLOAT_LITERAL, {1, 13}, "1_000.0_001"},
                {TOK_EOF, {1, 24}, ""}
            }
        },

        // Hexadecimal literals with digit separators
        LexerTestCase{
            .name = "HexWithDigitSeparators",
            .fileID = 1,
            .source = "0x1_2_3 0xaa_bb_cc 0xABCD_EF01",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0x1_2_3"},
                {TOK_INTEGER_LITERAL, {1, 9}, "0xaa_bb_cc"},
                {TOK_INTEGER_LITERAL, {1, 20}, "0xABCD_EF01"},
                {TOK_EOF, {1, 31}, ""}
            }
        },

        // Binary literals with digit separators
        LexerTestCase{
            .name = "BinaryWithDigitSeparators",
            .fileID = 1,
            .source = "0b1_0_1 0b1010_1010",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0b1_0_1"},
                {TOK_INTEGER_LITERAL, {1, 9}, "0b1010_1010"},
                {TOK_EOF, {1, 20}, ""}
            }
        },

        // Octal literals with digit separators
        LexerTestCase{
            .name = "OctalWithDigitSeparators",
            .fileID = 1,
            .source = "0o1_2_3 0o123_456_7",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0o1_2_3"},
                {TOK_INTEGER_LITERAL, {1, 9}, "0o123_456_7"},
                {TOK_EOF, {1, 20}, ""}
            }
        },

        // Scientific notation with digit separators (only in mantissa)
        LexerTestCase{
            .name = "ScientificWithDigitSeparators",
            .fileID = 1,
            .source = "1_000e10 1.5_5e10 1_0e+10 1.5_5e-10",
            .expectedTokens = {
                {TOK_FLOAT_LITERAL, {1, 1}, "1_000e10"},
                {TOK_FLOAT_LITERAL, {1, 10}, "1.5_5e10"},
                {TOK_FLOAT_LITERAL, {1, 19}, "1_0e+10"},
                {TOK_FLOAT_LITERAL, {1, 27}, "1.5_5e-10"},
                {TOK_EOF, {1, 36}, ""}
            }
        },

        // Mixed number literals
        LexerTestCase{
            .name = "MixedNumberLiterals",
            .fileID = 1,
            .source = "42 3.14159 0xDEADBEEF 0b10101010 0o7654321 1.2e-10",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "42"},
                {TOK_FLOAT_LITERAL, {1, 4}, "3.14159"},
                {TOK_INTEGER_LITERAL, {1, 12}, "0xDEADBEEF"},
                {TOK_INTEGER_LITERAL, {1, 23}, "0b10101010"},
                {TOK_INTEGER_LITERAL, {1, 34}, "0o7654321"},
                {TOK_FLOAT_LITERAL, {1, 44}, "1.2e-10"},
                {TOK_EOF, {1, 51}, ""}
            }
        },

        // Edge cases - large numbers
        LexerTestCase{
            .name = "LargeNumbers",
            .fileID = 1,
            .source = "9223372036854775807 18446744073709551615 1e308",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "9223372036854775807"},
                {TOK_INTEGER_LITERAL, {1, 21}, "18446744073709551615"},
                {TOK_FLOAT_LITERAL, {1, 42}, "1e308"},
                {TOK_EOF, {1, 47}, ""}
            }
        },

        // Edge case - zeros
        LexerTestCase{
            .name = "ZeroFormats",
            .fileID = 1,
            .source = "0 0.0 0.0e0 0x0 0b0 0o0",
            .expectedTokens = {
                {TOK_INTEGER_LITERAL, {1, 1}, "0"},
                {TOK_FLOAT_LITERAL, {1, 3}, "0.0"},
                {TOK_FLOAT_LITERAL, {1, 7}, "0.0e0"},
                {TOK_INTEGER_LITERAL, {1, 13}, "0x0"},
                {TOK_INTEGER_LITERAL, {1, 17}, "0b0"},
                {TOK_INTEGER_LITERAL, {1, 21}, "0o0"},
                {TOK_EOF, {1, 24}, ""}
            }
        },

        // Invalid leading underscore
        LexerTestCase{
            .name = "InvalidLeadingUnderscore",
            .fileID = 1,
            .source = "_123 _0.5 _0xF _0b1 _0o7",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "_123"},
                {TOK_ERROR, {1, 6}, "_0.5"},
                {TOK_ERROR, {1, 11}, "_0xF"},
                {TOK_ERROR, {1, 16}, "_0b1"},
                {TOK_ERROR, {1, 21}, "_0o7"},
                {TOK_EOF, {1, 25}, ""}
            }
        },

        // Invalid trailing underscore
        LexerTestCase{
            .name = "InvalidTrailingUnderscore",
            .fileID = 1,
            .source = "123_ 0.5_ 0xF_ 0b1_ 0o7_",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "123_"},
                {TOK_ERROR, {1, 6}, "0.5_"},
                {TOK_ERROR, {1, 11}, "0xF_"},
                {TOK_ERROR, {1, 16}, "0b1_"},
                {TOK_ERROR, {1, 21}, "0o7_"},
                {TOK_EOF, {1, 25}, ""}
            }
        },

        // Invalid consecutive underscores
        LexerTestCase{
            .name = "InvalidConsecutiveUnderscores",
            .fileID = 1,
            .source = "1__23 0.5__5 0x1__F 0b1__0 0o1__7",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "1__23"},
                {TOK_ERROR, {1, 7}, "0.5__5"},
                {TOK_ERROR, {1, 14}, "0x1__F"},
                {TOK_ERROR, {1, 21}, "0b1__0"},
                {TOK_ERROR, {1, 28}, "0o1__7"},
                {TOK_EOF, {1, 34}, ""}
            }
        },

        // Invalid float format
        LexerTestCase{
            .name = "InvalidFloatFormat",
            .fileID = 1,
            .source = ".5 1.2.3 1e 1e+ 1e-",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, ".5"},
                {TOK_ERROR, {1, 4}, "1.2.3"},
                {TOK_ERROR, {1, 10}, "1e"},
                {TOK_ERROR, {1, 13}, "1e+"},
                {TOK_ERROR, {1, 17}, "1e-"},
                {TOK_EOF, {1, 20}, ""}
            }
        },

        // Invalid hex, binary, octal digits
        LexerTestCase{
            .name = "InvalidDigitsInSpecialBases",
            .fileID = 1,
            .source = "0xGHIJ 0bA123 0b23 0o89A",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "0xGHIJ"},
                {TOK_ERROR, {1, 8}, "0bA123"},
                {TOK_ERROR, {1, 15}, "0b23"},
                {TOK_ERROR, {1, 20}, "0o89A"},
                {TOK_EOF, {1, 25}, ""}
            }
        },

        // Invalid underscore placement
        LexerTestCase{
            .name = "InvalidUnderscorePlacement",
            .fileID = 1,
            .source = "0_.1 1e_10 0x_F 0b_1 0o_7",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "0_.1"},
                {TOK_ERROR, {1, 6}, "1e_10"},
                {TOK_ERROR, {1, 12}, "0x_F"},
                {TOK_ERROR, {1, 17}, "0b_1"},
                {TOK_ERROR, {1, 22}, "0o_7"},
                {TOK_EOF, {1, 26}, ""}
            }
        },

        // Invalid exponent with digit separators
        LexerTestCase{
            .name = "InvalidExponentWithDigitSeparators",
            .fileID = 1,
            .source = "1e1_0 1.5e1_0 1e+1_0 1.5e-1_0",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "1e1_0"},
                {TOK_ERROR, {1, 7}, "1.5e1_0"},
                {TOK_ERROR, {1, 15}, "1e+1_0"},
                {TOK_ERROR, {1, 22}, "1.5e-1_0"},
                {TOK_EOF, {1, 30}, ""}
            }
        },

        // Invalid combinations
        LexerTestCase{
            .name = "InvalidCombinations",
            .fileID = 1,
            .source = "0xx123 0bb101 0oo123 0bx123 0ox123",
            .expectedTokens = {
                {TOK_ERROR, {1, 1}, "0xx123"},
                {TOK_ERROR, {1, 8}, "0bb101"},
                {TOK_ERROR, {1, 15}, "0oo123"},
                {TOK_ERROR, {1, 22}, "0bx123"},
                {TOK_ERROR, {1, 29}, "0ox123"},
                {TOK_EOF, {1, 35}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
