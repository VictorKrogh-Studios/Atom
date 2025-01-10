#pragma once

#include "Core.h"
#include <filesystem>

#include "Log.h"

#ifdef AT_DEBUG
#define AT_ENABLE_ASSERTS
#endif

#ifdef AT_ENABLE_ASSERTS
// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define AT_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { AT##type##ERROR(msg, __VA_ARGS__); __debugbreak(); } }
#define AT_INTERNAL_ASSERT_WITH_MSG(type, check, ...) AT_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define AT_INTERNAL_ASSERT_NO_MSG(type, check) AT_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", AT_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define AT_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define AT_INTERNAL_ASSERT_GET_MACRO(...) AT_EXPAND_MACRO( AT_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, AT_INTERNAL_ASSERT_WITH_MSG, AT_INTERNAL_ASSERT_NO_MSG) )

#define AT_ASSERT(...) AT_EXPAND_MACRO(AT_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define AT_CORE_ASSERT(...) AT_EXPAND_MACRO(AT_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define AT_ASSERT(...)
#define AT_CORE_ASSERT(...)
#endif

