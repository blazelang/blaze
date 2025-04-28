#include <gtest/gtest.h>

#include "Lexer/Token.hpp"
#include "SourceManager/ISourceManager.hpp"

#include "Lexer/LexerBaseTest.hpp"

class LexerSymbolTest : public LexerBaseTest {
protected:
    void SetUp() override {
        LexerBaseTest::SetUp();
    }
};

TEST_F(LexerSymbolTest, TestAllSymbols) {
    ISourceManager::FileID fileID = 0;
    AddSource(fileID, "+ ++ += - -- -= * *= / /= % %= , ~ () [] ^ <> <= >= & && &= | || |= /= >>= <<= ^= ! ? ; . {} =");

    std::vector<Token> expectedTokens = {
        {TOK_PLUS, {1, 1}, "+"}, {TOK_INCREMENT, {1, 3}, "++"}, {TOK_PLUS_ASSIGN, {1, 6}, "+="}, {TOK_MINUS, {1, 9}, "-"},
        {TOK_DECREMENT, {1, 11}, "--"}, {TOK_MINUS_ASSIGN, {1, 14}, "-="}, {TOK_MULTIPLY, {1, 17}, "*"}, {TOK_MULTIPLY_ASSIGN, {1, 19}, "*="},
        {TOK_DIVIDE, {1, 22}, "/"}, {TOK_DIVIDE_ASSIGN, {1, 24}, "/="}, {TOK_MODULO, {1, 27}, "%"}, {TOK_MODULO_ASSIGN, {1, 29}, "%="},
        {TOK_COMMA, {1, 32}, ","}, {TOK_BITWISE_NOT, {1, 34}, "~"}, {TOK_LPAREN, {1, 36}, "("}, {TOK_RPAREN, {1, 37}, ")"},
        {TOK_LBRACKET, {1, 39}, "["}, {TOK_RBRACKET, {1, 40}, "]"}, {TOK_XOR_ASSIGN, {1, 42}, "^"}, {TOK_LESS_THAN, {1, 44}, "<"},
        {TOK_GREATER_THAN, {1, 45}, ">"}, {TOK_LESS_EQUAL, {1, 47}, "<="}, {TOK_GREATER_EQUAL, {1, 50}, ">="}, {TOK_BITWISE_AND, {1, 53}, "&"},
        {TOK_LOGICAL_AND, {1, 55}, "&&"}, {TOK_AND_ASSIGN, {1, 58}, "&="}, {TOK_BITWISE_OR, {1, 61}, "|"}, {TOK_LOGICAL_OR, {1, 63}, "||"},
        {TOK_OR_ASSIGN, {1, 66}, "|="}, {TOK_DIVIDE_ASSIGN, {1, 69}, "/="}, {TOK_RIGHT_SHIFT_ASSIGN, {1, 72}, ">>="}, {TOK_LEFT_SHIFT_ASSIGN, {1, 76}, "<<="},
        {TOK_XOR_ASSIGN, {1, 80}, "^="}, {TOK_LOGICAL_NOT, {1, 83}, "!"}, {TOK_TERNARY_CONDITIONAL, {1, 85}, "?"}, {TOK_SEMICOLON, {1, 87}, ";"},
        {TOK_DOT, {1, 89}, "."}, {TOK_LBRACE, {1, 91}, "{"}, {TOK_RBRACE, {1, 92}, "}"}, {TOK_ASSIGN, {1, 94}, "="},
        {TOK_EOF, {1, 95}, ""},
    };

    std::vector<Token>& tokens = m_lexer->tokenize();

    CheckTokens(expectedTokens, tokens);
}

TEST_F(LexerSymbolTest, TestUnrecognizedSymbol) {
    ISourceManager::FileID fileID = 0;
    AddSource(fileID, "@+ #- $*");

    std::vector<Token> expectedTokens = {
        {TOK_ERROR, {1, 1}, "@"},
        {TOK_PLUS, {1, 2}, "+"},
        {TOK_ERROR, {1, 4}, "#"},
        {TOK_MINUS, {1, 5}, "-"},
        {TOK_ERROR, {1, 7}, "$"},
        {TOK_MULTIPLY, {1, 8}, "*"}
    };

    std::vector<Token>& tokens = m_lexer->tokenize();

    CheckTokens(expectedTokens, tokens);
}
