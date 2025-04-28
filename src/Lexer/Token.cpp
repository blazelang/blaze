#include "Lexer/Token.hpp"

#include <format>

std::string TokenKindToString(Token token) {
    switch (token.kind) {
        case TOK_LET: return "TOK_LET";
        case TOK_CONST: return "TOK_CONST";
        case TOK_FUNCTION: return "TOK_FUNCTION";
        case TOK_RETURN: return "TOK_RETURN";
        case TOK_IF: return "TOK_IF";
        case TOK_ELSE_IF: return "TOK_ELSE_IF";
        case TOK_ELSE: return "TOK_ELSE";
        case TOK_WHILE: return "TOK_WHILE";
        case TOK_BREAK: return "TOK_BREAK";
        case TOK_CONTINUE: return "TOK_CONTINUE";
        case TOK_FOR: return "TOK_FOR";
        case TOK_TRUE: return "TOK_TRUE";
        case TOK_FALSE: return "TOK_FALSE";
        case TOK_ENUM: return "TOK_ENUM";
        case TOK_NULL: return "TOK_NULL";
        case TOK_IMPORT: return "TOK_IMPORT";
        case TOK_EXPORT: return "TOK_EXPORT";

        case TOK_U8: return "TOK_U8";
        case TOK_U16: return "TOK_U16";
        case TOK_U32: return "TOK_U32";
        case TOK_U64: return "TOK_U64";
        case TOK_U128: return "TOK_U128";
        case TOK_I8: return "TOK_I8";
        case TOK_I16: return "TOK_I16";
        case TOK_I32: return "TOK_I32";
        case TOK_I64: return "TOK_I64";
        case TOK_I128: return "TOK_I128";
        case TOK_F16: return "TOK_F16";
        case TOK_F32: return "TOK_F32";
        case TOK_F64: return "TOK_F64";
        case TOK_CHAR: return "TOK_CHAR";
        case TOK_STRING: return "TOK_STRING";
        case TOK_BOOL: return "TOK_BOOL";
        case TOK_VOID: return "TOK_VOID";

        case TOK_ASSIGN: return "TOK_ASSIGN";
        case TOK_PLUS: return "TOK_PLUS";
        case TOK_MINUS: return "TOK_MINUS";
        case TOK_MULTIPLY: return "TOK_MULTIPLY";
        case TOK_DIVIDE: return "TOK_DIVIDE";
        case TOK_MODULO: return "TOK_MODULO";
        case TOK_INCREMENT: return std::format("TOK_INCREMENT(\"{}\")", token.lexeme);
        case TOK_DECREMENT: return "TOK_DECREMENT";
        case TOK_BITWISE_AND: return "TOK_BITWISE_AND";
        case TOK_BITWISE_OR: return "TOK_BITWISE_OR";
        case TOK_BITWISE_XOR: return "TOK_BITWISE_XOR";
        case TOK_BITWISE_NOT: return "TOK_BITWISE_NOT";
        case TOK_LEFT_SHIFT: return "TOK_LEFT_SHIFT";
        case TOK_RIGHT_SHIFT: return "TOK_RIGHT_SHIFT";
        case TOK_EQUAL: return "TOK_EQUAL";
        case TOK_NOT_EQUAL: return "TOK_NOT_EQUAL";
        case TOK_LESS_THAN: return "TOK_LESS_THAN";
        case TOK_GREATER_THAN: return "TOK_GREATER_THAN";
        case TOK_LESS_EQUAL: return "TOK_LESS_EQUAL";
        case TOK_GREATER_EQUAL: return "TOK_GREATER_EQUAL";
        case TOK_LOGICAL_AND: return "TOK_LOGICAL_AND";
        case TOK_LOGICAL_OR: return "TOK_LOGICAL_OR";
        case TOK_LOGICAL_NOT: return "TOK_LOGICAL_NOT";
        case TOK_TERNARY_CONDITIONAL: return "TOK_TERNARY_CONDITIONAL";

        case TOK_PLUS_ASSIGN: return "TOK_PLUS_ASSIGN";
        case TOK_MINUS_ASSIGN: return "TOK_MINUS_ASSIGN";
        case TOK_MULTIPLY_ASSIGN: return "TOK_MULTIPLY_ASSIGN";
        case TOK_DIVIDE_ASSIGN: return "TOK_DIVIDE_ASSIGN";
        case TOK_MODULO_ASSIGN: return "TOK_MODULO_ASSIGN";
        case TOK_AND_ASSIGN: return "TOK_AND_ASSIGN";
        case TOK_OR_ASSIGN: return "TOK_OR_ASSIGN";
        case TOK_XOR_ASSIGN: return "TOK_XOR_ASSIGN";
        case TOK_LEFT_SHIFT_ASSIGN: return "TOK_LEFT_SHIFT_ASSIGN";
        case TOK_RIGHT_SHIFT_ASSIGN: return "TOK_RIGHT_SHIFT_ASSIGN";

        case TOK_DOT: return "TOK_DOT";

        case TOK_COMMA: return "TOK_COMMA";
        case TOK_COLON: return "TOK_COLON";
        case TOK_SEMICOLON: return "TOK_SEMICOLON";

        case TOK_LPAREN: return std::format("TOK_LPAREN(\"{}\")", token.lexeme);
        case TOK_RPAREN: return std::format("TOK_RPAREN(\"{}\")", token.lexeme);
        case TOK_LBRACE: return std::format("TOK_LBRACE(\"{}\")", token.lexeme);
        case TOK_RBRACE: return std::format("TOK_RBRACE(\"{}\")", token.lexeme);
        case TOK_LBRACKET: return std::format("TOK_LBRACKET(\"{}\")", token.lexeme);
        case TOK_RBRACKET: return std::format("TOK_RBRACKET(\"{}\")", token.lexeme);

        case TOK_INTEGER_LITERAL: return std::format("TOK_INTEGER_LITERAL(\"{}\")", token.lexeme);
        case TOK_FLOAT_LITERAL: return std::format("TOK_FLOAT_LITERAL(\"{}\")", token.lexeme);
        case TOK_CHAR_LITERAL: return "TOK_CHAR_LITERAL";
        case TOK_STRING_LITERAL: return "TOK_STRING_LITERAL";

        case TOK_IDENTIFIER: return std::format("TOK_IDENTIFIER(\"{}\")", token.lexeme);
        case TOK_ERROR: return "TOK_ERROR";
        case TOK_EOF: return "TOK_EOF";

        default: return "TOK_UNKNOWN";
    }
}
