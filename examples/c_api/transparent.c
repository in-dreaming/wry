/**
 * Transparent WebView Example - C API
 * 
 * This example demonstrates a transparent WebView with semi-transparent background.
 * Matches the Rust version with proper window transparency setup.
 * 
 * Key features:
 * - No window decorations (no border, no title bar)
 * - Transparent background showing desktop
 * - Semi-transparent content box
 * 
 * Compile: zig cc -o transparent.exe transparent.c -I../../include -L../../target/debug -lwry
 * Run: transparent.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

int main() {
    printf("=== Wry Transparent WebView Example ===\n");
    printf("Version: %s\n\n", wry_version());
    
    if (wry_init() != WRY_OK) {
        printf("ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("OK: Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WryTransparentClass";
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    // Transparent background - no brush needed
    wc.hbrBackground = NULL;
    
    if (!RegisterClassW(&wc)) {
        printf("ERROR: Failed to register window class\n");
        return 1;
    }
    printf("OK: Window class registered\n");
    
    // Create window WITHOUT decorations (no border, no title bar)
    // Use WS_POPUP for a borderless window
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,  // Layered window for transparency
        CLASS_NAME, L"Wry Transparent WebView",
        WS_POPUP,  // Borderless window, no title bar
        100, 100,  // Position
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        printf("ERROR: Failed to create window\n");
        return 1;
    }
    printf("OK: Window created (borderless)\n");
    
    // Enable per-pixel alpha blending for transparency
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    printf("OK: Window shown with transparency enabled\n\n");
    
    // HTML content with transparent background
    // The transparency effect comes from:
    // 1. Window: WS_EX_LAYERED + SetLayeredWindowAttributes
    // 2. WebView: wry_builder_with_transparent
    // 3. HTML/CSS: transparent background + rgba colors
    const char* html = 
        "<html><head><style>"
        "html { margin: 0; padding: 0; background: transparent; width: 100vw; height: 100vh; }"
        "body { margin: 0; padding: 20px; background: transparent; "
        "color: white; font-family: Arial, sans-serif; display: flex; "
        "align-items: center; justify-content: center; height: 100vh; "
        "box-sizing: border-box; }"
        ".content { text-align: center; background-color: rgba(87, 87, 87, 0.5); "
        "padding: 30px; border-radius: 10px; }"
        "h1 { margin: 0 0 10px 0; font-size: 28px; }"
        "p { margin: 0; font-size: 16px; }"
        "</style></head><body>"
        "<div class=\"content\">"
        "<h1>Transparent WebView</h1>"
        "<p id=\"userAgent\"></p>"
        "</div>"
        "<script>"
        "window.onload = function() {"
        "document.getElementById('userAgent').innerText = 'User Agent: ' + navigator.userAgent;"
        "};"
        "</script>"
        "</body></html>";
    
    // Create WebView
    printf("Creating WebView...\n");
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        printf("ERROR: Failed to create builder\n");
        return 1;
    }
    
    // Set HTML content
    if (wry_builder_with_html(builder, html) != WRY_OK) {
        printf("ERROR: Failed to set HTML: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    // Enable transparency on the WebView
    // This allows the HTML background to show through
    if (wry_builder_with_transparent(builder, 1) != WRY_OK) {
        printf("ERROR: Failed to set transparency: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    
    // Build WebView
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
    if (!webview) {
        printf("ERROR: Failed to build WebView: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    printf("OK: WebView created successfully\n");
    printf("Transparency enabled\n\n");
    printf("Close the window to exit.\n");
    
    // Message loop
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    printf("\nCleaning up...\n");
    if (webview) {
        wry_webview_destroy(webview);
        printf("OK: WebView destroyed\n");
    }
    
    printf("Done!\n");
    return 0;
}
