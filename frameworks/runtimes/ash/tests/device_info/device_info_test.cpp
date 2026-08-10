#include "ash/device_info/device_info.h"
#include <gtest/gtest.h>

// Simulate NuttX environment
#define ASH_OS_NUTTX
#include "uv_ext.h"
namespace ash {

class DeviceInfoTest : public ::testing::Test {
 protected:
  uv_devinfo_t devinfo;
  void SetUp() override {
    memset(&devinfo, 0, sizeof(devinfo));
    uv_getdeviceinfo(&devinfo);
    // Initialize device information
    DeviceInfo::init();
  }
};

// Test initialization function
TEST_F(DeviceInfoTest, InitLoadsDataCorrectly) {
  EXPECT_EQ(DeviceInfo::brand(), devinfo.brand);
  EXPECT_EQ(DeviceInfo::manufacturer(), devinfo.manufacturer);
  EXPECT_EQ(DeviceInfo::model(), devinfo.model);
}

// Test brand()
TEST_F(DeviceInfoTest, BrandReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::brand(), devinfo.brand);
  EXPECT_NE(DeviceInfo::brand(), "");
}

// Test manufacturer()
TEST_F(DeviceInfoTest, ManufacturerReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::manufacturer(), devinfo.manufacturer);
  EXPECT_NE(DeviceInfo::manufacturer(), "");
}

// Test model()
TEST_F(DeviceInfoTest, ModelReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::model(), devinfo.model);
  EXPECT_NE(DeviceInfo::model(), "");
}

// Test product()
TEST_F(DeviceInfoTest, ProductReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::product(), devinfo.product);
  EXPECT_NE(DeviceInfo::product(), "");
}

// Test osType()
TEST_F(DeviceInfoTest, OsTypeReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::osType(), devinfo.ostype);
  EXPECT_NE(DeviceInfo::osType(), "");
}

// Test osVersionName()
TEST_F(DeviceInfoTest, OsVersionNameReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::osVersionName(), devinfo.osversionname);
  EXPECT_NE(DeviceInfo::osVersionName(), "");
}

// Test language()
TEST_F(DeviceInfoTest, LanguageReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::language(), devinfo.language);
  EXPECT_NE(DeviceInfo::language(), "");
}

// Test region()
TEST_F(DeviceInfoTest, RegionReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::region(), devinfo.region);
  EXPECT_NE(DeviceInfo::region(), "");
}

// Test did()
TEST_F(DeviceInfoTest, DidReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::did(), devinfo.did);
  EXPECT_NE(DeviceInfo::did(), "");
}

// Test screenShape()
TEST_F(DeviceInfoTest, ScreenShapeReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::screenShape(), devinfo.screenshape);
  EXPECT_NE(DeviceInfo::screenShape(), "");
}

// Test deviceType()
TEST_F(DeviceInfoTest, DeviceTypeReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::deviceType(), devinfo.devicetype);
  EXPECT_NE(DeviceInfo::deviceType(), "");
}

// Test osVersionCode()
TEST_F(DeviceInfoTest, OsVersionCodeReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::osVersionCode(), devinfo.osversioncode);
  EXPECT_GT(DeviceInfo::osVersionCode(), 0);
}

// Test screenWidth()
TEST_F(DeviceInfoTest, ScreenWidthReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::screenWidth(), devinfo.screenwidth);
  EXPECT_GT(DeviceInfo::screenWidth(), 0);
}

// Test screenHeight()
TEST_F(DeviceInfoTest, ScreenHeightReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::screenHeight(), devinfo.screenheight);
  EXPECT_GT(DeviceInfo::screenHeight(), 0);
}

// Test screenDensity()
TEST_F(DeviceInfoTest, ScreenDensityReturnsCorrectValue) {
  EXPECT_FLOAT_EQ(DeviceInfo::screenDensity(), devinfo.screendensity);
  EXPECT_GT(DeviceInfo::screenDensity(), 0.0);
}

#ifdef CONFIG_QUICKAPP_TEST_FRAMEWORK
// Test bpp() (only when configuration is enabled)
TEST_F(DeviceInfoTest, BppReturnsCorrectValue) {
  EXPECT_EQ(DeviceInfo::bpp(), devinfo.bpp);
  EXPECT_GT(DeviceInfo::bpp(), 0);
}
#endif

}  // namespace ash
