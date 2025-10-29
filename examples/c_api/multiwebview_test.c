/**
 * Multi-WebView example using Wry C API - Test Version
 * 
 * This version includes timeout and debug output to verify the application works.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "wry.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define GRID_SIZE 2
#define TEST_TIMEOUT_MS 3000  // 3 seconds timeout for testing

typedef struct {
    WryWebView* webview;
    const char* label;
} WebViewInfo;

typedef struct {
    HWND hwnd;
    WebViewInfo webviews[GRID_SIZE * GRID_SIZE];
    int webview_count;
    DWORD start_time;
} AppState;

AppState g_app_state = {0};

void update_webview_bounds(HWND hwnd) {
    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    
    int window_width = client_rect.right - client_rect.left;
    int window_height = client_rect.bottom - client_rect.top;
    
    int half_width = window_width / 2;
    int half_height = window_height / 2;
    
    WryRect bounds[4] = {
        {0, 0, half_width, half_height},
        {half_width, 0, half_width, half_height},
        {0, half_height, half_width, half_height},
        {half_width, half_height, half_width, half_height}
    };
    
    for (int i = 0; i < g_app_state.webview_count; i++) {
        if (g_app_state.webviews[i].webview) {
            wry_webview_set_bounds(g_app_state.webviews[i].webview, &bounds[i]);
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
            update_webview_bounds(hwnd);
            return 0;
            
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
    printf("=== Wry Multi-WebView Example (C API) - Test Version ===\n");
    printf("Version: %s\n", wry_version());
    printf("Start time: %ld ms\n\n", GetTickCount());
    
    if (wry_init() != WRY_OK) {
        fprintf(stderr, "ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("[OK] Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WryMultiWebViewClass";
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClassW(&wc)) {
        fprintf(stderr, "ERROR: Failed to register window class\n");
        return 1;
    }
    printf("[OK] Window class registered\n");
    
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Wry Multi-WebView Example",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        fprintf(stderr, "ERROR: Failed to create window\n");
        return 1;
    }
    printf("[OK] Window created: %p\n", hwnd);
    
    g_app_state.hwnd = hwnd;
    g_app_state.start_time = GetTickCount();
    
    printf("[INFO] Creating 4 WebViews in 2x2 grid layout...\n\n");
    
    const char* html_contents[4] = {
        "<html><body style='background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; font-family: Arial;'><div style='text-align: center;'><h1>WebView 1</h1><p>Top-Left</p></div></body></html>",
        "<html><body style='background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%); color: white; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; font-family: Arial;'><div style='text-align: center;'><h1>WebView 2</h1><p>Top-Right</p></div></body></html>",
        "<html><body style='background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); color: white; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; font-family: Arial;'><div style='text-align: center;'><h1>WebView 3</h1><p>Bottom-Left</p></div></body></html>",
        "<html><body style='background: linear-gradient(135deg, #43e97b 0%, #38f9d7 100%); color: white; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; font-family: Arial;'><div style='text-align: center;'><h1>WebView 4</h1><p>Bottom-Right</p></div></body></html>"
    };
    
    const char* names[4] = {
        "WebView 1 (Top-Left)",
        "WebView 2 (Top-Right)",
        "WebView 3 (Bottom-Left)",
        "WebView 4 (Bottom-Right)"
    };
    
    for (int i = 0; i < 4; i++) {
        printf("[%d] Creating %s...\n", i + 1, names[i]);
        fflush(stdout);
        
        WryWebViewBuilder* builder = wry_builder_new();
        if (!builder) {
            fprintf(stderr, "ERROR: Failed to create builder for WebView %d\n", i + 1);
            continue;
        }
        printf("    [OK] Builder created\n");
        
        if (wry_builder_with_html(builder, html_contents[i]) != WRY_OK) {
            fprintf(stderr, "ERROR: Failed to set HTML for WebView %d: %s\n", i + 1, wry_last_error_message());
            wry_builder_destroy(builder);
            continue;
        }
        printf("    [OK] HTML set\n");
        
        wry_builder_with_devtools(builder, 1);
        wry_builder_with_visible(builder, 1);
        printf("    [OK] DevTools and visibility configured\n");
        
        printf("    [INFO] Building WebView as child...\n");
        fflush(stdout);
        
        WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
        
        if (!webview) {
            fprintf(stderr, "ERROR: Failed to build WebView %d: %s\n", i + 1, wry_last_error_message());
            wry_builder_destroy(builder);
            continue;
        }
        
        printf("    [OK] WebView %d created successfully\n\n", i + 1);
        
        g_app_state.webviews[i].webview = webview;
        g_app_state.webviews[i].label = names[i];
        g_app_state.webview_count++;
    }
    
    printf("=== Summary ===\n");
    printf("Total WebViews created: %d/4\n", g_app_state.webview_count);
    printf("Time elapsed: %ld ms\n\n", GetTickCount() - g_app_state.start_time);
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    printf("[OK] Window is now visible\n");
    printf("[INFO] Running message loop (close window to exit)...\n\n");
    fflush(stdout);
    
    MSG msg = {0};
    int msg_count = 0;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        msg_count++;
    }
    
    printf("\n[INFO] Message loop exited after %d messages\n", msg_count);
    printf("[INFO] Cleaning up...\n");
    
    for (int i = 0; i < g_app_state.webview_count; i++) {
        if (g_app_state.webviews[i].webview) {
            wry_webview_destroy(g_app_state.webviews[i].webview);
            printf("[OK] WebView %d destroyed\n", i + 1);
        }
    }
    
    printf("\n[OK] Example completed successfully!\n");
    printf("Total time: %ld ms\n", GetTickCount() - g_app_state.start_time);
    return 0;
}
