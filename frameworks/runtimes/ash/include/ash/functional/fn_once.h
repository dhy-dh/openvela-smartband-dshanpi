#ifndef ASH_FUNCTIONAL_FN_ONCE_H_
#define ASH_FUNCTIONAL_FN_ONCE_H_

#include <memory>
#include <type_traits>
#include "ash/macros/disallow_copy.h"

namespace ash {

template <typename R, typename... Args>
class FnOnceStateBase {
 public:
  FnOnceStateBase() = default;
  virtual ~FnOnceStateBase() = default;

  virtual R Run(Args&&... args) && = 0;
  ASH_DISALLOW_COPY_AND_MOVE(FnOnceStateBase);
};

template <typename F, typename R, typename... Args>
class FnOnceState : public FnOnceStateBase<R, Args...> {
 public:
  FnOnceState(F functor) : functor_(std::move(functor)) {}
  ~FnOnceState() override = default;

  R Run(Args&&... args) && override {
    return std::move(functor_)(std::forward<Args>(args)...);
  }

 private:
  F functor_;
};

template <typename S>
class FnOnce;

// TODO(xuyan15): DisallowCopy doesn't work well on nuttx, provider another
// way to disable copy behaviors.
template <typename R, typename... Args>
class FnOnce<R(Args...)> {
 public:
  FnOnce() = default;
  ~FnOnce() = default;

  FnOnce(const FnOnce&) = delete;
  FnOnce(FnOnce&&) = default;
  FnOnce& operator=(const FnOnce&) = delete;
  FnOnce& operator=(FnOnce&&) = default;

  template <typename F,
            typename = std::enable_if_t<std::is_invocable_v<F, Args...>>>
  FnOnce(F functor)
      : state_(
            std::make_unique<FnOnceState<F, R, Args...>>(std::move(functor))) {}

  R operator()(Args&&... args) && {
    auto state = std::move(state_);
    return std::move(*state).Run(std::forward<Args>(args)...);
  }

  operator bool() const { return !!state_; }

  bool operator!() const { return !state_; }

 private:
  std::unique_ptr<FnOnceStateBase<R, Args...>> state_;
};

}  // namespace ash

#endif  // ASH_FUNCTIONAL_FN_ONCE_H_
