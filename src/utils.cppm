module;

#include <algorithm>
#include <array>
#include <source_location>
#include <string_view>
#include <utility>

export module utils;

namespace utils {

export template<std::array src, size_t start, size_t length>
constexpr auto substr() {
    std::array<char, length> result{};
    std::copy(src.begin() + start, src.begin() + start + length, result.begin());
    return result;
}

export template<std::array str, std::array from, std::array to>
constexpr auto replace_sub_str() {
    constexpr std::string_view sv{str.data(), str.size()};
    constexpr std::string_view from_sv{from.data(), from[from.size()-1] == '\0' ? from.size()-1 : from.size()};
    constexpr std::string_view to_sv{to.data(), to[to.size()-1] == '\0' ? to.size()-1 : to.size()};
    // Do NOT use "str", "from" or "to", they might or might not be null-terminated

    constexpr auto count = [](std::string_view sv, std::string_view needle) {
        unsigned int count = 0;
        size_t pos = 0;
        while((pos = sv.find(needle, pos)) != std::string_view::npos) {
            pos += needle.size();
            ++count;
        }
        return count;
    };
    constexpr unsigned int new_size = sv.size() + count(sv, from_sv) * (to_sv.size() - from_sv.size());
    std::array<char, new_size> result{};

    size_t pos = 0;
    size_t start = 0;
    size_t out_pos = 0;
    while((pos = sv.find(from_sv, start)) != std::string_view::npos) {
        std::copy(sv.begin() + start, sv.begin() + pos, result.begin() + out_pos);
        out_pos += pos - start;
        std::copy(to_sv.begin(), to_sv.end(), result.begin() + out_pos);
        out_pos += to_sv.size();
        start = pos + from_sv.size();
    }
    std::copy(sv.begin() + start, sv.end(), result.begin() + out_pos);

    return result;
}

export template<typename T>
constexpr std::string_view get_typename() {
    std::string_view name = std::source_location::current().function_name();
    name.remove_prefix(std::string_view::traits_type::length("std::string_view utils::get_typename()"));
    name.remove_prefix(std::string_view::traits_type::length(" [T = "));
    name.remove_suffix(std::string_view::traits_type::length("]"));
    return name;
}
// based on https://stackoverflow.com/questions/28828957/how-to-convert-an-enum-to-a-string-in-modern-c#comment115999268_55312360
template<auto... values>
constexpr std::string_view get_enum_values_impl() {
    return std::source_location::current().function_name();
}
template<size_t N>
constexpr std::array<std::string_view, N> parse_enum_list(std::string_view sv, size_t prefix = 0) {
    std::array<std::string_view, N> array;
    size_t p = prefix + 2;
    for(size_t i = 0; i<N; i++) {
        size_t np = sv.find(",", p);
        auto name = sv.substr(p, np-p);
        array[i] = name;
        p = np + prefix + 2 + 2;
    }
    return array;
}
// based on https://stackoverflow.com/questions/28828957/how-to-convert-an-enum-to-a-string-in-modern-c#comment115999268_55312360
template<typename Enum, auto... values>
constexpr std::string_view get_enum_values_str(std::index_sequence<values...>) {
    auto sv = get_enum_values_impl<static_cast<Enum>(values)...>();
    sv.remove_prefix(std::string_view::traits_type::length("std::string_view utils::get_enum_values_impl() [values = <"));
    sv.remove_suffix(std::string_view::traits_type::length(">]"));
    return sv;
}
export template<typename Enum, size_t N = 256>
constexpr std::array<std::string_view, N> get_enum_values() {
    constexpr auto sv = get_enum_values_str<Enum>(std::make_index_sequence<N>());
    return parse_enum_list<N>(sv, get_typename<Enum>().size());
}
export template<typename Enum, size_t N = 256>
constexpr std::string_view enum_name(Enum e) {
    constexpr auto list = get_enum_values<Enum, N>();
    return list[std::to_underlying(e)];
}


#ifdef __clang__
export constexpr auto cxx_compiler_name = "clang++";
export constexpr auto cxx_compiler_version_major = __clang_major__;
export constexpr auto cxx_compiler_version_minor = __clang_minor__;
export constexpr auto cxx_compiler_version_patch = __clang_patchlevel__;
#else
export constexpr auto cxx_compiler_name = "g++";
export constexpr auto cxx_compiler_version_major = __GNUC__;
export constexpr auto cxx_compiler_version_minor = __GNUC_MINOR__;
export constexpr auto cxx_compiler_version_patch = __GNUC_PATCHLEVEL__;
#endif

}
