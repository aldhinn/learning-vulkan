#if !defined(_VK_TUT_LOGGING_HEADER_)
#define _VK_TUT_LOGGING_HEADER_

// C++ only region.
#if defined(__cplusplus)
#include <iostream>
#include <stdexcept>
#include <filesystem>

// < --------------------------- Logging Macros --------------------------- >

// General logging macro.
#if !defined(VK_TUT_LOG)
#define VK_TUT_LOG(_LEVEL_, _COLOUR_CODE_, _OUTPUT_STREAM_, msg) \
_OUTPUT_STREAM_ << #_COLOUR_CODE_ << "[" << #_LEVEL_ << "]\033[0m\t" \
<< msg << " " << #_COLOUR_CODE_ << "[line " << __LINE__ << " of " \
<< ::std::filesystem::path(__FILE__).filename().string() << "]\033[0m\n"
#endif

// Trace log macro.
// This log messages doesn't show up in release builds.
#if !defined(VK_TUT_LOG_TRACE)
// Release mode defines.
#if defined(_NDEBUG) || defined(NDEBUG)
#define VK_TUT_LOG_TRACE(msg)
#else
#define VK_TUT_LOG_TRACE(msg) \
VK_TUT_LOG(TRACE, \033[0;94m, ::std::cout, msg)
#endif
#endif
// End Trace log macro.

// Debug log macro.
#if !defined(VK_TUT_LOG_DEBUG)
#define VK_TUT_LOG_DEBUG(msg) \
VK_TUT_LOG(DEBUG, \033[0;96m, ::std::cout, msg)
#endif
// End Debug log macro.

// Error log macro.
// Will throw a runtime exception when called.
#if !defined(VK_TUT_LOG_ERROR)
#define VK_TUT_LOG_ERROR(msg) \
VK_TUT_LOG(ERROR, \033[0;91m, ::std::cerr, msg); \
throw ::std::runtime_error(msg)
#endif

// < ------------------------- END Logging Macros ------------------------- >

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.