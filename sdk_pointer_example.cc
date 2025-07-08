#include "src/FocusStackSDK.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace FocusStackSDK;

int main() {
    std::cout << "Focus Stack SDK Pointer Interface Example" << std::endl;
    
    // Load images into memory first
    std::vector<std::string> imagePaths = {
        "examples/pcb/pcb_001.jpg", 
        "examples/pcb/pcb_002.jpg", 
        "examples/pcb/pcb_003.jpg"
    };
    
    std::vector<cv::Mat> images;
    std::vector<const cv::Mat*> imagePointers;
    
    // Reserve capacity to prevent reallocation and pointer invalidation
    images.reserve(imagePaths.size());
    imagePointers.reserve(imagePaths.size());
    
    std::cout << "\n1. Loading images into memory..." << std::endl;
    for (const auto& path : imagePaths) {
        cv::Mat img = cv::imread(path, cv::IMREAD_ANYCOLOR);
        if (img.empty()) {
            std::cerr << "✗ Failed to load image: " << path << std::endl;
            std::cerr << "Note: This example requires the PCB sample images." << std::endl;
            std::cerr << "You can create dummy images for testing instead." << std::endl;
            
            // Create a dummy image for demonstration
            img = cv::Mat::zeros(480, 640, CV_8UC3);
            cv::putText(img, "Dummy Image " + std::to_string(images.size() + 1), 
                       cv::Point(50, 240), cv::FONT_HERSHEY_SIMPLEX, 1, 
                       cv::Scalar(255, 255, 255), 2);
        }
        
        images.push_back(img);
        imagePointers.push_back(&images.back());
        std::cout << "  ✓ Loaded image " << (images.size()) << " (" << img.cols << "x" << img.rows << ")" << std::endl;
    }
    
    // Example 1: Simple processing with pointers using defaults
    std::cout << "\n2. Simple processing with pointer interface..." << std::endl;
    auto result = EasyFocusStacker::processWithDefaults(imagePointers, "sdk_pointer_output.jpg");
    if (result) {
        std::cout << "✓ Pointer-based processing completed successfully!" << std::endl;
    } else {
        std::cerr << "✗ Pointer-based processing failed: " << static_cast<int>(result.error()) << std::endl;
        return 1;
    }
    
    // Example 2: Advanced processing with custom options using pointers
    std::cout << "\n3. Advanced processing with pointer interface..." << std::endl;
    FocusStackOptions options;
    auto outputResult = options.setOutput("sdk_pointer_advanced_output.jpg");
    auto qualityResult = options.setJpgQuality(90);
    auto verboseResult = options.setVerbose(false);  // Quiet mode for cleaner output
    
    if (!outputResult || !qualityResult || !verboseResult) {
        std::cerr << "Failed to configure options" << std::endl;
        return 1;
    }
    
    auto config = options.build();
    if (!config) {
        std::cerr << "Failed to build config: " << static_cast<int>(config.error()) << std::endl;
        return 1;
    }
    
    ThreadSafeFocusStacker stacker;
    auto processResult = stacker.processImages(imagePointers, *config);
    if (processResult) {
        std::cout << "✓ Advanced pointer-based processing completed!" << std::endl;
        
        // Get processing status
        auto status = stacker.getStatus();
        std::cout << "  Final status: " << status.progressPercentage << "% complete" << std::endl;
        
        // Get result image
        auto resultImage = stacker.getResultImage();
        if (resultImage) {
            std::cout << "  Result image size: " << resultImage->rows << "x" << resultImage->cols << std::endl;
        }
        
        // Save result with different name
        auto saveResult = stacker.saveResult("sdk_pointer_saved_result.jpg");
        if (saveResult) {
            std::cout << "  ✓ Result saved successfully" << std::endl;
        }
    } else {
        std::cerr << "✗ Advanced pointer-based processing failed: " << static_cast<int>(processResult.error()) << std::endl;
        return 1;
    }
    
    // Example 3: Streaming interface with pointers
    std::cout << "\n4. Streaming interface with pointers..." << std::endl;
    ThreadSafeFocusStacker streamStacker;
    
    FocusStackOptions streamOptions;
    streamOptions.setOutput("sdk_pointer_stream_output.jpg");
    auto streamConfig = streamOptions.build();
    if (!streamConfig) {
        std::cerr << "Failed to build stream config" << std::endl;
        return 1;
    }
    
    auto startResult = streamStacker.startProcessing(*streamConfig);
    if (!startResult) {
        std::cerr << "Failed to start streaming processing" << std::endl;
        return 1;
    }
    
    // Add images one by one using pointers
    for (size_t i = 0; i < imagePointers.size(); ++i) {
        auto addResult = streamStacker.addImage(imagePointers[i]);
        if (addResult) {
            std::cout << "  ✓ Added image " << (i + 1) << " via pointer" << std::endl;
        } else {
            std::cerr << "  ✗ Failed to add image " << (i + 1) << std::endl;
            return 1;
        }
    }
    
    auto finishResult = streamStacker.finishProcessing();
    if (finishResult) {
        std::cout << "✓ Streaming processing with pointers completed!" << std::endl;
    } else {
        std::cerr << "✗ Streaming processing failed" << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ All pointer interface examples completed successfully!" << std::endl;
    std::cout << "\nPerformance Benefits of Pointer Interface:" << std::endl;
    std::cout << "  • Zero-copy image passing (no file I/O)" << std::endl;
    std::cout << "  • Reduced memory allocation overhead" << std::endl;
    std::cout << "  • Faster processing for in-memory images" << std::endl;
    std::cout << "  • Better integration with image processing pipelines" << std::endl;
    
    return 0;
}