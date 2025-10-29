/**
 * Simple example using Wry C API
 * 
 * This example demonstrates basic WebView creation and usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wry.h"

// IPC callback
void on_ipc_message(const char* message, size_t len, void* userdata) {
    (void)userdata;
    printf("IPC message received: %.*s\n", (int)len, message);
}

// Navigation callback (must return int per header)
int on_navigation(const char* url, void* userdata) {
    (void)userdata;
    printf("Navigating to: %s\n", url);
    return 1; // non-zero to allow navigation
}

int main() {
    printf("Wry C API Example\n");
    printf("Version: %s\n", wry_version());
    
    // Initialize (required on Linux)
    if (wry_init() != WRY_OK) {
        fprintf(stderr, "Failed to initialize Wry\n");
        return 1;
    }
    
    // Create a WebView builder
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        fprintf(stderr, "Failed to create builder\n");
        return 1;
    }
    
    // Configure builder
    wry_builder_with_url(builder, "https://www.rust-lang.org");
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    
    // Set IPC handler
    wry_builder_with_ipc_handler(builder, on_ipc_message, NULL);
    
    // Set navigation handler
    wry_builder_with_navigation_handler(builder, on_navigation, NULL);
    
    // Note: In a real application, you would need to provide a window handle
    // For this example, we'll just demonstrate the API
    
    // Clean up
    wry_builder_destroy(builder);
    
    printf("Example completed successfully\n");
    return 0;
}


