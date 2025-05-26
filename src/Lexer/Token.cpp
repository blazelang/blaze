#include "Lexer/Token.hpp"

#include <format>

std::unordered_map<std::string, TokenKind> g_keywordMap = {
    {"let", TOK_LET},
    {"const", TOK_CONST},
    {"fn", TOK_FN},
    {"return", TOK_RETURN},
    {"if", TOK_IF},
    {"elif", TOK_ELIF},
    {"else", TOK_ELSE},
    {"while", TOK_WHILE},
    {"break", TOK_BREAK},
    {"continue", TOK_CONTINUE},
    {"for", TOK_FOR},
    {"true", TOK_TRUE},
    {"false", TOK_FALSE},
    {"enum", TOK_ENUM},
    {"null", TOK_NULL},
    {"import", TOK_IMPORT},
    {"export", TOK_EXPORT},
    {"u8", TOK_U8},
    {"u16", TOK_U16},
    {"u32", TOK_U32},
    {"u64", TOK_U64},
    {"u128", TOK_U128},
    {"i8", TOK_I8},
    {"i16", TOK_I16},
    {"i32", TOK_I32},
    {"i64", TOK_I64},
    {"i128", TOK_I128},
    {"f16", TOK_F16},
    {"f32", TOK_F32},
    {"f64", TOK_F64},
    {"char", TOK_CHAR},
    {"string", TOK_STRING},
    {"bool", TOK_BOOL},
    {"void", TOK_VOID},
};

std::unordered_map<std::string, TokenKind> g_symbolMap = {
    {",", TOK_COMMA},
    {":", TOK_COLON},
    {";", TOK_SEMICOLON},
    {".", TOK_DOT},
    {"~", TOK_BITWISE_NOT},
    {"(", TOK_LPAREN},
    {")", TOK_RPAREN},
    {"{", TOK_LBRACE},
    {"}", TOK_RBRACE},
    {"[", TOK_LBRACKET},
    {"]", TOK_RBRACKET},
    {"?", TOK_TERNARY_CONDITIONAL},
    {"=", TOK_ASSIGN},
    {"+", TOK_PLUS},
    {"-", TOK_MINUS},
    {"*", TOK_MULTIPLY},
    {"/", TOK_DIVIDE},
    {"%", TOK_MODULO},
    {"&", TOK_BITWISE_AND},
    {"|", TOK_BITWISE_OR},
    {"^", TOK_XOR_ASSIGN},
    {"<", TOK_LESS_THAN},
    {">", TOK_GREATER_THAN},
    {"!", TOK_LOGICAL_NOT},
    {"!=", TOK_NOT_EQUAL},
    {"==", TOK_EQUAL},
    {"->", TOK_ARROW},
    {"+=", TOK_PLUS_ASSIGN},
    {"++", TOK_INCREMENT},
    {"-=", TOK_MINUS_ASSIGN},
    {"--", TOK_DECREMENT},
    {"*=", TOK_MULTIPLY_ASSIGN},
    {"/=", TOK_DIVIDE_ASSIGN},
    {"%=", TOK_MODULO_ASSIGN},
    {"&=", TOK_AND_ASSIGN},
    {"&&", TOK_LOGICAL_AND},
    {"|=", TOK_OR_ASSIGN},
    {"||", TOK_LOGICAL_OR},
    {"^=", TOK_XOR_ASSIGN},
    {"<=", TOK_LESS_EQUAL},
    {"<<=", TOK_LEFT_SHIFT_ASSIGN},
    {"<<", TOK_LEFT_SHIFT},
    {">=", TOK_GREATER_EQUAL},
    {">>=", TOK_RIGHT_SHIFT_ASSIGN},
    {">>", TOK_RIGHT_SHIFT}
};

