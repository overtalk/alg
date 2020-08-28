#pragma once

#include "head.hpp"
#include "singleton.hpp"

class WaitGroup : public SingleTon<WaitGroup>
{
private:
    std::mutex mutex_;
    std::atomic<int> counter_;
    std::condition_variable cond_;

    WaitGroup()
        : counter_(0)
    {
    }

    virtual ~WaitGroup() = default;

public:
    using Ptr = std::shared_ptr<WaitGroup>;

    static Ptr Create()
    {
        struct make_shared_enabler : public WaitGroup
        {
        };
        return std::make_shared<make_shared_enabler>();
    }

    void Add(int i = 1)
    {
        counter_ += i;
    }

    void Done()
    {
        counter_--;
        cond_.notify_all();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> l(mutex_);
        cond_.wait(l, [&] { return counter_ <= 0; });
    }

    template<class Rep, class Period>
    void Wait(const std::chrono::duration<Rep, Period>& timeout)
    {
        std::unique_lock<std::mutex> l(mutex_);
        cond_.wait_for(l, timeout, [&] { return counter_ <= 0; });
    }
};

void work(int i, WaitGroup::Ptr wg)
{
    std::this_thread::sleep_for(std::chrono::seconds(i + 1));
    std::cout << "worker-" << i << " done the job" << std::endl;
    wg->Done();
}

void wait_group_test()
{
    std::cout << "-----------------wait_group_test-------------------" << std::endl;

    int count = 10;
    WaitGroup::Ptr wg = WaitGroup::Create();

    wg->Add(count);

    std::thread threads[10];
    for (int i = 0; i < count; ++i)
        threads[i] = std::thread(std::bind(std::forward<std::function<void(int, WaitGroup::Ptr)>>(work), i, wg));

    std::cout << "start to work" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (auto& th : threads)
        th.detach();

    std::cout << "wait for all workers!" << std::endl;
    wg->Wait();
    std::cout << "all work finished!" << std::endl;
}