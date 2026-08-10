#include "ash/memory/lazy_instance.h"
#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>

namespace ash {
namespace {

// Use global atomic counters instead of member variable counters
std::atomic<int> g_ctor_count(0);
std::atomic<int> g_dtor_count(0);

// Simplified helper class: uses global counters
class ObjectWithCounters {
 public:
  ObjectWithCounters() { g_ctor_count++; }

  ~ObjectWithCounters() { g_dtor_count++; }
};

// Test fixture - resets global counters
class LazyInstanceTest : public testing::Test {
 protected:
  void SetUp() override {
    g_ctor_count = 0;
    g_dtor_count = 0;
  }
};

// Case 1: No construction/destruction calls without accessing after default
// construction
TEST_F(LazyInstanceTest, NoConstructionWithoutAccess) {
  {
    LazyInstance<ObjectWithCounters> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
    // Do not access the instance
  }
  EXPECT_EQ(g_ctor_count, 0);
  EXPECT_EQ(g_dtor_count, 0);
}

// Case 2: First Get() call triggers construction
TEST_F(LazyInstanceTest, FirstGetTriggersConstruction) {
  LazyInstance<ObjectWithCounters> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
  lazy.Get();
  EXPECT_EQ(g_ctor_count, 1);
  EXPECT_EQ(g_dtor_count, 0);
}

// Case 3: Repeated Get() returns the same instance
TEST_F(LazyInstanceTest, RepeatedGetReturnsSameInstance) {
  LazyInstance<ObjectWithCounters> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
  auto& ref1 = lazy.Get();
  auto& ref2 = lazy.Get();
  EXPECT_EQ(&ref1, &ref2);
  EXPECT_EQ(g_ctor_count, 1);  // Constructed only once
}

// Case 4: Pointer() access is consistent with Get()
TEST_F(LazyInstanceTest, PointerConsistency) {
  LazyInstance<ObjectWithCounters> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
  ObjectWithCounters* ptr = lazy.Pointer();
  ObjectWithCounters& ref = lazy.Get();
  EXPECT_EQ(ptr, &ref);
}

// Case 5: Destructor invokes the object's destructor
TEST_F(LazyInstanceTest, DestructorInvokesObjectDestruction) {
  {
    LazyInstance<ObjectWithCounters> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
    lazy.Get();
    EXPECT_EQ(g_dtor_count, 0);  // Not destructed after construction
  }
  EXPECT_EQ(g_ctor_count, 1);
  EXPECT_EQ(g_dtor_count,
            1);  // Object destruction triggered when lazy is destructed
}

// Case 6: Only constructed once under multithreading
TEST_F(LazyInstanceTest, SingleConstructionUnderThreads) {
  LazyInstance<ObjectWithCounters> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
  constexpr int kNumThreads = 100;
  std::vector<std::thread> threads;

  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back([&]() { lazy.Get(); });
  }

  for (auto& t : threads) {
    t.join();
  }

  EXPECT_EQ(g_ctor_count, 1);  // Ensure constructed only once
}

// Case 7: Alignment storage verification
TEST_F(LazyInstanceTest, AlignmentCheck) {
  struct alignas(128) AlignedType {
    int value;
  };

  LazyInstance<AlignedType> lazy = ASH_LAZY_INSTANCE_INITIALIZER;
  AlignedType* ptr = lazy.Pointer();
  const uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
  constexpr size_t alignment = alignof(AlignedType);

  EXPECT_EQ(address % alignment, static_cast<uintptr_t>(0))
      << "Address " << address << " not aligned to " << alignment;
}

// Case 8: Multiple instances are managed independently
TEST_F(LazyInstanceTest, MultipleIndependentInstances) {
  LazyInstance<ObjectWithCounters> lazy1 = ASH_LAZY_INSTANCE_INITIALIZER;
  LazyInstance<ObjectWithCounters> lazy2 = ASH_LAZY_INSTANCE_INITIALIZER;

  lazy1.Get();
  EXPECT_EQ(g_ctor_count, 1);

  // Reset counters to test the second instance
  g_ctor_count = 0;
  g_dtor_count = 0;

  lazy2.Get();
  EXPECT_EQ(g_ctor_count, 1);
  EXPECT_NE(&lazy1.Get(), &lazy2.Get());
}

}  // namespace
}  // namespace ash
