#pragma once

class Noncopyable {
protected:
  Noncopyable() = default;
  ~Noncopyable() = default;

  Noncopyable(const Noncopyable &) = delete;
  Noncopyable &operator=(const Noncopyable &) = delete;
  Noncopyable(Noncopyable &&) = delete;
  Noncopyable &operator=(Noncopyable &&) = delete;
};

template <typename T> class SingleTon : public Noncopyable {
private:
  static T *instance_;
  static std::once_flag once_;

  static void init() {
    if (instance_ == nullptr) {
      instance_ = new T();
    }
  }

public:
  static T *Instance() {
    std::call_once(once_, &SingleTon<T>::init);
    return instance_;
  }

  static void Release() { delete instance_; }
};
