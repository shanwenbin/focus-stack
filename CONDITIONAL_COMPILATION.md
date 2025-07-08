# Conditional Compilation Support

The FocusStackSDK now supports conditional compilation to allow usage without OpenCV dependency in the header file.

## Usage Modes

### 1. Library-Independent Mode (Default)

By default, only raw pointer interfaces are available. This mode doesn't require OpenCV headers in your project.

```cpp
#include "FocusStackSDK.h"
#include <vector>

// Only raw pointer methods are available
std::vector<FocusStackSDK::RawImageData> rawImages;
// ... populate rawImages ...

auto result = FocusStackSDK::EasyFocusStacker::processWithDefaults(rawImages, "output.jpg");
```

**Compilation:**
```bash
g++ -I./src your_code.cpp -lfocusstack
```

### 2. OpenCV-Compatible Mode

Define `FOCUSSTACK_INCLUDE_OPENCV` before including the header to enable cv::Mat interfaces.

```cpp
#define FOCUSSTACK_INCLUDE_OPENCV
#include <opencv2/opencv.hpp>
#include "FocusStackSDK.h"
#include <vector>

// Both raw pointer and cv::Mat methods are available
std::vector<const cv::Mat*> matPointers;
std::vector<FocusStackSDK::RawImageData> rawImages;

// cv::Mat interface
auto result1 = FocusStackSDK::EasyFocusStacker::processWithDefaults(matPointers, "output1.jpg");

// Raw pointer interface (still available)
auto result2 = FocusStackSDK::EasyFocusStacker::processWithDefaults(rawImages, "output2.jpg");
```

**Compilation:**
```bash
g++ -I./src -DFOCUSSTACK_INCLUDE_OPENCV your_code.cpp -lfocusstack `pkg-config --cflags --libs opencv4`
```

## Available Interfaces

### Always Available (Library-Independent)

- `RawImageData` struct for image data representation
- `processRawImages()` - batch processing with raw pointers
- `addRawImage()` - streaming interface with raw pointers
- `getResultImageRaw()`, `getDepthMapRaw()`, etc. - raw pointer output methods
- `EasyFocusStacker::processWithDefaults(const std::vector<RawImageData>&, const std::string&)`

### Available Only with FOCUSSTACK_INCLUDE_OPENCV

- `processImages(const std::vector<const cv::Mat*>&, ...)` - batch processing with cv::Mat pointers
- `addImage(const cv::Mat&)` and `addImage(const cv::Mat*)` - streaming interface with cv::Mat
- `getResultImage()`, `getDepthMap()`, etc. - cv::Mat output methods
- `getResultImagePtr()`, `getDepthMapPtr()`, etc. - cv::Mat pointer output methods
- `EasyFocusStacker::processWithDefaults(const std::vector<const cv::Mat*>&, const std::string&)`

## RawImageData Structure

```cpp
struct RawImageData {
    void* data;        // Pointer to image data
    int width;         // Image width in pixels
    int height;        // Image height in pixels
    int channels;      // Number of channels (1=grayscale, 3=RGB, 4=RGBA)
    int depth;         // Bit depth per channel (8, 16, 32)
    size_t step;       // Bytes per row (width * channels * (depth/8))
    
    bool isValid() const;  // Check if data is valid
};
```

## Migration Guide

### From cv::Mat to Raw Pointers

**Before:**
```cpp
cv::Mat image = cv::imread("input.jpg");
std::vector<const cv::Mat*> images = {&image};
auto result = stacker.processImages(images, config);
cv::Mat output = *stacker.getResultImage();
```

**After:**
```cpp
cv::Mat image = cv::imread("input.jpg");
FocusStackSDK::RawImageData rawImage;
rawImage.data = image.data;
rawImage.width = image.cols;
rawImage.height = image.rows;
rawImage.channels = image.channels();
rawImage.depth = image.depth() == CV_8U ? 8 : (image.depth() == CV_16U ? 16 : 32);
rawImage.step = image.step;

std::vector<FocusStackSDK::RawImageData> rawImages = {rawImage};
auto result = stacker.processRawImages(rawImages, config);
FocusStackSDK::RawImageData outputRaw = stacker.getResultImageRaw();

// Convert back to cv::Mat if needed
cv::Mat output(outputRaw.height, outputRaw.width, 
               CV_MAKETYPE(outputRaw.depth == 8 ? CV_8U : CV_16U, outputRaw.channels),
               outputRaw.data, outputRaw.step);
```

## Benefits

1. **Reduced Dependencies**: Projects that don't need cv::Mat can avoid OpenCV headers
2. **Faster Compilation**: Smaller header dependency tree
3. **Cross-Platform Compatibility**: Easier to use in environments where OpenCV headers are problematic
4. **Backward Compatibility**: Existing cv::Mat code continues to work when FOCUSSTACK_INCLUDE_OPENCV is defined
5. **Flexibility**: Choose the interface that best fits your project's needs