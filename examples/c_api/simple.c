/**
 * Simple WebView Example - C API
 * 
 * This is the simplest example showing how to create a WebView
 * that loads a URL from the internet.
 * 
 * Compile: zig cc -o simple.exe simple.c -I../../include -L../../target/debug -lwry
 * Run: simple.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
    printf("=== Wry Simple WebView Example ===\n");
    printf("Version: %s\n\n", wry_version());
    
    if (wry_init() != WRY_OK) {
        printf("ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("OK: Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WrySimpleClass";
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClassW(&wc)) {
        printf("ERROR: Failed to register window class\n");
        return 1;
    }
    printf("OK: Window class registered\n");
    
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Wry Simple WebView",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        printf("ERROR: Failed to create window\n");
        return 1;
    }
    printf("OK: Window created\n");
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    printf("OK: Window shown\n\n");
    
    // Create WebView
    printf("Creating WebView...\n");
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        printf("ERROR: Failed to create builder\n");
        return 1;
    }
    
    // Set URL to load
    if (wry_builder_with_url(builder, "https://tauri.app") != WRY_OK) {
        printf("ERROR: Failed to set URL: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    // Enable developer tools
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    
    // Build WebView as child of window
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
    if (!webview) {
        printf("ERROR: Failed to build WebView: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    printf("OK: WebView created successfully\n");
    printf("Loading: https://tauri.app\n\n");
    printf("Window is ready. Close the window to exit.\n");
    
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
