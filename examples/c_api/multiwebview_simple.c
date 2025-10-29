/**
 * Multi-WebView example using Wry C API - Simple approach
 * 
 * This example creates all WebViews sequentially in the main thread,
 * but with proper message loop handling.
 * 
 * Compile: zig cc -o multiwebview_simple.exe multiwebview_simple.c -I../../include -L../../target/debug -lwry
 * Run: ./multiwebview_simple.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define GRID_SIZE 2

typedef struct {
    WryWebView* webview;
    const char* label;
} WebViewInfo;

typedef struct {
    HWND hwnd;
    WebViewInfo webviews[GRID_SIZE * GRID_SIZE];
    int webview_count;
    int creation_index;
    int creation_complete;
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

void create_next_webview() {
    if (g_app_state.creation_index >= 4) {
        g_app_state.creation_complete = 1;
        printf("All WebViews created!\\n");
        fflush(stdout);
        return;
    }
    
    int i = g_app_state.creation_index;
    
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
    
    printf("Creating %s...\\n", names[i]);
    fflush(stdout);
    
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        fprintf(stderr, "Failed to create builder for WebView %d\\n", i + 1);
        fflush(stderr);
        g_app_state.creation_index++;
        return;
    }
    
    if (wry_builder_with_html(builder, html_contents[i]) != WRY_OK) {
        fprintf(stderr, "Failed to set HTML for WebView %d: %s\\n", i + 1, wry_last_error_message());
        fflush(stderr);
        wry_builder_destroy(builder);
        g_app_state.creation_index++;
        return;
    }
    
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)g_app_state.hwnd);
    
    if (!webview) {
        fprintf(stderr, "Failed to build WebView %d: %s\\n", i + 1, wry_last_error_message());
        fflush(stderr);
        wry_builder_destroy(builder);
        g_app_state.creation_index++;
        return;
    }
    
    printf("  OK %s created successfully\\n", names[i]);
    fflush(stdout);
    
    g_app_state.webviews[i].webview = webview;
    g_app_state.webviews[i].label = names[i];
    g_app_state.webview_count++;
    
    g_app_state.creation_index++;
    
    // Update bounds for the newly created WebView
    update_webview_bounds(g_app_state.hwnd);
    
    // Post message to create the next WebView
    PostMessageW(g_app_state.hwnd, WM_USER + 1, 0, 0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_USER + 1:
            // Create next WebView
            create_next_webview();
            return 0;
            
        case WM_SIZE:
            if (g_app_state.creation_complete) {
                update_webview_bounds(hwnd);
            }
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
    printf("Wry Multi-WebView Example (C API - Simple)\\n");
    printf("Version: %s\\n\\n", wry_version());
    fflush(stdout);
    
    if (wry_init() != WRY_OK) {
        fprintf(stderr, "Failed to initialize Wry\\n");
        fflush(stderr);
        return 1;
    }
    
    printf("Wry initialized\\n");
    fflush(stdout);
    
    const wchar_t CLASS_NAME[] = L"WryMultiWebViewClass";
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClassW(&wc)) {
        fprintf(stderr, "Failed to register window class\\n");
        fflush(stderr);
        return 1;
    }
    
    printf("Window class registered\\n");
    fflush(stdout);
    
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
        fprintf(stderr, "Failed to create window\\n");
        fflush(stderr);
        return 1;
    }
    
    g_app_state.hwnd = hwnd;
    
    printf("Window created: %p\\n", hwnd);
    fflush(stdout);
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    printf("Window is now visible\\n");
    printf("Creating WebViews...\\n\\n");
    fflush(stdout);
    
    // Start creating WebViews
    PostMessageW(hwnd, WM_USER + 1, 0, 0);
    
    printf("Entering message loop. Close the window to exit.\\n\\n");
    fflush(stdout);
    
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    printf("\\nCleaning up...\\n");
    fflush(stdout);
    
    for (int i = 0; i < g_app_state.webview_count; i++) {
        if (g_app_state.webviews[i].webview) {
            printf("Destroying WebView %d\\n", i + 1);
            fflush(stdout);
            wry_webview_destroy(g_app_state.webviews[i].webview);
        }
    }
    
    printf("Example completed successfully!\\n");
    fflush(stdout);
    return 0;
}
