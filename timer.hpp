#pragma once

#include "head.hpp"

class TimerMgr;

class Timer final
{
public:
    using Ptr = std::shared_ptr<Timer>;
    using WeakPtr = std::weak_ptr<Timer>;
    using Callback = std::function<void(void)>;

private:
    std::once_flag once_;
    Callback callback_;
    const std::chrono::steady_clock::time_point start_time_;
    const std::chrono::nanoseconds last_time_;

    friend class TimerMgr;

public:
    Timer(std::chrono::steady_clock::time_point startTime, std::chrono::nanoseconds lastTime, Callback&& callback)
        : callback_(std::move(callback))
        , start_time_(startTime)
        , last_time_(lastTime)
    {
    }

    const std::chrono::steady_clock::time_point& GetStartTime() const
    {
        return start_time_;
    }

    const std::chrono::nanoseconds& GetLastTime() const
    {
        return last_time_;
    }

    std::chrono::nanoseconds GetLeftTime() const
    {
        const auto now = std::chrono::steady_clock::now();
        return GetLastTime() - (now - GetStartTime());
    }

    void Cancel()
    {
        std::call_once(once_, [this]() { callback_ = nullptr; });
    }

private:
    void operator()()
    {
        Callback callback;
        std::call_once(once_, [&callback, this]() {
            callback = std::move(callback_);
            callback_ = nullptr;
        });

        if (callback != nullptr)
            callback();
    }
};

class TimerMgr final
{
private:
    class CompareTimer
    {
    public:
        bool operator()(const Timer::Ptr& left, const Timer::Ptr& right) const
        {
            const auto startDiff = left->GetStartTime() - right->GetStartTime();
            const auto lastDiff = left->GetStartTime() - right->GetStartTime();
            const auto diff = startDiff.count() + lastDiff.count();
            return diff > 0;
        }
    };

    // priority_queue : 优先队列，类似heap
    std::priority_queue<Timer::Ptr, std::vector<Timer::Ptr>, CompareTimer> timers_;

public:
    using Ptr = std::shared_ptr<TimerMgr>;

    template<typename F, typename... TArgs>
    Timer::WeakPtr AddTimer(std::chrono::nanoseconds timeout, F&& callback, TArgs&&... args)
    {
        auto timer = std::make_shared<Timer>(std::chrono::steady_clock::now(), std::chrono::nanoseconds(timeout),
            std::bind(std::forward<F>(callback), std::forward<TArgs>(args)...));
        timers_.push(timer);
        return timer;
    }

    void AddTimer(const Timer::Ptr& timer)
    {
        timers_.push(timer);
    }

    bool IsEmpty() const
    {
        return timers_.empty();
    }

    void Schedule()
    {

        while (!timers_.empty())
        {
            auto tmp = timers_.top();
            if (tmp->GetLeftTime() > std::chrono::nanoseconds::zero())
            {
                break;
            }

            timers_.pop();
            (*tmp)();
        }
    }

    // if timer empty, return zero
    std::chrono::nanoseconds NearLeftTime() const
    {
        if (timers_.empty())
        {
            return std::chrono::nanoseconds::zero();
        }

        auto result = timers_.top()->GetLeftTime();
        if (result < std::chrono::nanoseconds::zero())
        {
            return std::chrono::nanoseconds::zero();
        }

        return result;
    }

    void Clear()
    {
        while (!timers_.empty())
        {
            timers_.pop();
        }
    }
};

void timer_test()
{
    std::cout << "-----------------timer_test-------------------" << std::endl;

    bool flag = false;
    TimerMgr timerMgr;
    timerMgr.AddTimer(std::chrono::nanoseconds(1000 * 1000 * 2), [&flag]() {
        std::cout << "hello timer " << std::endl;
        flag = true;
    });

    while (!flag)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::cout << "+ sleep 1 millisecond +" << std::endl;
        timerMgr.Schedule();
    }
}