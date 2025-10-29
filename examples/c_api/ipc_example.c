/**
 * IPC Example
 * 
 * This example demonstrates IPC (Inter-Process Communication) 
 * between JavaScript and C code using Wry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wry.h"

// IPC callback function
void on_ipc_message(const char* message, size_t len, void* userdata) {
    printf("IPC Message received (len=%zu): %.*s\n", len, (int)len, message);
    
    // Echo the message back (in a real app, you would do something useful)
    if (userdata != NULL) {
        printf("User data context available\n");
    }
}

// Navigation callback
int on_navigation(const char* url, void* userdata) {
    printf("Navigating to: %s\n", url);
    // Return 1 to allow navigation, 0 to block
    return 1;
}

int main() {
    printf("Wry IPC Example\n");
    printf("Version: %s\n", wry_version());
    
    // Initialize
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
    wry_builder_with_url(builder, "data:text/html,<!DOCTYPE html><html><body><h1>IPC Example</h1><p>Open console and type: window.webkit.messageHandlers.ipc.postMessage('Hello from JS!')</p></body></html>");
    wry_builder_with_devtools(builder, 1);
    
    // Set IPC handler
    wry_builder_with_ipc_handler(builder, on_ipc_message, NULL);
    
    // Set navigation handler
    wry_builder_with_navigation_handler(builder, on_navigation, NULL);
    
    printf("Builder configured with IPC handler\n");
    printf("In a real application, you would:\n");
    printf("1. Build the webview with a window handle\n");
    printf("2. Run an event loop\n");
    printf("3. Handle messages from JavaScript\n\n");
    
    // Clean up
    wry_builder_destroy(builder);
    
    printf("Example completed successfully\n");
    return 0;
}


