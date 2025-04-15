#pragma once

#include <coroutine>
#include <utility>
//
#include <cutecoro/concepts/awaitable.hpp>
#include <cutecoro/concepts/future.hpp>

namespace cutecoro {

namespace concepts {

template <typename P>
concept Promise = requires(P p) {
    { p.get_return_object() } -> Future;
    { p.initial_suspend() } -> Awaitable;
    { p.final_suspend() } noexcept -> Awaitable;
    p.unhandled_exception();
    requires(requires(int v) { p.return_value(v); } || requires { p.return_void(); });
};

}  // namespace concepts

}  // namespace cutecoro

// #include<
