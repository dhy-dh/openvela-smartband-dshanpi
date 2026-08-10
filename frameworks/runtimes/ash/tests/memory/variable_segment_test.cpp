#include "ash/memory/variable_segment.h"
#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <vector>

namespace ash {
namespace {

// Test helper class
struct TrackedObject {
  TrackedObject() { default_constructed++; }
  explicit TrackedObject(int v) : value(v) { arg_constructed++; }
  explicit TrackedObject(std::string s) : str(std::move(s)) {}
  ~TrackedObject() { destructed++; }

  int value = 0;
  std::string str;

  static int default_constructed;
  static int arg_constructed;
  static int destructed;

  static void ResetCounters() {
    default_constructed = 0;
    arg_constructed = 0;
    destructed = 0;
  }
};

int TrackedObject::default_constructed = 0;
int TrackedObject::arg_constructed = 0;
int TrackedObject::destructed = 0;

// Basic test fixture
class VariableSegmentTest : public testing::Test {
 protected:
  void SetUp() override { TrackedObject::ResetCounters(); }

  VariableSegmentDefination def;
};

// 1. Test basic registration functionality
TEST_F(VariableSegmentTest, RegisterSingleVariable) {
  const int offset = def.RegisterVariable(
      sizeof(int), [](void* addr) { *static_cast<int*>(addr) = 42; },
      [](void* addr) { *static_cast<int*>(addr) = 0; });

  EXPECT_EQ(offset, 0);
}

// 2. Test basic segment creation functionality
TEST_F(VariableSegmentTest, CreateSegmentWithSingleVariable) {
  const int offset = def.RegisterVariable(
      sizeof(int), [](void* addr) { *static_cast<int*>(addr) = 42; },
      [](void* addr) {});

  uint8_t* segment = def.CreateSegment();
  EXPECT_EQ(*reinterpret_cast<int*>(segment + offset), 42);
  def.DestroySegment(segment);
}

// 3. Test basic segment destruction functionality
TEST_F(VariableSegmentTest, DestroySegmentCallsDestructor) {
  bool destructor_called = false;
  def.RegisterVariable(
      sizeof(int), [](void* addr) { *static_cast<int*>(addr) = 42; },
      [&](void* addr) { destructor_called = true; });

  uint8_t* segment = def.CreateSegment();
  def.DestroySegment(segment);
  EXPECT_TRUE(destructor_called);
}

// 4. Test 1-byte variable alignment
TEST_F(VariableSegmentTest, AlignmentFor1ByteVariable) {
  def.RegisterVariable(
      1, [](void*) {}, [](void*) {});
  const int offset = def.RegisterVariable(
      1, [](void*) {}, [](void*) {});
  EXPECT_EQ(offset, 1);
}

// 5. Test 2-byte variable alignment
TEST_F(VariableSegmentTest, AlignmentFor2ByteVariable) {
  def.RegisterVariable(
      1, [](void*) {}, [](void*) {});
  const int offset = def.RegisterVariable(
      2, [](void*) {}, [](void*) {});
  EXPECT_EQ(offset, 2);
}

// 6. Test 4-byte variable alignment
TEST_F(VariableSegmentTest, AlignmentFor4ByteVariable) {
  def.RegisterVariable(
      1, [](void*) {}, [](void*) {});
  const int offset = def.RegisterVariable(
      4, [](void*) {}, [](void*) {});
  EXPECT_EQ(offset, 4);
}

// 7. Test 8-byte variable alignment (64-bit only)
#if defined(ASH_ARCH_64)
TEST_F(VariableSegmentTest, AlignmentFor8ByteVariable) {
  def.RegisterVariable(
      1, [](void*) {}, [](void*) {});
  const int offset = def.RegisterVariable(
      8, [](void*) {}, [](void*) {});
  EXPECT_EQ(offset, 8);
}
#endif

// 8. Test template registration with arguments
TEST_F(VariableSegmentTest, TemplateRegisterWithArguments) {
  const int offset = def.RegisterVariableWithArgs<TrackedObject>(42);
  uint8_t* segment = def.CreateSegment();
  TrackedObject* obj = reinterpret_cast<TrackedObject*>(segment + offset);
  EXPECT_EQ(obj->value, 42);
  def.DestroySegment(segment);
}

// 9. Test template registration with default construction
TEST_F(VariableSegmentTest, TemplateRegisterDefaultConstruction) {
  const int offset = def.RegisterVariableWithArgs<TrackedObject>();
  uint8_t* segment = def.CreateSegment();
  TrackedObject* obj = reinterpret_cast<TrackedObject*>(segment + offset);
  EXPECT_EQ(obj->value, 0);
  def.DestroySegment(segment);
}

// 10. Test registration with custom initializer
TEST_F(VariableSegmentTest, RegisterWithCustomInitializer) {
  const int offset = def.RegisterVariableWithInitializer<std::string>(
      [] { return "initialized_value"; });

  uint8_t* segment = def.CreateSegment();
  std::string* str = reinterpret_cast<std::string*>(segment + offset);
  EXPECT_EQ(*str, "initialized_value");
  def.DestroySegment(segment);
}

// 11. Test multiple variable offset calculation
TEST_F(VariableSegmentTest, MultipleVariableOffsets) {
  const int offset1 = def.RegisterVariable(
      1, [](void*) {}, [](void*) {});
  const int offset2 = def.RegisterVariable(
      4, [](void*) {}, [](void*) {});
  const int offset3 = def.RegisterVariable(
      8, [](void*) {}, [](void*) {});

  EXPECT_EQ(offset1, 0);
  EXPECT_EQ(offset2, 4);
  EXPECT_EQ(offset3, 8);
}

// 12. Test multiple variable construction order
TEST_F(VariableSegmentTest, MultipleVariableConstruction) {
  std::vector<int> construction_order;

  def.RegisterVariable(
      4, [&](void* addr) { construction_order.push_back(1); }, [](void*) {});

  def.RegisterVariable(
      4, [&](void* addr) { construction_order.push_back(2); }, [](void*) {});

  uint8_t* segment = def.CreateSegment();
  EXPECT_EQ(construction_order.size(), static_cast<size_t>(2));
  EXPECT_EQ(construction_order[0], 1);
  EXPECT_EQ(construction_order[1], 2);
  def.DestroySegment(segment);
}

// 13. Test multiple variable destruction order
TEST_F(VariableSegmentTest, MultipleVariableDestruction) {
  std::vector<int> destruction_order;

  def.RegisterVariable(
      4, [](void*) {}, [&](void* addr) { destruction_order.push_back(1); });

  def.RegisterVariable(
      4, [](void*) {}, [&](void* addr) { destruction_order.push_back(2); });

  uint8_t* segment = def.CreateSegment();
  def.DestroySegment(segment);

  EXPECT_EQ(destruction_order.size(), static_cast<size_t>(2));
  EXPECT_EQ(destruction_order[0], 1);
  EXPECT_EQ(destruction_order[1], 2);
}

// 14. Test construction/destruction count
TEST_F(VariableSegmentTest, ConstructionDestructionCount) {
  def.RegisterVariableWithArgs<TrackedObject>(42);
  uint8_t* segment = def.CreateSegment();
  EXPECT_EQ(TrackedObject::arg_constructed, 1);

  def.DestroySegment(segment);
  EXPECT_EQ(TrackedObject::destructed, 1);
}

// 15. Test complex object lifecycle
TEST_F(VariableSegmentTest, ComplexObjectLifecycle) {
  struct ResourceHolder {
    explicit ResourceHolder(int* hcounter) : counter(hcounter) { (*counter)++; }
    ~ResourceHolder() { (*counter)--; }
    int* counter;
  };

  int resource_count = 0;
  def.RegisterVariableWithArgs<ResourceHolder>(&resource_count);

  uint8_t* segment = def.CreateSegment();
  EXPECT_EQ(resource_count, 1);

  def.DestroySegment(segment);
  EXPECT_EQ(resource_count, 0);
}

// 16. Test memory non-overlapping
TEST_F(VariableSegmentTest, MemoryNoOverlap) {
  const int offset1 = def.RegisterVariable(
      10, [](void* addr) { std::memset(addr, 0xAA, 10); }, [](void*) {});

  const int offset2 = def.RegisterVariable(
      10, [](void* addr) { std::memset(addr, 0xBB, 10); }, [](void*) {});

  EXPECT_GE(offset2, offset1 + 10);
  uint8_t* segment = def.CreateSegment();

  // Check memory for the first variable
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(segment[offset1 + i], 0xAA);
  }

  // Check memory for the second variable
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(segment[offset2 + i], 0xBB);
  }

  def.DestroySegment(segment);
}

// 17. Test string variable lifecycle
TEST_F(VariableSegmentTest, StringVariableLifecycle) {
  const int offset = def.RegisterVariableWithArgs<std::string>("test_content");

  uint8_t* segment = def.CreateSegment();
  std::string* str = reinterpret_cast<std::string*>(segment + offset);
  EXPECT_EQ(*str, "test_content");

  // Verify memory management by modifying content
  str->append("_modified");
  EXPECT_EQ(*str, "test_content_modified");

  def.DestroySegment(segment);
}

}  // namespace
}  // namespace ash
