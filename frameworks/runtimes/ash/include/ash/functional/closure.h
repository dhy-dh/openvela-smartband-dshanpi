#ifndef ASH_FUNCTIONAL_CLOSURE_H_
#define ASH_FUNCTIONAL_CLOSURE_H_

#include <functional>
#include "ash/functional/fn_once.h"

namespace ash {

using Closure = std::function<void()>;
using OnceClosure = FnOnce<void()>;

}  // namespace ash

#endif  // ASH_FUNCTIONAL_CLOSURE_H_
