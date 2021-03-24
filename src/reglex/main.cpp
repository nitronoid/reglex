#include <fstream>
#include <iostream>
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

struct Matcher
{
    template <TokenType>
    static constexpr std::string_view pattern = "";
};
// clang-format off
template<> constexpr std::string_view Matcher::pattern<TokenType::COMMENT> = R"((?://[^\n]*)|(?:/\*[^*]*\*+(?:[^/*][^*]*\*+)*/))";
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
template<> constexpr std::string_view Matcher::pattern<TokenType::AND> = R"(and(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::STRUCT> = R"(struct(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::ELSE> = R"(else(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::FUN> = R"(fun(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::FOR> = R"(for(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::IF> = R"(if(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::NIL> = R"(nil(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::OR> = R"(or(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::PRINT> = R"(print(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::RETURN> = R"(return(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::SUPER> = R"(super(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::THIS> = R"(this(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::TRUE> = R"(true(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::FALSE> = R"(false(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::VAR> = R"(var(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::WHILE> = R"(while(?=\W|$))";
template<> constexpr std::string_view Matcher::pattern<TokenType::IDENTIFIER> = R"([a-zA-Z_]+\w*)";
template<> constexpr std::string_view Matcher::pattern<TokenType::STRING> = R"("[^"]*")";
template<> constexpr std::string_view Matcher::pattern<TokenType::NUMBER> = R"([0-9]+(?:\.[0-9]+)?)";
template<> constexpr std::string_view Matcher::pattern<TokenType::ERROR> = R"([^\s]+)";
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

