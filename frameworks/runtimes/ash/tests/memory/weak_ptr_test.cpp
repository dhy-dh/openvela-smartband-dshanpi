#include "ash/memory/weak_ptr.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>

namespace ash {
namespace {

// Test helper class
class TestObject {
 public:
  int value = 42;
  int GetValue() const { return value; }
};

class TestSupportsWeakPtr : public SupportsWeakPtr<TestSupportsWeakPtr> {
 public:
  TestSupportsWeakPtr() : SupportsWeakPtr<TestSupportsWeakPtr>(this) {}
  explicit TestSupportsWeakPtr(int v)
      : SupportsWeakPtr<TestSupportsWeakPtr>(this), value(v) {}
  void DoSomething() {}
  int value = 0;
};

// WeakPtrImpl Tests ================================================
TEST(WeakPtrImplTest, ConstructWithValidPointer) {
  TestObject obj;
  WeakPtrImpl<TestObject> impl(&obj);
  EXPECT_TRUE(impl.IsAlive());
}

TEST(WeakPtrImplTest, GetReturnsCorrectPointer) {
  TestObject obj;
  WeakPtrImpl<TestObject> impl(&obj);
  EXPECT_EQ(impl.Get(), &obj);
}

TEST(WeakPtrImplTest, ArrowOperatorAccess) {
  TestObject obj;
  WeakPtrImpl<TestObject> impl(&obj);
  EXPECT_EQ(impl->value, 42);
}

TEST(WeakPtrImplTest, ResetMakesIsAliveFalse) {
  TestObject obj;
  WeakPtrImpl<TestObject> impl(&obj);
  impl.Reset();
  EXPECT_FALSE(impl.IsAlive());
}

TEST(WeakPtrImplTest, ResetMakesGetNull) {
  TestObject obj;
  WeakPtrImpl<TestObject> impl(&obj);
  impl.Reset();
  EXPECT_EQ(impl.Get(), nullptr);
}

TEST(WeakPtrImplTest, IsAliveAfterReset) {
  TestObject obj;
  WeakPtrImpl<TestObject> impl(&obj);
  EXPECT_TRUE(impl.IsAlive());
  impl.Reset();
  EXPECT_FALSE(impl.IsAlive());
}

// WeakPtr Basic Functionality Tests
// =============================================

TEST(WeakPtrTest, DefaultConstructedIsInvalid) {
  WeakPtr<TestObject> weak_ptr;
  EXPECT_FALSE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), nullptr);
}

TEST(WeakPtrTest, ConstructFromValidImpl) {
  TestObject obj;
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(&obj);
  WeakPtr<TestObject> weak_ptr(impl);

  EXPECT_TRUE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), &obj);
}

TEST(WeakPtrTest, CopyConstructor) {
  TestObject obj;
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(&obj);
  WeakPtr<TestObject> weak_ptr1(impl);
  WeakPtr<TestObject> weak_ptr2(weak_ptr1);

  EXPECT_TRUE(weak_ptr1.IsAlive());
  EXPECT_TRUE(weak_ptr2.IsAlive());
  EXPECT_EQ(weak_ptr1.Get(), &obj);
  EXPECT_EQ(weak_ptr2.Get(), &obj);
}

TEST(WeakPtrTest, CopyAssignment) {
  TestObject obj;
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(&obj);

  WeakPtr<TestObject> weak_ptr1(impl);
  WeakPtr<TestObject> weak_ptr2;
  weak_ptr2 = weak_ptr1;

  EXPECT_TRUE(weak_ptr1.IsAlive());
  EXPECT_TRUE(weak_ptr2.IsAlive());
  EXPECT_EQ(weak_ptr1.Get(), &obj);
  EXPECT_EQ(weak_ptr2.Get(), &obj);
}

TEST(WeakPtrTest, AccessObjectMembers) {
  TestObject obj;
  obj.value = 100;
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(&obj);
  WeakPtr<TestObject> weak_ptr(impl);

  EXPECT_EQ(weak_ptr.Get()->GetValue(), 100);
  EXPECT_EQ(weak_ptr.Get()->value, 100);
}

// Lifetime Management Tests ================================================

TEST(WeakPtrTest, BecomesInvalidAfterImplReset) {
  TestObject obj;
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(&obj);
  WeakPtr<TestObject> weak_ptr(impl);

  impl->Reset();
  EXPECT_FALSE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), nullptr);
}

TEST(WeakPtrTest, SupportsWeakPtrBasicUsage) {
  auto obj = std::make_unique<TestSupportsWeakPtr>();
  WeakPtr<TestSupportsWeakPtr> weak_ptr = obj->AsWeakPtr();

  EXPECT_TRUE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), obj.get());
}

TEST(WeakPtrTest, ObjectDestructionInvalidatesWeakPtr) {
  WeakPtr<TestSupportsWeakPtr> weak_ptr;
  {
    auto obj = std::make_unique<TestSupportsWeakPtr>();
    weak_ptr = obj->AsWeakPtr();
    EXPECT_TRUE(weak_ptr.IsAlive());
  }

  EXPECT_FALSE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), nullptr);
}

TEST(WeakPtrTest, ManualResetInvalidatesWeakPtr) {
  auto obj = std::make_unique<TestSupportsWeakPtr>();
  WeakPtr<TestSupportsWeakPtr> weak_ptr = obj->AsWeakPtr();

  obj->Reset();
  EXPECT_FALSE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), nullptr);
}

