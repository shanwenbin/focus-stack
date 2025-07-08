#include "src/FocusStackSDK.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>

using namespace FocusStackSDK;

int main() {
    std::cout << "=== Zero-Copy Output Interface Demo ===" << std::endl;
    
    // Load test images
    std::vector<cv::Mat> images;
    std::vector<const cv::Mat*> imagePointers;
    
    // Create test images (simulating focus stack)
    for (int i = 0; i < 3; i++) {
        cv::Mat img = cv::Mat::zeros(400, 400, CV_8UC3);
        
        // Create different focus areas for each image
        cv::Point center(200 + i * 50, 200);
        cv::circle(img, center, 80 - i * 10, cv::Scalar(255, 255, 255), -1);
        cv::circle(img, center, 60 - i * 10, cv::Scalar(100 + i * 50, 150 + i * 30, 200 + i * 25), -1);
        
        images.push_back(img);
        imagePointers.push_back(&images.back());
    }
    
    std::cout << "Created " << images.size() << " test images" << std::endl;
    
    // Configure focus stacking options
    FocusStackOptions options;
    options.setVerbose(true);
    options.setThreadCount(2);
    options.setOutput("zero_copy_result.jpg");  // Required output path
    
    auto config = options.build();
    if (!config) {
        std::cerr << "Failed to build configuration" << std::endl;
        return 1;
    }
    
    // Create focus stacker
    ThreadSafeFocusStacker stacker;
    
    std::cout << "\n=== Processing with Pointer Interface ===" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    // Process images using pointer interface
    auto result = stacker.processImages(imagePointers, *config);
    if (!result) {
        std::cerr << "Processing failed!" << std::endl;
        return 1;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Processing completed in " << duration.count() << "ms" << std::endl;
    
    std::cout << "\n=== Comparing Output Methods ===" << std::endl;
    
    // Method 1: Traditional copy-based access
    std::cout << "\n1. Copy-based access (getResultImage):" << std::endl;
    auto copyStart = std::chrono::high_resolution_clock::now();
    
    auto resultCopy = stacker.getResultImage();
    if (resultCopy) {
        auto copyEnd = std::chrono::high_resolution_clock::now();
        auto copyDuration = std::chrono::duration_cast<std::chrono::microseconds>(copyEnd - copyStart);
        
        std::cout << "   - Result size: " << resultCopy->rows << "x" << resultCopy->cols << std::endl;
        std::cout << "   - Memory usage: " << (resultCopy->total() * resultCopy->elemSize()) / 1024 << " KB" << std::endl;
        std::cout << "   - Copy time: " << copyDuration.count() << " μs" << std::endl;
        
        // Save copy-based result
        cv::imwrite("zero_copy_traditional_output.jpg", *resultCopy);
        std::cout << "   - Saved: zero_copy_traditional_output.jpg" << std::endl;
    }
    
    // Method 2: Zero-copy pointer access
    std::cout << "\n2. Zero-copy pointer access (getResultImagePtr):" << std::endl;
    auto ptrStart = std::chrono::high_resolution_clock::now();
    
    const cv::Mat* resultPtr = stacker.getResultImagePtr();
    if (resultPtr) {
        auto ptrEnd = std::chrono::high_resolution_clock::now();
        auto ptrDuration = std::chrono::duration_cast<std::chrono::microseconds>(ptrEnd - ptrStart);
        
        std::cout << "   - Result size: " << resultPtr->rows << "x" << resultPtr->cols << std::endl;
        std::cout << "   - Memory usage: 0 KB (zero-copy)" << std::endl;
        std::cout << "   - Access time: " << ptrDuration.count() << " μs" << std::endl;
        
        // Save pointer-based result
        cv::imwrite("zero_copy_pointer_output.jpg", *resultPtr);
        std::cout << "   - Saved: zero_copy_pointer_output.jpg" << std::endl;
        
        // Demonstrate direct access to pixel data
        std::cout << "   - Direct pixel access: (" 
                  << (int)resultPtr->at<cv::Vec3b>(200, 200)[0] << ", "
                  << (int)resultPtr->at<cv::Vec3b>(200, 200)[1] << ", "
                  << (int)resultPtr->at<cv::Vec3b>(200, 200)[2] << ")" << std::endl;
    }
    
    // Test all pointer output methods
    std::cout << "\n=== Testing All Pointer Output Methods ===" << std::endl;
    
    const cv::Mat* depthPtr = stacker.getDepthMapPtr();
    if (depthPtr && !depthPtr->empty()) {
        std::cout << "✅ Depth map pointer: " << depthPtr->rows << "x" << depthPtr->cols << std::endl;
        cv::imwrite("zero_copy_depth_output.jpg", *depthPtr);
    } else {
        std::cout << "❌ Depth map not available" << std::endl;
    }
    
    const cv::Mat* previewPtr = stacker.get3DPreviewPtr();
    if (previewPtr && !previewPtr->empty()) {
        std::cout << "✅ 3D preview pointer: " << previewPtr->rows << "x" << previewPtr->cols << std::endl;
        cv::imwrite("zero_copy_3d_output.jpg", *previewPtr);
    } else {
        std::cout << "❌ 3D preview not available" << std::endl;
    }
    
    const cv::Mat* maskPtr = stacker.getForegroundMaskPtr();
    if (maskPtr && !maskPtr->empty()) {
        std::cout << "✅ Foreground mask pointer: " << maskPtr->rows << "x" << maskPtr->cols << std::endl;
        cv::imwrite("zero_copy_mask_output.jpg", *maskPtr);
    } else {
        std::cout << "❌ Foreground mask not available" << std::endl;
    }
    
    std::cout << "\n=== Performance Summary ===" << std::endl;
    std::cout << "✅ Input: Pointer interface (zero-copy)" << std::endl;
    std::cout << "✅ Output: Pointer interface (zero-copy)" << std::endl;
    std::cout << "✅ Processing: " << duration.count() << "ms" << std::endl;
    std::cout << "✅ Memory efficiency: Minimal copying" << std::endl;
    
    std::cout << "\n=== Interface Comparison ===" << std::endl;
    std::cout << "Traditional (copy-based):" << std::endl;
    std::cout << "  - Input: File paths → Load → Copy" << std::endl;
    std::cout << "  - Output: Internal → Copy → Return" << std::endl;
    std::cout << "  - Memory: 2x image data" << std::endl;
    
    std::cout << "\nPointer-based (zero-copy):" << std::endl;
    std::cout << "  - Input: cv::Mat* → Direct access" << std::endl;
    std::cout << "  - Output: Internal → Return pointer" << std::endl;
    std::cout << "  - Memory: 1x image data" << std::endl;
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    return 0;
}