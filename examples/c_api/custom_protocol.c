/**
 * Custom Protocol Example
 * 
 * This example demonstrates using custom URL protocols with Wry
 * Note: This is a conceptual example showing the API usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wry.h"

// Custom protocol callback
void on_custom_protocol(
    const char* url,
    const char* method,
    const char* body,
    size_t body_len,
    void* response_sender,
    void* userdata
) {
    printf("Custom protocol request: %s %s\n", method, url);
    
    // In a real implementation, you would send a response
    // This requires additional C API functions to be implemented
    // wry_protocol_send_response(response_sender, status, content_type, body, body_len);
}

int main() {
    printf("Wry Custom Protocol Example\n");
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
    
    // Note: Custom protocol support requires additional C API functions
    // wry_builder_with_custom_protocol(builder, "wry", on_custom_protocol, NULL);
    
    // Set URL to custom protocol
    wry_builder_with_url(builder, "wry://localhost");
    wry_builder_with_devtools(builder, 1);
    
    // Clean up
    wry_builder_destroy(builder);
    
    printf("Example completed (requires full custom protocol API implementation)\n");
    return 0;
}