std::string TokenKindToString(Token token) {
    switch (token.kind) {
        case TOK_LET: return std::format("TOK_LET(\"{}\")", token.lexeme);
        case TOK_CONST: return std::format("TOK_CONST(\"{}\")", token.lexeme);
        case TOK_FN: return std::format("TOK_FN(\"{}\")", token.lexeme);
        case TOK_RETURN: return std::format("TOK_RETURN(\"{}\")", token.lexeme);
        case TOK_IF: return std::format("TOK_IF(\"{}\")", token.lexeme);
        case TOK_ELIF: return std::format("TOK_ELSE_IF(\"{}\")", token.lexeme);
        case TOK_ELSE: return std::format("TOK_ELSE(\"{}\")", token.lexeme);
        case TOK_WHILE: return std::format("TOK_WHILE(\"{}\")", token.lexeme);
        case TOK_BREAK: return std::format("TOK_BREAK(\"{}\")", token.lexeme);
        case TOK_CONTINUE: return std::format("TOK_CONTINUE(\"{}\")", token.lexeme);
        case TOK_FOR: return std::format("TOK_FOR(\"{}\")", token.lexeme);
        case TOK_TRUE: return std::format("TOK_TRUE(\"{}\")", token.lexeme);
        case TOK_FALSE: return std::format("TOK_FALSE(\"{}\")", token.lexeme);
        case TOK_ENUM: return std::format("TOK_ENUM(\"{}\")", token.lexeme);
        case TOK_NULL: return std::format("TOK_NULL(\"{}\")", token.lexeme);
        case TOK_IMPORT: return std::format("TOK_IMPORT(\"{}\")", token.lexeme);
        case TOK_EXPORT: return std::format("TOK_EXPORT(\"{}\")", token.lexeme);

        case TOK_U8: return std::format("TOK_U8(\"{}\")", token.lexeme);
        case TOK_U16: return std::format("TOK_U16(\"{}\")", token.lexeme);
        case TOK_U32: return std::format("TOK_U32(\"{}\")", token.lexeme);
        case TOK_U64: return std::format("TOK_U64(\"{}\")", token.lexeme);
        case TOK_U128: return std::format("TOK_U128(\"{}\")", token.lexeme);
        case TOK_I8: return std::format("TOK_I8(\"{}\")", token.lexeme);
        case TOK_I16: return std::format("TOK_I16(\"{}\")", token.lexeme);
        case TOK_I32: return std::format("TOK_I32(\"{}\")", token.lexeme);
        case TOK_I64: return std::format("TOK_I64(\"{}\")", token.lexeme);
        case TOK_I128: return std::format("TOK_I128(\"{}\")", token.lexeme);
        case TOK_F16: return std::format("TOK_F16(\"{}\")", token.lexeme);
        case TOK_F32: return std::format("TOK_F32(\"{}\")", token.lexeme);
        case TOK_F64: return std::format("TOK_F64(\"{}\")", token.lexeme);
        case TOK_CHAR: return std::format("TOK_CHAR(\"{}\")", token.lexeme);
        case TOK_STRING: return std::format("TOK_STRING(\"{}\")", token.lexeme);
        case TOK_BOOL: return std::format("TOK_BOOL(\"{}\")", token.lexeme);
        case TOK_VOID: return std::format("TOK_VOID(\"{}\")", token.lexeme);

        case TOK_ASSIGN: return std::format("TOK_ASSIGN(\"{}\")", token.lexeme);
        case TOK_PLUS: return std::format("TOK_PLUS(\"{}\")", token.lexeme);
        case TOK_MINUS: return std::format("TOK_MINUS(\"{}\")", token.lexeme);
        case TOK_MULTIPLY: return std::format("TOK_MULTIPLY(\"{}\")", token.lexeme);
        case TOK_DIVIDE: return std::format("TOK_DIVIDE(\"{}\")", token.lexeme);
        case TOK_MODULO: return std::format("TOK_MODULO(\"{}\")", token.lexeme);
        case TOK_INCREMENT: return std::format("TOK_INCREMENT(\"{}\")", token.lexeme);
        case TOK_DECREMENT: return std::format("TOK_DECREMENT(\"{}\")", token.lexeme);
        case TOK_BITWISE_AND: return std::format("TOK_BITWISE_AND(\"{}\")", token.lexeme);
        case TOK_BITWISE_OR: return std::format("TOK_BITWISE_OR(\"{}\")", token.lexeme);
        case TOK_BITWISE_XOR: return std::format("TOK_BITWISE_XOR(\"{}\")", token.lexeme);
        case TOK_BITWISE_NOT: return std::format("TOK_BITWISE_NOT(\"{}\")", token.lexeme);
        case TOK_LEFT_SHIFT: return std::format("TOK_LEFT_SHIFT(\"{}\")", token.lexeme);
        case TOK_RIGHT_SHIFT: return std::format("TOK_RIGHT_SHIFT(\"{}\")", token.lexeme);
        case TOK_EQUAL: return std::format("TOK_EQUAL(\"{}\")", token.lexeme);
        case TOK_NOT_EQUAL: return std::format("TOK_NOT_EQUAL(\"{}\")", token.lexeme);
        case TOK_LESS_THAN: return std::format("TOK_LESS_THAN(\"{}\")", token.lexeme);
        case TOK_GREATER_THAN: return std::format("TOK_GREATER_THAN(\"{}\")", token.lexeme);
        case TOK_LESS_EQUAL: return std::format("TOK_LESS_EQUAL(\"{}\")", token.lexeme);
        case TOK_GREATER_EQUAL: return std::format("TOK_GREATER_EQUAL(\"{}\")", token.lexeme);
        case TOK_LOGICAL_AND: return std::format("TOK_LOGICAL_AND(\"{}\")", token.lexeme);
        case TOK_LOGICAL_OR: return std::format("TOK_LOGICAL_OR(\"{}\")", token.lexeme);
        case TOK_LOGICAL_NOT: return std::format("TOK_LOGICAL_NOT(\"{}\")", token.lexeme);
        case TOK_TERNARY_CONDITIONAL: return std::format("TOK_TERNARY_CONDITIONAL(\"{}\")", token.lexeme);

        case TOK_PLUS_ASSIGN: return std::format("TOK_PLUS_ASSIGN(\"{}\")", token.lexeme);
        case TOK_MINUS_ASSIGN: return std::format("TOK_MINUS_ASSIGN(\"{}\")", token.lexeme);
        case TOK_MULTIPLY_ASSIGN: return std::format("TOK_MULTIPLY_ASSIGN(\"{}\")", token.lexeme);
        case TOK_DIVIDE_ASSIGN: return std::format("TOK_DIVIDE_ASSIGN(\"{}\")", token.lexeme);
        case TOK_MODULO_ASSIGN: return std::format("TOK_MODULO_ASSIGN(\"{}\")", token.lexeme);
        case TOK_AND_ASSIGN: return std::format("TOK_AND_ASSIGN(\"{}\")", token.lexeme);
        case TOK_OR_ASSIGN: return std::format("TOK_OR_ASSIGN(\"{}\")", token.lexeme);
        case TOK_XOR_ASSIGN: return std::format("TOK_XOR_ASSIGN(\"{}\")", token.lexeme);
        case TOK_LEFT_SHIFT_ASSIGN: return std::format("TOK_LEFT_SHIFT_ASSIGN(\"{}\")", token.lexeme);
        case TOK_RIGHT_SHIFT_ASSIGN: return std::format("TOK_RIGHT_SHIFT_ASSIGN(\"{}\")", token.lexeme);

        case TOK_DOT: return std::format("TOK_DOT(\"{}\")", token.lexeme);
        case TOK_ARROW: return std::format("TOK_ARROW(\"{}\")", token.lexeme);

        case TOK_COMMA: return std::format("TOK_COMMA(\"{}\")", token.lexeme);
        case TOK_COLON: return std::format("TOK_COLON(\"{}\")", token.lexeme);
        case TOK_SEMICOLON: return std::format("TOK_SEMICOLON(\"{}\")", token.lexeme);

        case TOK_LPAREN: return std::format("TOK_LPAREN(\"{}\")", token.lexeme);
        case TOK_RPAREN: return std::format("TOK_RPAREN(\"{}\")", token.lexeme);
        case TOK_LBRACE: return std::format("TOK_LBRACE(\"{}\")", token.lexeme);
        case TOK_RBRACE: return std::format("TOK_RBRACE(\"{}\")", token.lexeme);
        case TOK_LBRACKET: return std::format("TOK_LBRACKET(\"{}\")", token.lexeme);
        case TOK_RBRACKET: return std::format("TOK_RBRACKET(\"{}\")", token.lexeme);

        case TOK_INTEGER_LITERAL: return std::format("TOK_INTEGER_LITERAL(\"{}\")", token.lexeme);
        case TOK_FLOAT_LITERAL: return std::format("TOK_FLOAT_LITERAL(\"{}\")", token.lexeme);
        case TOK_CHAR_LITERAL: return std::format("TOK_CHAR_LITERAL(\"{}\")", token.lexeme);
        case TOK_STRING_LITERAL: return std::format("TOK_STRING_LITERAL(\"{}\")", token.lexeme);

        case TOK_IDENTIFIER: return std::format("TOK_IDENTIFIER(\"{}\")", token.lexeme);
        case TOK_DOC_COMMENT_LINE_OUTER: return std::format("TOK_DOC_COMMENT_LINE_OUTER(\"{}\")", token.lexeme);
        case TOK_DOC_COMMENT_LINE_INNER: return std::format("TOK_DOC_COMMENT_LINE_INNER(\"{}\")", token.lexeme);
        case TOK_DOC_COMMENT_BLOCK_OUTER: return std::format("TOK_DOC_COMMENT_BLOCK_OUTER(\"{}\")", token.lexeme);
        case TOK_DOC_COMMENT_BLOCK_INNER: return std::format("TOK_DOC_COMMENT_BLOCK_INNER(\"{}\")", token.lexeme);
        case TOK_ERROR: return std::format("TOK_ERROR(\"{}\")", token.lexeme);
        case TOK_EOF: return "TOK_EOF";

        default: return std::format("TOK_UNKNOWN(\"{}\")", token.lexeme);
    }
}
