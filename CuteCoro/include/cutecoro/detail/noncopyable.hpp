#pragma once

namespace cutecoro {

struct NonCopyable {
protected:
    NonCopyable() = default;

    ~NonCopyable() = default;

    NonCopyable(NonCopyable&&) = default;

    NonCopyable& operator=(NonCopyable&&) = default;

    NonCopyable(const NonCopyable&) = delete;

    NonCopyable& operator=(const NonCopyable&) = delete;
};

}  // namespace cutecoro
