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
        LexerTestCase {
            "SingleCharSymbols",
            1,
            "",
            {
                {TOK_EOF, {1, 1}, ""}
            }
        },
        LexerTestCase {
            "MultiCharSymbols",
            2,
            "",
            {
                {TOK_EOF, {1, 1}, ""}
            }
        }
    ),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
