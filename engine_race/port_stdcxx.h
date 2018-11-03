// Copyright [2018] Alibaba Cloud All rights reserved
#ifndef ENGINE_RACE_PORT_STDCXX_H_
#define ENGINE_RACE_PORT_STDCXX_H_

#include <condition_variable>
#include <mutex>
#include <string>

namespace polar_race {
namespace port {

class Mutex {
 public:
  Mutex() = default;
  ~Mutex() = default;

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  void Lock() /* EXCLUSIVE_LOCK_FUNCTION() */ { mu_.lock(); }
  void Unlock() /* UNLOCK_FUNCTION() */ { mu_.unlock(); }
  void AssertHeld() /* ASSERT_EXCLUSIVE_LOCK() */ { }

 private:
  friend class CondVar;
  std::mutex mu_;
};

// Thinly wraps std::condition_variable.
class CondVar {
 public:
  explicit CondVar(Mutex* mu) : mu_(mu) { /* assert(mu != nullptr);  */ }
  ~CondVar() = default;

  CondVar(const CondVar&) = delete;
  CondVar& operator=(const CondVar&) = delete;

  void Wait() {
    std::unique_lock<std::mutex> lock(mu_->mu_, std::adopt_lock);
    cv_.wait(lock);
    lock.release();
  }
  void Signal() { cv_.notify_one(); }
  void SignalAll() { cv_.notify_all(); }
 private:
  std::condition_variable cv_;
  Mutex* const mu_;
};

}  // namespace port
}  // namespace polar_race

#endif  // ENGINE_RACE_ENGINE_RACE_H_
