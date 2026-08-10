#include "ash/memory/raw_ptr.h"
#include <gtest/gtest.h>

namespace ash {
namespace {

class RawPtrTest : public testing::Test {
 protected:
  struct Base {
    virtual ~Base() = default;
    int base_data = 10;
  };

  struct Derived : public Base {
    int derived_data = 20;
  };

  int raw_value = 42;
  int value1 = 100;
  int value2 = 200;
};

// Constructor Tests =====================================================

TEST_F(RawPtrTest, DefaultConstructor) {
  RawPtr<int> ptr;
  EXPECT_EQ(ptr.Get(), nullptr);
  EXPECT_FALSE(ptr);
}

TEST_F(RawPtrTest, PointerConstructor) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_EQ(ptr.Get(), &raw_value);
  EXPECT_TRUE(ptr);  // operator bool() test, expecting true
  EXPECT_EQ(*ptr, 42);
}

TEST_F(RawPtrTest, NullptrConstructor) {
  RawPtr<int> ptr(nullptr);
  EXPECT_EQ(ptr.Get(), nullptr);
  EXPECT_FALSE(ptr);
}

TEST_F(RawPtrTest, CopyConstructor) {
  RawPtr<int> original(&raw_value);
  RawPtr<int> copy(original);
  EXPECT_EQ(copy.Get(), &raw_value);
  EXPECT_EQ(*copy, 42);
}

TEST_F(RawPtrTest, TemplateConstructorFromRawPtr) {
  Derived d;
  RawPtr<Derived> derived_ptr(&d);
  RawPtr<Base> base_ptr(derived_ptr);
  EXPECT_EQ(base_ptr.Get(), &d);
}

TEST_F(RawPtrTest, TemplateConstructorFromPointer) {
  Derived d;
  RawPtr<Base> base_ptr(&d);
  EXPECT_EQ(base_ptr.Get(), &d);
}

// Assignment Operator Tests ===================================================

TEST_F(RawPtrTest, AssignmentFromSameType) {
  RawPtr<int> ptr1(&raw_value);
  RawPtr<int> ptr2;
  ptr2 = ptr1;
  EXPECT_EQ(ptr2.Get(), &raw_value);
  EXPECT_EQ(*ptr2, 42);
}

TEST_F(RawPtrTest, AssignmentDifferentType) {
  struct A {
    int val;
  };
  struct B : public A {};

  B b_obj;
  b_obj.val = 123;
  RawPtr<B> rb(&b_obj);
  RawPtr<A> ra;
  ra = rb;
  EXPECT_EQ(ra.Get(), &b_obj);
  EXPECT_EQ(ra->val, 123);
}

TEST_F(RawPtrTest, AssignmentFromNullptr) {
  RawPtr<int> ptr(&raw_value);
  ptr = nullptr;
  EXPECT_EQ(ptr.Get(), nullptr);
  EXPECT_FALSE(ptr);
}

TEST_F(RawPtrTest, AssignmentFromRawPointer) {
  RawPtr<int> ptr;
  ptr = &raw_value;
  EXPECT_EQ(ptr.Get(), &raw_value);
  EXPECT_EQ(*ptr, 42);
}

TEST_F(RawPtrTest, TemplateAssignmentFromRawPtr) {
  Derived d;
  RawPtr<Derived> derived_ptr(&d);
  RawPtr<Base> base_ptr;
  base_ptr = derived_ptr;
  EXPECT_EQ(base_ptr.Get(), &d);
}

TEST_F(RawPtrTest, TemplateAssignmentFromPointer) {
  Derived d;
  RawPtr<Base> base_ptr;
  base_ptr = &d;
  EXPECT_EQ(base_ptr.Get(), &d);
}

// Dereference Operator Tests =================================================

TEST_F(RawPtrTest, DereferenceOperator) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_EQ(*ptr, 42);
  *ptr = 99;
  EXPECT_EQ(raw_value, 99);
}

TEST_F(RawPtrTest, ConstDereferenceOperator) {
  const RawPtr<int> ptr(&raw_value);
  EXPECT_EQ(*ptr, 42);
}

TEST_F(RawPtrTest, ArrowOperator) {
  Derived d;
  RawPtr<Derived> ptr(&d);
  EXPECT_EQ(ptr->derived_data, 20);
  ptr->derived_data = 30;
  EXPECT_EQ(d.derived_data, 30);
  EXPECT_EQ(ptr->derived_data, 30);
}

TEST_F(RawPtrTest, ConstArrowOperator) {
  Derived d;
  const RawPtr<Derived> ptr(&d);
  EXPECT_EQ(ptr->derived_data, 20);
}

// Boolean Conversion Tests
// =====================================================

TEST_F(RawPtrTest, BoolConversionValid) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_TRUE(ptr);
}

TEST_F(RawPtrTest, BoolConversionNull) {
  RawPtr<int> ptr(nullptr);
  EXPECT_FALSE(ptr);
}

// Comparison Operator Tests (Member Functions)
// =========================================

TEST_F(RawPtrTest, EqualitySameType) {
  RawPtr<int> ptr1(&value1);
  RawPtr<int> ptr2(&value1);
  EXPECT_TRUE(ptr1 == ptr2);

  RawPtr<int> ptr3(&value2);
  EXPECT_FALSE(ptr1 == ptr3);
}

