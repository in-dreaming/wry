/**
 * Cookies Example - C API
 * 
 * This example demonstrates cookie management in Wry.
 * It sets cookies, displays them, and deletes them.
 * 
 * Compile: zig cc -o cookies.exe cookies.c -I../../include -L../../target/debug -lwry
 * Run: cookies.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    printf("=== Wry Cookies Example ===\n");
    printf("Version: %s\n\n", wry_version());
    
    if (wry_init() != WRY_OK) {
        printf("ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("OK: Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WryCookiesClass";
    
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
    
    // Create window
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Wry Cookies Example",
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
    
    // HTML content
    const char* html = 
        "<html><head><style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #f5f5f5; }"
        "h1 { color: #333; }"
        ".section { background-color: white; padding: 15px; margin: 10px 0; "
        "border-radius: 5px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }"
        ".info { color: #666; font-size: 14px; }"
        "</style></head><body>"
        "<h1>Wry Cookies Example</h1>"
        "<div class=\"section\">"
        "<h2>Cookie Management</h2>"
        "<p class=\"info\">This example demonstrates setting, reading, and deleting cookies.</p>"
        "<p>Check the console output for cookie operations.</p>"
        "</div>"
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
    
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    
    // Build WebView
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
    if (!webview) {
        printf("ERROR: Failed to build WebView: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    printf("OK: WebView created successfully\n\n");
    
    // Demonstrate cookie operations
    printf("=== Cookie Operations ===\n\n");
    
    // Set a cookie
    printf("Setting cookie: foo1=bar1\n");
    if (wry_webview_set_cookie(webview, "foo1", "bar1", "localhost", "/") != WRY_OK) {
        printf("ERROR: Failed to set cookie: %s\n", wry_last_error_message());
    } else {
        printf("OK: Cookie set\n");
    }
    
    // Set another cookie
    printf("Setting cookie: foo2=bar2\n");
    if (wry_webview_set_cookie(webview, "foo2", "bar2", "localhost", "/") != WRY_OK) {
        printf("ERROR: Failed to set cookie: %s\n", wry_last_error_message());
    } else {
        printf("OK: Cookie set\n");
    }
    
    // Set a cookie to be deleted
    printf("Setting cookie: will_be_deleted=value\n");
    if (wry_webview_set_cookie(webview, "will_be_deleted", "value", "localhost", "/") != WRY_OK) {
        printf("ERROR: Failed to set cookie: %s\n", wry_last_error_message());
    } else {
        printf("OK: Cookie set\n");
    }
    
    printf("\n");
    
    // Get all cookies
    printf("Getting all cookies:\n");
    WryCookie* cookies = NULL;
    size_t cookie_count = 0;
    
    if (wry_webview_cookies(webview, &cookies, &cookie_count) == WRY_OK) {
        printf("Found %zu cookies:\n", cookie_count);
        for (size_t i = 0; i < cookie_count; i++) {
            printf("  [%zu] name=%s, value=%s, domain=%s, path=%s\n",
                   i + 1,
                   cookies[i].name ? cookies[i].name : "(null)",
                   cookies[i].value ? cookies[i].value : "(null)",
                   cookies[i].domain ? cookies[i].domain : "(null)",
                   cookies[i].path ? cookies[i].path : "(null)");
        }
        wry_cookies_free(cookies, cookie_count);
    } else {
        printf("ERROR: Failed to get cookies: %s\n", wry_last_error_message());
    }
    
    printf("\n");
    
    // Delete a cookie
    printf("Deleting cookie: will_be_deleted\n");
    if (wry_webview_delete_cookie(webview, "will_be_deleted") != WRY_OK) {
        printf("ERROR: Failed to delete cookie: %s\n", wry_last_error_message());
    } else {
        printf("OK: Cookie deleted\n");
    }
    
    printf("\n");
    
    // Get cookies again
    printf("Getting cookies after deletion:\n");
    cookies = NULL;
    cookie_count = 0;
    
    if (wry_webview_cookies(webview, &cookies, &cookie_count) == WRY_OK) {
        printf("Found %zu cookies:\n", cookie_count);
        for (size_t i = 0; i < cookie_count; i++) {
            printf("  [%zu] name=%s, value=%s\n",
                   i + 1,
                   cookies[i].name ? cookies[i].name : "(null)",
                   cookies[i].value ? cookies[i].value : "(null)");
        }
        wry_cookies_free(cookies, cookie_count);
    } else {
        printf("ERROR: Failed to get cookies: %s\n", wry_last_error_message());
    }
    
    printf("\n");
    printf("Cookie operations completed.\n");
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
