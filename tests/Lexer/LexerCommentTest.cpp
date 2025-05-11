#include <gtest/gtest.h>

#include "Lexer/Token.hpp"

#include "Lexer/LexerBaseTest.cc"

TEST_P(LexerBaseTest, TokenizeComments) {
    const LexerTestCase& testcase = GetParam();
    std::vector<Token>& tokens = m_lexer->tokenize();
    CheckTokens(testcase.expectedTokens, tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerCommentTest,
    LexerBaseTest,
    testing::Values(),
    [](const testing::TestParamInfo<LexerTestCase>& info) {
        return info.param.name;
    }
);