TEST(WeakPtrTest, MultipleWeakPtrsSameObject) {
  auto obj = std::make_unique<TestSupportsWeakPtr>();
  WeakPtr<TestSupportsWeakPtr> weak_ptr1 = obj->AsWeakPtr();
  WeakPtr<TestSupportsWeakPtr> weak_ptr2 = obj->AsWeakPtr();

  EXPECT_TRUE(weak_ptr1.IsAlive());
  EXPECT_TRUE(weak_ptr2.IsAlive());
  EXPECT_EQ(weak_ptr1.Get(), obj.get());
  EXPECT_EQ(weak_ptr2.Get(), obj.get());

  obj.reset();

  EXPECT_FALSE(weak_ptr1.IsAlive());
  EXPECT_FALSE(weak_ptr2.IsAlive());
}

TEST(WeakPtrTest, CopyAfterObjectDestruction) {
  WeakPtr<TestSupportsWeakPtr> original;
  {
    auto obj = std::make_unique<TestSupportsWeakPtr>();
    original = obj->AsWeakPtr();
  }

  WeakPtr<TestSupportsWeakPtr> copy(original);
  EXPECT_FALSE(original.IsAlive());
  EXPECT_FALSE(copy.IsAlive());
}

// Thread Safety Tests ================================================

constexpr int kThreadCount = 100;
constexpr int kIterations = 1000;

TEST(WeakPtrTest, ThreadSafety) {
  auto obj = std::make_shared<TestSupportsWeakPtr>();
  std::vector<WeakPtr<TestSupportsWeakPtr>> weak_ptrs(kThreadCount);
  std::vector<std::thread> threads;

  // Create threads to concurrently obtain WeakPtr
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back([&, i] {
      for (int j = 0; j < kIterations; ++j) {
        weak_ptrs[i] = obj->AsWeakPtr();
      }
    });
  }

  for (auto& t : threads)
    t.join();

  // Verify all WeakPtrs are valid
  for (const auto& weak_ptr : weak_ptrs) {
    EXPECT_TRUE(weak_ptr.IsAlive());
    EXPECT_EQ(weak_ptr.Get(), obj.get());
  }

  // Reset object and verify WeakPtrs become invalid
  obj.reset();
  for (const auto& weak_ptr : weak_ptrs) {
    EXPECT_FALSE(weak_ptr.IsAlive());
  }
}

// Edge Cases Tests ===================================================

TEST(WeakPtrTest, NullObjectConstruction) {
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(nullptr);
  WeakPtr<TestObject> weak_ptr(impl);

  EXPECT_FALSE(weak_ptr.IsAlive());
  EXPECT_EQ(weak_ptr.Get(), nullptr);
}

TEST(WeakPtrTest, CopyFromInvalidWeakPtr) {
  WeakPtr<TestObject> invalid_ptr;
  WeakPtr<TestObject> copy(invalid_ptr);

  EXPECT_FALSE(copy.IsAlive());
}

TEST(WeakPtrTest, MoveFromInvalidWeakPtr) {
  WeakPtr<TestObject> invalid_ptr;
  WeakPtr<TestObject> moved(std::move(invalid_ptr));

  EXPECT_FALSE(moved.IsAlive());
}

TEST(WeakPtrTest, AssignToValidFromInvalid) {
  TestObject obj;
  auto impl = std::make_shared<WeakPtrImpl<TestObject>>(&obj);
  WeakPtr<TestObject> valid_ptr(impl);
  WeakPtr<TestObject> invalid_ptr;

  valid_ptr = invalid_ptr;
  EXPECT_FALSE(valid_ptr.IsAlive());
}

TEST(WeakPtrTest, ChainedAssignment) {
  TestObject obj1, obj2;
  auto impl1 = std::make_shared<WeakPtrImpl<TestObject>>(&obj1);
  auto impl2 = std::make_shared<WeakPtrImpl<TestObject>>(&obj2);

  WeakPtr<TestObject> ptr1(impl1);
  WeakPtr<TestObject> ptr2(impl2);
  WeakPtr<TestObject> ptr3;

  ptr3 = ptr2 = ptr1;

  EXPECT_TRUE(ptr1.IsAlive());
  EXPECT_TRUE(ptr2.IsAlive());
  EXPECT_TRUE(ptr3.IsAlive());
  EXPECT_EQ(ptr1.Get(), &obj1);
  EXPECT_EQ(ptr2.Get(), &obj1);
  EXPECT_EQ(ptr3.Get(), &obj1);
}

// Type System Tests ===================================================

TEST(WeakPtrTest, DerivedClassSupport) {
  class Base : public SupportsWeakPtr<Base> {
   public:
    Base() : SupportsWeakPtr<Base>(this) {}
    virtual ~Base() = default;
    virtual int Value() const { return 1; }
  };

  class Derived : public Base {
   public:
    int Value() const override { return 2; }
  };

  auto derived = std::make_unique<Derived>();
  WeakPtr<Base> base_ptr = derived->AsWeakPtr();

  EXPECT_TRUE(base_ptr.IsAlive());
  EXPECT_EQ(base_ptr.Get()->Value(), 2);
}

}  // namespace
}  // namespace ash
