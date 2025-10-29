/**
 * Full Features Example
 * 
 * This example demonstrates the full set of Wry C API features
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wry.h"

// Callback implementations
void on_ipc(const char* msg, size_t len, void* data) {
    printf("IPC: %.*s\n", (int)len, msg);
}

int on_nav(const char* url, void* data) {
    printf("Navigation: %s\n", url);
    return 1; // Allow
}

// Main application structure
typedef struct {
    WryWebContext* context;
    WryWebViewBuilder* builder;
    int initialized;
} App;

int main() {
    printf("=== Wry C API Full Features Demo ===\n\n");
    printf("Version: %s\n\n", wry_version());
    
    // Initialize
    if (wry_init() != WRY_OK) {
        fprintf(stderr, "Failed to initialize Wry\n");
        return 1;
    }
    
    printf("✓ Initialized Wry\n");
    
    // Create WebContext
    WryWebContext* context = wry_context_new(NULL);
    if (!context) {
        fprintf(stderr, "Failed to create WebContext\n");
        return 1;
    }
    printf("✓ Created WebContext\n");
    
    // Configure context
    wry_context_set_allows_automation(context, 0);
    printf("✓ Configured WebContext\n");
    
    // Create builder
    WryWebViewBuilder* builder = wry_builder_new_with_context(context);
    if (!builder) {
        fprintf(stderr, "Failed to create builder\n");
        wry_context_destroy(context);
        return 1;
    }
    printf("✓ Created WebViewBuilder\n");
    
    // Configure builder
    wry_builder_with_url(builder, "https://www.rust-lang.org");
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    wry_builder_with_transparent(builder, 0);
    wry_builder_with_user_agent(builder, "Wry-C-API/1.0");
    printf("✓ Configured builder (URL, DevTools, Visible, UserAgent)\n");
    
    // Set IPC handler
    wry_builder_with_ipc_handler(builder, on_ipc, NULL);
    printf("✓ Set IPC handler\n");
    
    // Set navigation handler
    wry_builder_with_navigation_handler(builder, on_nav, NULL);
    printf("✓ Set navigation handler\n");
    
    printf("\nBuilder configuration complete!\n");
    printf("\nTo use the webview:\n");
    printf("1. Build it with: wry_builder_build(builder, window_handle, &webview)\n");
    printf("2. Run your event loop\n");
    printf("3. Use webview API functions to interact with it\n");
    printf("4. Clean up with: wry_webview_destroy(webview)\n\n");
    
    // Clean up
    wry_builder_destroy(builder);
    wry_context_destroy(context);
    printf("✓ Cleaned up resources\n");
    
    printf("\nDemo completed successfully!\n");
    return 0;
}


