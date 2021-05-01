#include <fstream>
#include <iostream>
#define REGLEX_USE_MACROS
#include <reglex/reglex.hpp>

// clang-format off
enum class TokenType : uint8_t
{
    // Should be ignored.---------------------------------------------------------
    COMMENT,
    // Single-character tokens.---------------------------------------------------
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, QUESTION, COLON,
    // One or two character tokens.-----------------------------------------------
    BANG_EQUAL, BANG, EQUAL, GREATER_EQUAL, LESS_EQUAL, GREATER, LESS, ASSIGN,
    // Keywords.
    AND, STRUCT, ELSE, FUN, FOR, IF, NIL, OR, PRINT, RETURN, SUPER, THIS,
    TRUE, FALSE, VAR, WHILE,
    // Literals.-----------------------------------------------------------------
    IDENTIFIER, STRING, NUMBER,
    // Represents a lexical error.------------------------------------------------
    ERROR,
};
// clang-format on

struct Matcher : reglex::Matcher<TokenType>
{
};
// clang-format off
template<> constexpr std::string_view Matcher::pattern<TokenType::COMMENT> = reglex::cstyle_comment;
template<> constexpr std::string_view Matcher::pattern<TokenType::LEFT_PAREN> = R"(\()";
template<> constexpr std::string_view Matcher::pattern<TokenType::RIGHT_PAREN> = R"(\))";
template<> constexpr std::string_view Matcher::pattern<TokenType::LEFT_BRACE> = R"(\{)";
template<> constexpr std::string_view Matcher::pattern<TokenType::RIGHT_BRACE> = R"(\})";
template<> constexpr std::string_view Matcher::pattern<TokenType::LEFT_BRACKET> = R"(\[)";
template<> constexpr std::string_view Matcher::pattern<TokenType::RIGHT_BRACKET> = R"(\])";
template<> constexpr std::string_view Matcher::pattern<TokenType::COMMA> = R"(,)";
template<> constexpr std::string_view Matcher::pattern<TokenType::DOT> = R"(\.)";
template<> constexpr std::string_view Matcher::pattern<TokenType::MINUS> = R"(\-)";
template<> constexpr std::string_view Matcher::pattern<TokenType::PLUS> = R"(\+)";
template<> constexpr std::string_view Matcher::pattern<TokenType::SEMICOLON> = R"(;)";
template<> constexpr std::string_view Matcher::pattern<TokenType::SLASH> = R"(/)";
template<> constexpr std::string_view Matcher::pattern<TokenType::STAR> = R"(\*)";
template<> constexpr std::string_view Matcher::pattern<TokenType::QUESTION> = R"(\?)";
template<> constexpr std::string_view Matcher::pattern<TokenType::COLON> = R"(:)";
template<> constexpr std::string_view Matcher::pattern<TokenType::BANG_EQUAL> = R"(!=)";
template<> constexpr std::string_view Matcher::pattern<TokenType::BANG> = R"(!)";
template<> constexpr std::string_view Matcher::pattern<TokenType::EQUAL> = R"(==)";
template<> constexpr std::string_view Matcher::pattern<TokenType::GREATER_EQUAL> = R"(>=)";
template<> constexpr std::string_view Matcher::pattern<TokenType::LESS_EQUAL> = R"(<=)";
template<> constexpr std::string_view Matcher::pattern<TokenType::GREATER> = R"(>)";
template<> constexpr std::string_view Matcher::pattern<TokenType::LESS> = R"(<)";
template<> constexpr std::string_view Matcher::pattern<TokenType::ASSIGN> = R"(=)";
template<> constexpr std::string_view Matcher::pattern<TokenType::AND> = REGLEX_KEYWORD("and");
template<> constexpr std::string_view Matcher::pattern<TokenType::STRUCT> = REGLEX_KEYWORD("struct");
template<> constexpr std::string_view Matcher::pattern<TokenType::ELSE> = REGLEX_KEYWORD("else");
template<> constexpr std::string_view Matcher::pattern<TokenType::FUN> = REGLEX_KEYWORD("fun");
template<> constexpr std::string_view Matcher::pattern<TokenType::FOR> = REGLEX_KEYWORD("for");
template<> constexpr std::string_view Matcher::pattern<TokenType::IF> = REGLEX_KEYWORD("if");
template<> constexpr std::string_view Matcher::pattern<TokenType::NIL> = REGLEX_KEYWORD("nil");
template<> constexpr std::string_view Matcher::pattern<TokenType::OR> = REGLEX_KEYWORD("or");
template<> constexpr std::string_view Matcher::pattern<TokenType::PRINT> = REGLEX_KEYWORD("print");
template<> constexpr std::string_view Matcher::pattern<TokenType::RETURN> = REGLEX_KEYWORD("return");
template<> constexpr std::string_view Matcher::pattern<TokenType::SUPER> = REGLEX_KEYWORD("super");
template<> constexpr std::string_view Matcher::pattern<TokenType::THIS> = REGLEX_KEYWORD("this");
template<> constexpr std::string_view Matcher::pattern<TokenType::TRUE> = REGLEX_KEYWORD("true");
template<> constexpr std::string_view Matcher::pattern<TokenType::FALSE> = REGLEX_KEYWORD("false");
template<> constexpr std::string_view Matcher::pattern<TokenType::VAR> = REGLEX_KEYWORD("var");
template<> constexpr std::string_view Matcher::pattern<TokenType::WHILE> = REGLEX_KEYWORD("while");
template<> constexpr std::string_view Matcher::pattern<TokenType::IDENTIFIER> = reglex::identifier;
template<> constexpr std::string_view Matcher::pattern<TokenType::STRING> = reglex::string;
template<> constexpr std::string_view Matcher::pattern<TokenType::NUMBER> = reglex::real_number;
template<> constexpr std::string_view Matcher::pattern<TokenType::ERROR> = reglex::non_whitespace;

template<> constexpr bool Matcher::filter_out<TokenType::COMMENT> = true;
// clang-format on

// Define the traits for the token
using TokenTraits = reglex::LexTraits<TokenType, Matcher>;

int main()
{
    std::ifstream file("test.lox");
    if (!file.is_open())
    {
        std::cout << "Failed to open file.\n";
        return 1;
    }
    using source_iter = std::istreambuf_iterator<char>;
    std::string const source{source_iter(file), source_iter{}};

    auto const res = reglex::lex<TokenTraits>(source);
    for (auto const& tok : res.tokens)
    {
        std::cout << magic_enum::enum_name(tok.type) << " " << tok.first_line << "\n";
    }
    return 0;
}

