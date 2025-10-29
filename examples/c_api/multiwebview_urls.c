/**
 * Multi-WebView example with URLs
 * Creates 4 WebViews in a 2x2 grid loading real websites
 * Similar to the Rust multiwebview.rs example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900

typedef struct {
    WryWebView* webview;
    const char* label;
    const char* url;
} WebViewInfo;

typedef struct {
    HWND hwnd;
    WebViewInfo webviews[4];
    int webview_count;
} AppState;

AppState g_app_state = {0};

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
    printf("=== Wry Multi-WebView with URLs ===\n");
    printf("Version: %s\n\n", wry_version());
    
    if (wry_init() != WRY_OK) {
        printf("ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("OK: Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WryMultiWebViewURLClass";
    
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
        0, CLASS_NAME, L"Wry Multi-WebView - URLs",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        printf("ERROR: Failed to create window\n");
        return 1;
    }
    printf("OK: Window created: %p\n", hwnd);
    
    g_app_state.hwnd = hwnd;
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    printf("OK: Window shown\n\n");
    
    // URLs for each WebView (same as Rust example)
    const char* urls[4] = {
        "https://tauri.app",
        "https://github.com/tauri-apps/wry",
        "https://twitter.com/TauriApps",
        "https://google.com"
    };
    
    const char* names[4] = {
        "WebView 1 (Top-Left) - tauri.app",
        "WebView 2 (Top-Right) - github.com/tauri-apps/wry",
        "WebView 3 (Bottom-Left) - twitter.com/TauriApps",
        "WebView 4 (Bottom-Right) - google.com"
    };
    
    int half_width = WINDOW_WIDTH / 2;
    int half_height = WINDOW_HEIGHT / 2;
    
    // Create 4 WebViews
    for (int i = 0; i < 4; i++) {
        printf("Creating %s...\n", names[i]);
        
        WryWebViewBuilder* builder = wry_builder_new();
        if (!builder) {
            printf("ERROR: Failed to create builder\n");
            continue;
        }
        
        // Set URL
        if (wry_builder_with_url(builder, urls[i]) != WRY_OK) {
            printf("ERROR: Failed to set URL: %s\n", wry_last_error_message());
            wry_builder_destroy(builder);
            continue;
        }
        
        // Set bounds using with_bounds
        double x, y;
        if (i == 0) {
            x = 0;
            y = 0;
        } else if (i == 1) {
            x = half_width;
            y = 0;
        } else if (i == 2) {
            x = 0;
            y = half_height;
        } else {
            x = half_width;
            y = half_height;
        }
        
        if (wry_builder_with_bounds(builder, x, y, half_width, half_height) != WRY_OK) {
            printf("ERROR: Failed to set bounds\n");
            wry_builder_destroy(builder);
            continue;
        }
        
        wry_builder_with_devtools(builder, 1);
        wry_builder_with_visible(builder, 1);
        
        WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
        
        if (!webview) {
            printf("ERROR: Failed to build WebView: %s\n", wry_last_error_message());
            wry_builder_destroy(builder);
            continue;
        }
        
        printf("  OK: %s created successfully\n", names[i]);
        
        g_app_state.webviews[i].webview = webview;
        g_app_state.webviews[i].label = names[i];
        g_app_state.webviews[i].url = urls[i];
        g_app_state.webview_count++;
    }
    
    printf("\n=== Summary ===\n");
    printf("Total WebViews created: %d\n", g_app_state.webview_count);
    printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    printf("Each WebView: %dx%d\n\n", half_width, half_height);
    printf("Entering message loop. Close the window to exit.\n");
    
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    printf("\nCleaning up...\n");
    for (int i = 0; i < g_app_state.webview_count; i++) {
        if (g_app_state.webviews[i].webview) {
            printf("Destroying WebView %d: %s\n", i + 1, g_app_state.webviews[i].label);
            wry_webview_destroy(g_app_state.webviews[i].webview);
        }
    }
    
    printf("Done!\n");
    return 0;
}
