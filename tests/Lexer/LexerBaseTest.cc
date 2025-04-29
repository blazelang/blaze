#pragma once

#include <algorithm>
#include <memory>
#include <format>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Lexer/Lexer.hpp"
#include "Lexer/Token.hpp"
#include "SourceManager/ISourceManager.hpp"

#include "SourceManager/MockSourceManager.hpp"
#include "Diagnostics/MockDiagnosticEngine.hpp"

struct LexerTestCase {
    std::string name;
    ISourceManager::FileID fileID;
    std::string source;
    std::vector<Token> expectedTokens;
};

class LexerBaseTest : public testing::Test, public testing::WithParamInterface<LexerTestCase> {
protected:
    MockSourceManager m_sourceManager;
    MockDiagnosticEngine m_diagnosticEngine;
    std::unique_ptr<Lexer> m_lexer;

    void SetUp() override {
        const LexerTestCase& testcase = GetParam();
        EXPECT_CALL(m_sourceManager, getBuffer(testcase.fileID)).WillOnce(testing::Return(testcase.source));
        EXPECT_CALL(m_diagnosticEngine, report(testing::_)).Times(testing::AtLeast(countTokenErrors(testcase.expectedTokens)));
        m_lexer = std::make_unique<Lexer>(Lexer(testcase.fileID, m_sourceManager, m_diagnosticEngine));
    }

    size_t countTokenErrors(const std::vector<Token>& tokens) {
        return std::count_if(tokens.begin(), tokens.end(), [](const Token& t) {
            return t.kind == TOK_ERROR;
        });
    }

    void CheckTokens(const std::vector<Token>& expectedTokens, const std::vector<Token>& recievedTokens) {
        // ASSERT_EQ(expectedTokens.size(), recievedTokens.size());
        for (int i = 0; i < recievedTokens.size(); ++i) {
            SCOPED_TRACE(testing::Message() << std::format("Expected: {}, Received: {}", TokenKindToString(expectedTokens[i]), TokenKindToString(recievedTokens[i])));
            EXPECT_EQ(recievedTokens[i].kind, expectedTokens[i].kind);
            EXPECT_EQ(recievedTokens[i].lexeme, expectedTokens[i].lexeme);
            EXPECT_EQ(recievedTokens[i].position, expectedTokens[i].position);
        }
    }
};