TEST_F(RawPtrTest, EqualityDifferentTypes) {
  Derived d1, d2;
  RawPtr<Derived> derived_ptr(&d1);
  RawPtr<Base> base_ptr(&d1);
  EXPECT_TRUE(derived_ptr == base_ptr);

  base_ptr = &d2;
  EXPECT_FALSE(derived_ptr == base_ptr);
}

TEST_F(RawPtrTest, EqualityWithNullptr) {
  RawPtr<int> ptr;
  EXPECT_TRUE(ptr == nullptr);

  ptr = &raw_value;
  EXPECT_FALSE(ptr == nullptr);
}

TEST_F(RawPtrTest, EqualityWithRawPointer) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_TRUE(ptr == &raw_value);
  EXPECT_FALSE(ptr == &value1);
}

TEST_F(RawPtrTest, InequalitySameType) {
  RawPtr<int> ptr1(&value1);
  RawPtr<int> ptr2(&value2);
  EXPECT_TRUE(ptr1 != ptr2);

  RawPtr<int> ptr3(&value1);
  EXPECT_FALSE(ptr1 != ptr3);
}

TEST_F(RawPtrTest, LessThanSameType) {
  int arr[3] = {10, 20, 30};
  RawPtr<int> rp1(&arr[0]);
  RawPtr<int> rp2(&arr[1]);
  RawPtr<int> rp3(&arr[2]);

  EXPECT_TRUE(rp1 < rp2);
  EXPECT_TRUE(rp2 < rp3);
  EXPECT_TRUE(rp1 < rp3);
}

TEST_F(RawPtrTest, GreaterThanSameType) {
  int arr[3] = {10, 20, 30};
  RawPtr<int> rp1(&arr[0]);
  RawPtr<int> rp2(&arr[1]);
  RawPtr<int> rp3(&arr[2]);

  EXPECT_TRUE(rp3 > rp2);
  EXPECT_TRUE(rp2 > rp1);
  EXPECT_FALSE(rp1 > rp3);
}

TEST_F(RawPtrTest, LessThanOrEqualSameType) {
  int arr[3] = {10, 20, 30};
  RawPtr<int> rp1(&arr[0]);
  RawPtr<int> rp2(&arr[1]);
  RawPtr<int> rp3(&arr[2]);

  EXPECT_TRUE(rp1 <= rp2);
  EXPECT_TRUE(rp2 <= rp2);
}

TEST_F(RawPtrTest, GreaterThanOrEqualSameType) {
  int arr[3] = {10, 20, 30};
  RawPtr<int> rp1(&arr[0]);
  RawPtr<int> rp2(&arr[1]);
  RawPtr<int> rp3(&arr[2]);

  EXPECT_TRUE(rp3 >= rp2);
  EXPECT_TRUE(rp3 >= rp3);
}

// Global Comparison Operator Tests
// ===============================================

TEST_F(RawPtrTest, GlobalEqualityNullptrLeft) {
  RawPtr<int> ptr;
  EXPECT_TRUE(nullptr == ptr);

  ptr = &raw_value;
  EXPECT_FALSE(nullptr == ptr);
}

TEST_F(RawPtrTest, GlobalInequalityNullptrLeft) {
  RawPtr<int> ptr;
  EXPECT_FALSE(nullptr != ptr);

  ptr = &raw_value;
  EXPECT_TRUE(nullptr != ptr);
}

TEST_F(RawPtrTest, GlobalEqualityRawPointerLeft) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_TRUE(&raw_value == ptr);
  EXPECT_FALSE(&value1 == ptr);
}

TEST_F(RawPtrTest, GlobalInequalityRawPointerLeft) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_FALSE(&raw_value != ptr);
  EXPECT_TRUE(&value1 != ptr);
}

// Member Function Tests =====================================================

TEST_F(RawPtrTest, GetMethod) {
  RawPtr<int> ptr(&raw_value);
  EXPECT_EQ(ptr.Get(), &raw_value);
}

TEST_F(RawPtrTest, ResetNoArg) {
  RawPtr<int> ptr(&raw_value);
  ptr.Reset();
  EXPECT_EQ(ptr.Get(), nullptr);
}

TEST_F(RawPtrTest, ResetWithPointer) {
  RawPtr<int> ptr;
  ptr.Reset(&raw_value);
  EXPECT_EQ(ptr.Get(), &raw_value);

  ptr.Reset(&value1);
  EXPECT_EQ(ptr.Get(), &value1);
}

// Edge Case Tests =====================================================

TEST_F(RawPtrTest, SameAddressComparison) {
  RawPtr<int> ptr1(&raw_value);
  RawPtr<int> ptr2(&raw_value);

  EXPECT_TRUE(ptr1 == ptr2);
  EXPECT_FALSE(ptr1 != ptr2);
  EXPECT_FALSE(ptr1 < ptr2);
  EXPECT_FALSE(ptr1 > ptr2);
  EXPECT_TRUE(ptr1 <= ptr2);
  EXPECT_TRUE(ptr1 >= ptr2);
}

TEST_F(RawPtrTest, NullptrComparison) {
  RawPtr<int> null_ptr1;
  RawPtr<int> null_ptr2;

  EXPECT_TRUE(null_ptr1 == null_ptr2);
  EXPECT_FALSE(null_ptr1 != null_ptr2);
}

}  // namespace
}  // namespace ash
