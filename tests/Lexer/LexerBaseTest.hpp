#pragma once

#include <memory>
#include <format>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Lexer/Lexer.hpp"

#include "SourceManager/MockSourceManager.hpp"
#include "Diagnostics/MockDiagnosticEngine.hpp"

class LexerBaseTest : public ::testing::Test {
protected:
    MockSourceManager m_sourceManager;
    MockDiagnosticEngine m_diagnosticEngine;
    std::unique_ptr<Lexer> m_lexer;

    void SetUp() override {}

    void AddSource(ISourceManager::FileID fileID, std::string_view source) {
        EXPECT_CALL(m_sourceManager, getBuffer(fileID)).WillOnce(testing::Return(source));
        m_lexer = std::make_unique<Lexer>(Lexer(fileID, m_sourceManager, m_diagnosticEngine));
    }

    void CheckTokens(std::vector<Token>& expectedTokens, std::vector<Token>& recievedTokens) {
        ASSERT_EQ(expectedTokens.size(), recievedTokens.size());

        for (int i = 0; i < recievedTokens.size(); ++i) {
            SCOPED_TRACE(testing::Message() << std::format("Expected: {}, Received: {}", TokenKindToString(expectedTokens[i]), TokenKindToString(recievedTokens[i])));
            EXPECT_EQ(recievedTokens[i].kind, expectedTokens[i].kind);
            EXPECT_EQ(recievedTokens[i].lexeme, expectedTokens[i].lexeme);
            EXPECT_EQ(recievedTokens[i].position, expectedTokens[i].position);
        }
    }
};
