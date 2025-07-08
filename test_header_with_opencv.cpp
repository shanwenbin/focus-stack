// Test compilation of header with OpenCV dependency
#define FOCUSSTACK_INCLUDE_OPENCV
#include <opencv2/opencv.hpp>
#include "src/FocusStackSDK.h"
#include <iostream>
#include <vector>

int main() {
    // Test that we can use both raw pointer and cv::Mat interfaces
    std::vector<FocusStackSDK::RawImageData> rawImages;
    std::vector<const cv::Mat*> matPointers;
    
    // Create a dummy cv::Mat
    cv::Mat testMat(480, 640, CV_8UC3);
    matPointers.push_back(&testMat);
    
    // Test EasyFocusStacker with cv::Mat pointers
    auto result1 = FocusStackSDK::EasyFocusStacker::processWithDefaults(matPointers, "output1.jpg");
    
    // Test EasyFocusStacker with raw images
    FocusStackSDK::RawImageData rawImage;
    rawImage.data = nullptr;
    rawImage.width = 640;
    rawImage.height = 480;
    rawImage.channels = 3;
    rawImage.depth = 8;
    rawImage.step = 640 * 3;
    rawImages.push_back(rawImage);
    
    auto result2 = FocusStackSDK::EasyFocusStacker::processWithDefaults(rawImages, "output2.jpg");
    
    std::cout << "Header with OpenCV compilation test passed!" << std::endl;
    return 0;
}