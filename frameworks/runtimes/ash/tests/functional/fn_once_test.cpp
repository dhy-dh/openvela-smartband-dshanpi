#include "ash/functional/fn_once.h"
#include <gtest/gtest.h>
#include <string>

namespace ash {
namespace {

// Test case: No-argument lambda invocation
TEST(FnOnceTest, NoArgLambda) {
  bool called = false;
  FnOnce<void()> f([&] { called = true; });
  std::move(f)();  // Must be called as rvalue
  EXPECT_TRUE(called);
}

// Test case: Lambda with arguments and return value
TEST(FnOnceTest, WithArgsLambda) {
  FnOnce<int(int, int)> add([](int a, int b) { return a + b; });
  int result = std::move(add)(3, 4);
  EXPECT_EQ(result, 7);
}

// Test case: String arguments and return value
TEST(FnOnceTest, StringArgs) {
  FnOnce<std::string(std::string)> greet(
      [](std::string name) { return "Hello, " + name + "!"; });
  auto result = std::move(greet)("FnOnce");
  EXPECT_EQ(result, "Hello, FnOnce!");
}

// Test case: Move construction
TEST(FnOnceTest, MoveConstruction) {
  int value = 0;
  FnOnce<void()> f1([&] { value = 42; });
  FnOnce<void()> f2(std::move(f1));  // Move construction
  std::move(f2)();
  EXPECT_EQ(value, 42);
}

// Test case: Move assignment
TEST(FnOnceTest, MoveAssignment) {
  int value = 0;
  FnOnce<void()> f1([&] { value = 100; });
  FnOnce<void()> f2;
  f2 = std::move(f1);  // Move assignment
  std::move(f2)();
  EXPECT_EQ(value, 100);
}

// Test case: Resource cleanup verification
TEST(FnOnceTest, ResourceCleanup) {
  struct Resource {
    bool* destroyed;
    ~Resource() { *destroyed = true; }
  };

  bool destroyed = false;
  {
    FnOnce<void()> f([res = Resource{&destroyed}] {});
    std::move(f)();
  }
  EXPECT_TRUE(destroyed);
}

// Test case: Function pointer support
TEST(FnOnceTest, FunctionPointer) {
  int (*func)(int) = [](int x) { return x * 2; };
  FnOnce<int(int)> f(func);
  EXPECT_EQ(std::move(f)(5), 10);
}

// Test case: Functor support
TEST(FnOnceTest, Functor) {
  struct Doubler {
    int operator()(int x) { return x * 2; }
  };
  FnOnce<int(int)> f = Doubler{};
  EXPECT_EQ(std::move(f)(7), 14);
}

// Test case: Member function binding
TEST(FnOnceTest, MemberFunction) {
  struct Calculator {
    int add(int a, int b) { return a + b; }
  };

  Calculator calc;
  FnOnce<int(int, int)> f([&calc](int a, int b) { return calc.add(a, b); });
  EXPECT_EQ(std::move(f)(3, 4), 7);
}

// Test case: Void return
TEST(FnOnceTest, VoidReturn) {
  int counter = 0;
  FnOnce<void(int)> increment([&](int n) { counter += n; });
  std::move(increment)(5);
  EXPECT_EQ(counter, 5);
}

}  // namespace
}  // namespace ash
