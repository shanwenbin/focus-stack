#include "src/FocusStackSDK.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace FocusStackSDK;

int main() {
    std::cout << "Simple Pointer Interface Test\n" << std::endl;
    
    // Load test images - reserve capacity to avoid reallocation
    std::vector<cv::Mat> images;
    std::vector<const cv::Mat*> imagePointers;
    images.reserve(10);
    imagePointers.reserve(10);
    
    std::vector<std::string> filePaths = {
        "examples/pcb/pcb_001.jpg",
        "examples/pcb/pcb_002.jpg", 
        "examples/pcb/pcb_003.jpg"
    };
    
    std::cout << "Loading test images..." << std::endl;
    for (const auto& path : filePaths) {
        cv::Mat img = cv::imread(path);
        if (img.empty()) {
            std::cerr << "Failed to load: " << path << std::endl;
            return 1;
        }
        images.push_back(img);
        imagePointers.push_back(&images.back());
        std::cout << "  ✓ Loaded " << path << " (" << img.cols << "x" << img.rows << ")" << std::endl;
    }
    
    // Validate pointers before processing
    std::cout << "\nValidating image pointers..." << std::endl;
    for (size_t i = 0; i < imagePointers.size(); ++i) {
        const cv::Mat* ptr = imagePointers[i];
        if (!ptr) {
            std::cerr << "✗ Null pointer at index " << i << std::endl;
            return 1;
        }
        if (ptr->empty()) {
            std::cerr << "✗ Empty image at index " << i << std::endl;
            return 1;
        }
        std::cout << "  ✓ Image " << i << " pointer valid (" << ptr->cols << "x" << ptr->rows << ")" << std::endl;
    }
    
    std::cout << "\nTesting pointer interface..." << std::endl;
    auto result = EasyFocusStacker::processWithDefaults(imagePointers, "simple_test_output.jpg");
    if (result) {
        std::cout << "✓ Pointer-based processing completed successfully!" << std::endl;
    } else {
        std::cerr << "✗ Pointer-based processing failed: " << static_cast<int>(result.error()) << std::endl;
        return 1;
    }
    
    return 0;
}