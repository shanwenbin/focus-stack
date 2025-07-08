#include "src/FocusStackSDK.h"
#include <opencv2/opencv.hpp>
#include <chrono>
#include <iostream>
#include <vector>

using namespace FocusStackSDK;

// Global variables to ensure images stay alive during processing
std::vector<cv::Mat> g_images;
std::vector<const cv::Mat*> g_imagePointers;

int main() {
    std::cout << "Focus Stack Performance Comparison: File vs Pointer Interface\n" << std::endl;
    
    // Load test images
    std::vector<std::string> filePaths = {
        "examples/pcb/pcb_001.jpg",
        "examples/pcb/pcb_002.jpg", 
        "examples/pcb/pcb_003.jpg"
    };
    
    // Reserve capacity to avoid reallocation
    g_images.reserve(10);
    g_imagePointers.reserve(10);
    
    std::cout << "Loading test images..." << std::endl;
    for (const auto& path : filePaths) {
        cv::Mat img = cv::imread(path);
        if (img.empty()) {
            std::cerr << "Failed to load: " << path << std::endl;
            return 1;
        }
        g_images.push_back(img);
        g_imagePointers.push_back(&g_images.back());
        std::cout << "  ✓ Loaded " << path << " (" << img.cols << "x" << img.rows << ")" << std::endl;
    }
    
    // Test 1: File-based interface
    std::cout << "\n1. Testing file-based interface..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    {
        auto result = EasyFocusStacker::processWithDefaults(filePaths, "test_file_interface.jpg");
        if (!result) {
            std::cerr << "File-based processing failed!" << std::endl;
            return 1;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto fileTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "  ✓ File-based processing completed in " << fileTime.count() << "ms" << std::endl;
    
    // Test 2: Pointer-based interface
    std::cout << "\n2. Testing pointer-based interface..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    
    {
        auto result = EasyFocusStacker::processWithDefaults(g_imagePointers, "test_pointer_interface.jpg");
        if (!result) {
            std::cerr << "Pointer-based processing failed!" << std::endl;
            return 1;
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto pointerTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "  ✓ Pointer-based processing completed in " << pointerTime.count() << "ms" << std::endl;
    
    // Results
    std::cout << "\n=== Performance Comparison Results ===" << std::endl;
    std::cout << "File-based interface:    " << fileTime.count() << "ms" << std::endl;
    std::cout << "Pointer-based interface: " << pointerTime.count() << "ms" << std::endl;
    
    if (pointerTime < fileTime) {
        double improvement = (double)(fileTime.count() - pointerTime.count()) / fileTime.count() * 100;
        std::cout << "Pointer interface is " << improvement << "% faster!" << std::endl;
    } else {
        double difference = (double)(pointerTime.count() - fileTime.count()) / fileTime.count() * 100;
        std::cout << "File interface is " << difference << "% faster (unexpected)" << std::endl;
    }
    
    std::cout << "\nBenefits of pointer interface:" << std::endl;
    std::cout << "  • Zero-copy image passing (no file I/O overhead)" << std::endl;
    std::cout << "  • Reduced memory allocation" << std::endl;
    std::cout << "  • Better integration with existing image processing pipelines" << std::endl;
    std::cout << "  • Eliminates temporary file creation" << std::endl;
    
    return 0;
}