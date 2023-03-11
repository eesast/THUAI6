#pragma once

#ifndef CONCURRENT_QUEUE_HPP
#define CONCURRENT_QUEUE_HPP

#include <queue>
#include <mutex>
#include <utility>
#include <optional>

template<typename Elem>
class ConcurrentQueue
{
private:
    using queueType = std::queue<Elem>;

public:
    using sizeType = typename queueType::size_type;
    using valueType = typename queueType::value_type;
    using reference = typename queueType::reference;
    using constReference = typename queueType::const_reference;
    using containerType = typename queueType::container_type;

    ConcurrentQueue() = default;
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ~ConcurrentQueue() noexcept = default;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    void clear()
    {
        std::scoped_lock<std::mutex> lock(mtx);
        while (!q.empty())
            q.pop();
    }

    [[nodiscard]] bool empty() const
    {
        std::scoped_lock<std::mutex> lock(mtx);
        return q.empty();
    }

    template<typename... Ts>
    void emplace(Ts&&... args)
    {
        std::scoped_lock<std::mutex> lock(mtx);
        q.emplace(std::forward<Ts>(args)...);
    }

    [[nodiscard]] std::optional<valueType> tryPop()
    {
        std::scoped_lock<std::mutex> lock(mtx);
        if (q.empty())
            return std::nullopt;
        auto out = std::make_optional<valueType>(std::move(q.front()));
        q.pop();
        return out;
    }

private:
    mutable std::mutex mtx;
    queueType q;
};

#endif
