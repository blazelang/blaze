#pragma once

#include <vector>
#include <optional>
#include <string_view>

#include "Diagnostics/Diagnostic.hpp"
#include "Diagnostics/DiagnosticID.hpp"
#include "Lexer/Token.hpp"
#include "SourceManager/SourceManager.hpp"
#include "Diagnostics/DiagnosticEngine.hpp"

class Lexer {
public:
    Lexer(ISourceManager::FileID fileID, ISourceManager& sourceManager, IDiagnosticEngine& diagnosticEngine);

    std::vector<Token>& tokenize();

private:
    enum NumericBase {
        Binary = 2,
        Octal = 8,
        Decimal = 10,
        HexaDecimal = 16
    };

    ISourceManager::FileID m_fileID;
    ISourceManager& m_sourceManager;
    IDiagnosticEngine& m_diagnosticEngine;

    std::string_view m_source;

    size_t m_start = 0;
    size_t m_pos = 0;
    size_t m_startLine = 1;
    size_t m_startColumn = 1;
    size_t m_endLine = 1;
    size_t m_endColumn = 1;

    std::vector<Token> m_tokens;

    char32_t advance();
    char32_t peek() const;
    char32_t lookahead(int i = 1) const;
    bool match(const char32_t cp);

    std::string_view getLexeme() const;

    void addToken(TokenKind kind, const std::optional<std::string>& lexeme = std::nullopt);

    std::unique_ptr<Diagnostic> buildDiagnostic(DiagnosticID id, std::string message, Span span);

    bool isEnd() const;
    bool isWhitespace(char32_t cp) const;
    bool isHexDigit(char32_t cp) const;
    bool isOctalDigit(char32_t cp) const;
    bool isBinaryDigit(char32_t cp) const;
    bool isDecimalDigit(char32_t cp) const;
    bool isNumberStart(char32_t cp) const;
    bool isDigit(Lexer::NumericBase base, char32_t cp) const;
    bool isAlpha(char32_t cp) const;
    bool isAlphaNum(char32_t cp) const;
    bool isIdentifierStart(char32_t cp);
    bool isIdentifierContinue(char32_t cp);

    void skipWhitespace(char32_t cp);
    void lexLineComment();
    void lexBlockComment();
    void lexKeywordOrIdentifier();
    void lexNumberLiteral(char32_t cp);
    bool lexEscapeSequence(bool isChar = true);
    void lexCharLiteral();
    void lexStringLiteral();
    void lexSymbol(char32_t cp);
};
