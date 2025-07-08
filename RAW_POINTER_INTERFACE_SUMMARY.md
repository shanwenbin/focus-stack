# Raw Pointer Interface Implementation Summary

## 概述

成功为FocusStackSDK添加了完整的裸指针接口，实现了库无关的图像处理接口。这些接口使用原始内存指针而不是OpenCV的cv::Mat，使得SDK可以与任何图像处理库集成，无需依赖OpenCV头文件。

## 主要更改

### 1. 头文件更改 (FocusStackSDK.h)

#### 新增RawImageData结构体
```cpp
struct RawImageData {
    void* data;           // 图像数据指针
    int width;            // 图像宽度
    int height;           // 图像高度  
    int channels;         // 通道数 (1=灰度, 3=RGB, 4=RGBA)
    int depth;            // 位深度 (8, 16, 32)
    size_t step;          // 行步长（字节）
    
    // 构造函数和工具方法
    RawImageData();
    RawImageData(void* data, int width, int height, int channels, int depth, size_t step);
    bool isValid() const;
    size_t dataSize() const;
};
```

#### ThreadSafeFocusStacker新增方法

**输入接口：**
- `VoidResult processRawImages(const std::vector<RawImageData>& rawImages, const FocusStackOptions::Config& config)`
- `VoidResult addRawImage(const RawImageData& rawImage)`

**输出接口：**
- `RawImageData getResultImageRaw() const`
- `RawImageData getDepthMapRaw() const`
- `RawImageData get3DPreviewRaw() const`
- `RawImageData getForegroundMaskRaw() const`

#### EasyFocusStacker新增方法
- `static VoidResult processWithDefaults(const std::vector<RawImageData>& rawImages, const std::string& outputPath)`
- `static VoidResult processWithOptions(const std::vector<RawImageData>& rawImages, const FocusStackOptions::Config& config)`

#### 依赖优化
- 移除了 `#include <opencv2/opencv.hpp>`
- 使用前向声明 `namespace cv { class Mat; }`
- 头文件现在完全独立于OpenCV

### 2. 实现文件更改 (FocusStackSDK.cc)

#### 转换工具函数
```cpp
// cv::Mat → RawImageData (零拷贝包装)
RawImageData matToRawImageData(const cv::Mat& mat);

// RawImageData → cv::Mat (零拷贝包装)
cv::Mat rawImageDataToMat(const RawImageData& rawData);
```

#### 完整的方法实现
- 所有新声明的方法都已完整实现
- 使用零拷贝转换，性能优异
- 保持与现有cv::Mat接口的兼容性

## 接口特性

### ✅ 优势

1. **库无关性**
   - 头文件不依赖OpenCV
   - 可与任何图像库集成
   - 跨平台兼容性强

2. **零拷贝设计**
   - 直接内存访问
   - 无数据复制开销
   - 高性能处理

3. **向后兼容**
   - 保留所有原有cv::Mat接口
   - 现有代码无需修改
   - 渐进式迁移支持

4. **易于集成**
   - 简单的C++结构体
   - 标准内存布局
   - 支持多种图像格式

### 📋 支持的图像格式

- **位深度**: 8位、16位、32位
- **通道数**: 1通道(灰度)、3通道(RGB)、4通道(RGBA)
- **数据类型**: 无符号整数、有符号整数、浮点数

## 使用示例

### 基本用法
```cpp
#include "FocusStackSDK.h"

// 创建原始图像数据
std::vector<RawImageData> rawImages;
for (auto& imageData : yourImageData) {
    RawImageData raw(imageData.ptr, width, height, channels, depth, step);
    rawImages.push_back(raw);
}

// 处理图像
ThreadSafeFocusStacker stacker;
FocusStackOptions options;
auto config = options.build();

auto result = stacker.processRawImages(rawImages, *config);
stacker.waitForCompletion();

// 获取结果
RawImageData resultImage = stacker.getResultImageRaw();
if (resultImage.isValid()) {
    // 使用结果数据
    processResult(resultImage.data, resultImage.width, resultImage.height);
}
```

### 简化接口
```cpp
// 一行代码处理
auto result = EasyFocusStacker::processWithDefaults(rawImages, "output.jpg");
```

### 流式处理
```cpp
ThreadSafeFocusStacker stacker;
stacker.startProcessing(*config);

for (const auto& rawImage : rawImages) {
    stacker.addRawImage(rawImage);
}

stacker.finishProcessing();
stacker.waitForCompletion();
```

## 测试验证

创建了完整的测试程序 `raw_pointer_interface_test.cc`，验证了：

- ✅ 批量处理接口
- ✅ 流式处理接口  
- ✅ 输出数据访问
- ✅ EasyFocusStacker接口
- ✅ 数据格式转换
- ✅ 错误处理

## 性能影响

- **零拷贝设计**: 无额外内存开销
- **直接内存访问**: 最优性能
- **兼容现有代码**: 无性能回退

## 迁移指南

### 从cv::Mat迁移到RawImageData

**之前:**
```cpp
std::vector<cv::Mat> images;
stacker.processImages(images, config);
cv::Mat result = stacker.getResultImage();
```

**之后:**
```cpp
std::vector<RawImageData> rawImages;
// 转换现有cv::Mat
for (const auto& mat : images) {
    rawImages.push_back(RawImageData(mat.data, mat.cols, mat.rows, 
                                   mat.channels(), 8, mat.step));
}
stacker.processRawImages(rawImages, config);
RawImageData result = stacker.getResultImageRaw();
```

## 总结

这次实现成功地为FocusStackSDK添加了完整的裸指针接口，实现了以下目标：

1. **✅ 库无关性**: 头文件完全独立于OpenCV
2. **✅ 高性能**: 零拷贝设计，直接内存访问
3. **✅ 易用性**: 简单直观的API设计
4. **✅ 兼容性**: 保持向后兼容，支持渐进迁移
5. **✅ 完整性**: 覆盖输入、输出、批量、流式所有场景

这些改进使得FocusStackSDK可以更容易地集成到各种项目中，无论使用什么图像处理库，都能享受到高性能的焦点堆叠功能。