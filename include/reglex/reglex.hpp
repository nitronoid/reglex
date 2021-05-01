#pragma once
#if !defined(REGLEX_H)
#define REGLEX_H

#if !defined(REGLEX_NAMESPACE)
#define REGLEX_NAMESPACE reglex
#endif

#include <array>
#include <functional>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include <magic_enum.hpp>
#include <ctre/ctre.hpp>

namespace REGLEX_NAMESPACE
{
template <typename TokenT>
struct Matcher
{
    template <TokenT>
    static constexpr std::string_view pattern = "";
    template <TokenT>
    static constexpr bool filter_out = false;
};

template <typename TokenType>
struct Token
{
    using token_type_t = TokenType;
    token_type_t type;
    std::string_view lexeme;
    std::size_t first_line;
    std::size_t num_lines;
};

template <typename TokenT, typename Matcher>
struct LexTraits
{
    using token_type_t = TokenT;
    using token_t = Token<token_type_t>;
    using matcher_t = Matcher;

    template <std::size_t J>
    static constexpr token_type_t lookup = magic_enum::enum_value<token_type_t>(J);
    static constexpr std::size_t token_count = magic_enum::enum_count<token_type_t>();
};

namespace detail
{
template <typename, typename>
struct make_pattern;

template <typename Traits, std::size_t... I>
struct make_pattern<Traits, std::index_sequence<I...>>
{
    using token_type_t = typename Traits::token_type_t;
    using matcher_t = typename Traits::matcher_t;

    static constexpr auto impl() noexcept
    {
        constexpr char delim = '|';
        constexpr char group_open = '(';
        constexpr char group_close = ')';
        constexpr char terminate = '\0';
        // An open/close and delimiter for each group. The final delimiter gets replaced by a null
        // terminator
        constexpr std::size_t len =
            (Traits::token_count * 3) +
            (matcher_t::template pattern<Traits::template lookup<I>>.size() + ...);
        // Temp array
        char arr[len]{};
        // Append a new pattern
        auto append = [i = 0, &arr](auto const& s) mutable {
            arr[i++] = group_open;
            for (auto c : s)
            {
                arr[i++] = c;
            }
            arr[i++] = group_close;
            arr[i++] = delim;
        };
        // Append every enums pattern
        (append(matcher_t::template pattern<Traits::template lookup<I>>), ...);
        // Set the final char to a null terminator
        arr[len - 1] = terminate;
        return ctll::fixed_string{arr};
    }
};

// Final pattern stored in the fixed_string type for CTRE
template <typename Traits>
static constexpr ctll::fixed_string pattern =
    make_pattern<Traits, std::make_index_sequence<Traits::token_count>>::impl();

// Utility for static for loops with a compile time index
template <typename F, std::size_t... I>
constexpr void for_n(F&& f, std::index_sequence<I...>)
{
    (std::invoke(std::forward<F>(f), std::integral_constant<std::size_t, I>{}), ...);
}

template <std::size_t N, typename F>
constexpr void for_n(F&& f)
{
    for_n(std::forward<F>(f), std::make_index_sequence<N>{});
}
} // namespace detail

enum class Status
{
    NoMatch = 0,
    FilteredMatch = 1,
    UnfilteredMatch = 2
};

template <typename TokenT>
struct LexResult
{
    TokenT token{};
    Status status = Status::NoMatch;
};

template <typename Traits>
constexpr auto lex_token(std::string_view src, std::size_t line = 0)
{
    using token_t = typename Traits::token_t;
    using matcher_t = typename Traits::matcher_t;
    // Default to an EOF
    LexResult<token_t> result;
    // Attempt to match our grammar, produces a tuple of match results
    auto const matches = ctre::search<detail::pattern<Traits>>(src);
    // Function to check for matches in the result tuple
    auto const extract_match = [&](auto i) {
        auto const& group = matches.template get<i.value>();
        // Index zero is a full match, which we are not interested in
        if (i.value == 0 || !group) return;
        // Build a new token from this match groups token type
        // Guard with the multiply to ensure that we don't use a negative index
        constexpr auto type = Traits::template lookup<(i.value - 1) * !!i.value>;
        constexpr Status status = matcher_t::template filter_out<type> ? Status::FilteredMatch : Status::UnfilteredMatch;
        // Get a view to the substring which matched this tokens pattern
        auto const lexeme = group.view();
        // Calculate the line that the lexeme began on
        auto const first_line =
            line + static_cast<std::size_t>(std::count(src.begin(), lexeme.begin(), '\n'));
        // Calculate how many lines this lexeme spans
        auto const num_lines = static_cast<std::size_t>(std::count(lexeme.begin(), lexeme.end(), '\n'));
        // Set the result token
        result.token = token_t{type, lexeme, first_line, num_lines};
        result.status = status;
    };
    // Apply our matcher to each match group, with its group index
    detail::for_n<Traits::token_count>(extract_match);
    return result;
}

template <typename T>
struct Lexed
{
    std::vector<T> tokens;
    std::string_view remainder;
};

template <typename Traits>
Lexed<typename Traits::token_t> lex(std::string_view source)
{
    // Build this token list
    Lexed<typename Traits::token_t> res;
    // Keep track of the line we're processing
    std::size_t line = 0;
    [&]{
        // Consume until we're out of input characters
        while (!source.empty())
        {
            // Try to lex the next token
            auto lexed = lex_token<Traits>(source, line);
            switch (lexed.status)
            {
            case Status::NoMatch: return;
            case Status::FilteredMatch:
            case Status::UnfilteredMatch:
            {
                // Advance past the source for this lexeme
                source = source.substr(static_cast<std::size_t>(&lexed.token.lexeme.back() + 1 - source.data()));
                // Update the line number
                line = lexed.token.first_line + lexed.token.num_lines;
                // Add the token to our stream
                if (lexed.status == Status::UnfilteredMatch)
                    res.tokens.emplace_back(std::move(lexed.token));
                break;
            }
            }
        }
    }();
    res.remainder = source;
    return res;
}

/// Useful regex constants
static constexpr std::string_view identifier = R"([a-zA-Z_]\w*)";
static constexpr std::string_view cstyle_comment = R"((?://[^\n]*)|(?:/\*[^*]*\*+(?:[^/*][^*]*\*+)*/))";
static constexpr std::string_view string = R"("[^"]*")";
static constexpr std::string_view real_number = R"([0-9]+(?:\.[0-9]+)?)";
static constexpr std::string_view integer = R"([1-9][0-9]*)";
static constexpr std::string_view non_whitespace = R"([^\s]+)";
} // namespace REGLEX_NAMESPACE

#if defined(REGLEX_USE_MACROS)
#define REGLEX_KEYWORD(word) word R"((?=\W|$))"
#endif

#endif // REGLEX_H
