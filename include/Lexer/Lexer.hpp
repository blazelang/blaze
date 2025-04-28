#pragma once

#include <vector>
#include <cstdint>
#include <string_view>
#include <unordered_map>

#include "Lexer/Token.hpp"
#include "SourceManager/ISourceManager.hpp"
#include "Diagnostics/IDiagnosticEngine.hpp"

class Lexer {
    public:
        Lexer(
            ISourceManager::FileID fileID,
            ISourceManager& sourceManager,
            IDiagnosticEngine& diagnosticEngine
        );

        std::vector<Token>& tokenize();

    private:
        struct CodepointInfo {
            char32_t codepoint;
            uint8_t byteLength;
        };

        ISourceManager::FileID m_fileID;
        ISourceManager& m_sourceManager;
        IDiagnosticEngine& m_diagnosticEngine;
        std::string_view m_source;
        size_t m_start;
        size_t m_pos;
        size_t m_line;
        size_t m_column;
        std::vector<Token> m_tokens;

        static std::unordered_map<std::string, TokenKind> m_keywords;
        static std::unordered_map<std::string, TokenKind> m_symbols;

        CodepointInfo decodeUTF8(int pos) const;
        std::string codepointToString(char32_t codepoint) const;

        char32_t advance();
        char32_t peek() const;
        char32_t lookahead(int i = 1) const;
        bool match(char32_t codepoint);

        SourcePosition currentPosition() const;
        void reportError(const std::string& message);

        void addToken(TokenKind kind);

        bool isEnd() const;
        bool isWhitespace(char32_t codepoint) const;
        bool isHexDigit(char32_t codepoint) const;
        bool isOctalDigit(char32_t codepoint) const;
        bool isBinaryDigit(char32_t codepoint) const;
        bool isDecimalDigit(char32_t codepoint) const;
        bool isAlpha(char32_t codepoint) const;
        bool isAlphaNum(char32_t codepoint) const;

        void skipWhitespace(char32_t codepoint);

        void lexSymbol(char32_t codepoint);
};
