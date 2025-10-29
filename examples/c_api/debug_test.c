/**
 * Debug test - writes to file instead of stdout
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "wry.h"

FILE* g_log = NULL;

void log_msg(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(g_log, fmt, args);
    fprintf(g_log, "\n");
    fflush(g_log);
    va_end(args);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            log_msg("WM_CLOSE received");
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            log_msg("WM_DESTROY received");
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

int main() {
    g_log = fopen("debug_test.log", "w");
    if (!g_log) {
        return 1;
    }
    
    log_msg("=== Debug Test Started ===");
    log_msg("Initializing Wry...");
    
    if (wry_init() != WRY_OK) {
        log_msg("ERROR: Failed to initialize Wry");
        fclose(g_log);
        return 1;
    }
    
    log_msg("Wry initialized successfully");
    log_msg("Version: %s", wry_version());
    
    // Register window class
    const wchar_t CLASS_NAME[] = L"DebugWebViewClass";
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClassW(&wc)) {
        log_msg("ERROR: Failed to register window class");
        fclose(g_log);
        return 1;
    }
    
    log_msg("Window class registered");
    
    // Create window
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Debug WebView Test",
        WS_OVERLAPPEDWINDOW,
        100, 100, 600, 600,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        log_msg("ERROR: Failed to create window");
        fclose(g_log);
        return 1;
    }
    
    log_msg("Window created: %p", hwnd);
    
    // Create builder
    log_msg("Creating WebViewBuilder...");
    
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        log_msg("ERROR: Failed to create builder");
        fclose(g_log);
        return 1;
    }
    
    log_msg("Builder created");
    
    // Set HTML
    const char* html = "<html><body style='background: #667eea; color: white; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0;'><h1>Hello WebView!</h1></body></html>";
    
    log_msg("Setting HTML content...");
    
    if (wry_builder_with_html(builder, html) != WRY_OK) {
        log_msg("ERROR: Failed to set HTML: %s", wry_last_error_message());
        wry_builder_destroy(builder);
        fclose(g_log);
        return 1;
    }
    
    log_msg("HTML set successfully");
    
    // Configure builder
    wry_builder_with_visible(builder, 1);
    wry_builder_with_devtools(builder, 1);
    
    log_msg("Building WebView as child...");
    
    // Build WebView
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
    
    if (!webview) {
        log_msg("ERROR: Failed to build WebView: %s", wry_last_error_message());
        wry_builder_destroy(builder);
        fclose(g_log);
        return 1;
    }
    
    log_msg("WebView created successfully!");
    
    // Show window
    log_msg("Showing window...");
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    log_msg("Window is visible. Running message loop...");
    
    // Message loop
    MSG msg = {0};
    int msg_count = 0;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        msg_count++;
        if (msg_count % 100 == 0) {
            log_msg("Processed %d messages", msg_count);
        }
    }
    
    log_msg("Message loop exited after %d messages", msg_count);
    
    // Cleanup
    log_msg("Cleaning up...");
    
    wry_webview_destroy(webview);
    
    log_msg("Done!");
    fclose(g_log);
    
    return 0;
}
