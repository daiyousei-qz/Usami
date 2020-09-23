#pragma once
#include "fmt/format.h"
#include <memory>

namespace usami
{
    using std::make_unique;
    using std::unique_ptr;

    using std::make_shared;
    using std::shared_ptr;

    template <typename TFmt, typename... TArgs>
    [[noreturn]] void Throw(const TFmt& fmt, const TArgs&... args)
    {
        throw std::runtime_error(fmt::format(fmt, args...));
    }

    template <typename TFmt, typename... TArgs>
    void Warn(const TFmt& fmt, const TArgs&... args)
    {
        fmt::print(fmt, args...);
    }
} // namespace usami

#define USAMI_DISABLE_COPY(KLASS)
#define USAMI_DISABLE_COPY_AND_MOVE(KLASS)

#define USAMI_STRINGIFY_AUX(X) #X
#define USAMI_STRINGIFY(X) USAMI_STRINGIFY_AUX(X)
#define USAMI_ERROR_MESSAGE(LEVEL, MSG)                                                            \
    ("[" __FILE__ ":" USAMI_STRINGIFY(__LINE__) "] " LEVEL " (" MSG ") FAILED\n")

//#define USAMI_DEBUG_MODE
#ifdef USAMI_DEBUG_MODE
#define USAMI_ASSERT(CONDITION)                                                                    \
    static_cast<void>(!!(CONDITION) ||                                                             \
                      (::usami::Throw(USAMI_ERROR_MESSAGE("ASSERT", #CONDITION)), 0))

#define USAMI_CHECK(CONDITION)                                                                     \
    static_cast<void>(!!(CONDITION) || (::usami::Warn(USAMI_ERROR_MESSAGE("CHECK", #CONDITION)), 0))
#else
#define USAMI_ASSERT(CONDITION)
#define USAMI_CHECK(CONDITION)
#endif // USAMI_DEBUG_MODE

#define USAMI_REQUIRE(CONDITION)                                                                   \
    static_cast<void>(!!(CONDITION) ||                                                             \
                      (::usami::Throw(USAMI_ERROR_MESSAGE("REQUIRE", #CONDITION)), 0))

#define USAMI_NO_IMPL() (::usami::Throw("no impl"))