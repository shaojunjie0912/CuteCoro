#pragma once

#include <fmt/core.h>
#include <unistd.h>

#include <cutecoro/detail/selector/event.hpp>
#include <vector>

namespace cutecoro {

// epoll 操作封装类
struct EpollSelector {
    EpollSelector() : epfd_(epoll_create1(0)) {
        if (epfd_ < 0) {
            perror("epoll_create1");
            throw;
        }
    }

    // 注册事件
    void RegisterEvent(Event const& event) {
        epoll_event ev;
        ev.events = event.flags;
        // 存储事件发生时要执行的任务协程句柄
        ev.data.ptr = const_cast<HandleInfo*>(&event.handle_info);  // NOTE: const_cast 去除常量属性
        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, event.fd, &ev) == 0) {
            ++register_event_count_;
        }
    }

    // 移除事件
    void RemoveEvent(const Event& event) {
        epoll_event ev;
        ev.events = event.flags;
        if (epoll_ctl(epfd_, EPOLL_CTL_DEL, event.fd, &ev) == 0) {
            --register_event_count_;
        }
    }

    /**
     * @brief 等待事件发生 (epoll_wait)
     *
     * @param timeout 超时等待时间, 单位: 毫秒
     * @return std::vector<Event>
     */
    std::vector<Event> Select(int timeout) {
        // errno = 0;
        std::vector<epoll_event> events;
        events.resize(register_event_count_);
        int num_events = epoll_wait(epfd_, events.data(), register_event_count_, timeout);
        std::vector<Event> result;
        for (int i = 0; i < num_events; ++i) {
            auto handle_info =
                reinterpret_cast<HandleInfo*>(events[i].data.ptr);  // void* -> HandleInfo*
            if (handle_info->handle != nullptr &&
                handle_info->handle != (Handle*)&handle_info->handle) {
                result.push_back(Event{.handle_info = *handle_info});
            } else {
                // NOTE: 特殊处理: 表示有事件发生, 但是句柄信息的句柄指针代表没有相应回调需要处理
                handle_info->handle = (Handle*)&handle_info->handle;
            }
        }
        return result;
    }

    ~EpollSelector() {
        if (epfd_ > 0) {
            close(epfd_);
        }
    }

    bool IsStop() { return register_event_count_ == 1; }

private:
    int epfd_;                     // epoll 文件描述符
    int register_event_count_{1};  // 注册事件数量
};

}  // namespace cutecoro
