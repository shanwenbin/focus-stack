// Test compilation of header without OpenCV dependency
#include "src/FocusStackSDK.h"
#include <iostream>
#include <vector>

int main() {
    // Test that we can use raw pointer interfaces without OpenCV
    std::vector<FocusStackSDK::RawImageData> rawImages;
    
    // Create a dummy raw image
    FocusStackSDK::RawImageData rawImage;
    rawImage.data = nullptr;
    rawImage.width = 640;
    rawImage.height = 480;
    rawImage.channels = 3;
    rawImage.depth = 8;
    rawImage.step = 640 * 3;
    
    rawImages.push_back(rawImage);
    
    // Test EasyFocusStacker with raw images
    auto result = FocusStackSDK::EasyFocusStacker::processWithDefaults(rawImages, "output.jpg");
    
    std::cout << "Header compilation test passed!" << std::endl;
    return 0;
}