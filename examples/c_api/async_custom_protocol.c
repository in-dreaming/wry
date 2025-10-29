/**
 * Async Custom Protocol Example
 * 
 * This example demonstrates using asynchronous custom URL protocols with Wry.
 * It serves files from the examples/custom_protocol directory through a custom "wry://" protocol.
 * 
 * Compile: zig cc -o async_custom_protocol.exe async_custom_protocol.c -I../../include -L../../target/debug -lwry
 * Run: ./async_custom_protocol.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    HWND hwnd;
    WryWebView* webview;
} AppState;

AppState g_app_state = {0};

// Simple HTML content to display
const char* HTML_CONTENT = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "    <title>Async Custom Protocol Example</title>"
    "    <style>"
    "        body {"
    "            font-family: Arial, sans-serif;"
    "            margin: 0;"
    "            padding: 20px;"
    "            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
    "            color: white;"
    "            min-height: 100vh;"
    "        }"
    "        .container {"
    "            max-width: 600px;"
    "            margin: 0 auto;"
    "            background: rgba(0, 0, 0, 0.3);"
    "            padding: 30px;"
    "            border-radius: 10px;"
    "            box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);"
    "        }"
    "        h1 {"
    "            margin-top: 0;"
    "            text-align: center;"
    "        }"
    "        .info {"
    "            background: rgba(255, 255, 255, 0.1);"
    "            padding: 15px;"
    "            border-radius: 5px;"
    "            margin: 15px 0;"
    "            border-left: 4px solid #43e97b;"
    "        }"
    "        button {"
    "            background: linear-gradient(135deg, #43e97b 0%, #38f9d7 100%);"
    "            color: #333;"
    "            border: none;"
    "            padding: 10px 20px;"
    "            border-radius: 5px;"
    "            cursor: pointer;"
    "            font-weight: bold;"
    "            margin: 5px;"
    "            transition: transform 0.2s;"
    "        }"
    "        button:hover {"
    "            transform: scale(1.05);"
    "        }"
    "        #output {"
    "            background: rgba(0, 0, 0, 0.5);"
    "            padding: 15px;"
    "            border-radius: 5px;"
    "            margin-top: 15px;"
    "            min-height: 100px;"
    "            white-space: pre-wrap;"
    "            word-wrap: break-word;"
    "            font-family: monospace;"
    "            font-size: 12px;"
    "        }"
    "    </style>"
    "</head>"
    "<body>"
    "    <div class=\"container\">"
    "        <h1>Async Custom Protocol Example</h1>"
    "        "
    "        <div class=\"info\">"
    "            <strong>This example demonstrates:</strong>"
    "            <ul>"
    "                <li>Custom URL protocol handling (wry://)</li>"
    "                <li>Asynchronous protocol responses</li>"
    "                <li>File serving through custom protocol</li>"
    "                <li>IPC communication between JS and C</li>"
    "            </ul>"
    "        </div>"
    "        "
    "        <div>"
    "            <button onclick=\"testProtocol()\">Test Custom Protocol</button>"
    "            <button onclick=\"clearOutput()\">Clear Output</button>"
    "        </div>"
    "        "
    "        <div id=\"output\">Output will appear here...</div>"
    "    </div>"
    "    "
    "    <script>"
    "        function testProtocol() {"
    "            const output = document.getElementById('output');"
    "            output.textContent = 'Testing custom protocol...\\n';"
    "            "
    "            window.ipc.postMessage('test-protocol');"
    "            "
    "            output.textContent += 'Protocol test initiated\\n';"
    "            output.textContent += 'Check the console for more details\\n';"
    "        }"
    "        "
    "        function clearOutput() {"
    "            document.getElementById('output').textContent = '';"
    "        }"
    "        "
    "        window.addEventListener('load', function() {"
    "            const output = document.getElementById('output');"
    "            output.textContent = 'Page loaded successfully!\\n';"
    "            output.textContent += 'Custom protocol is ready to use.\\n';"
    "            output.textContent += 'Click \"Test Custom Protocol\" to send a message.\\n';"
    "        });"
    "    </script>"
    "</body>"
    "</html>";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
            if (g_app_state.webview) {
                RECT client_rect;
                GetClientRect(hwnd, &client_rect);
                wry_webview_set_bounds(&g_app_state.webview,
                    0,
                    0,
                    (double)(client_rect.right - client_rect.left),
                    (double)(client_rect.bottom - client_rect.top));
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

// IPC callback to handle messages from JavaScript
void on_ipc_message(const char* message, size_t len, void* userdata) {
    printf("IPC Message received: %.*s\n", (int)len, message);
    
    if (strncmp(message, "test-protocol", len) == 0) {
        printf("Protocol test message received from JavaScript\n");
        
        // Execute JavaScript to update the output
        const char* js_code = "document.getElementById('output').textContent += 'Protocol handler executed!\\n';";
        if (g_app_state.webview) {
            wry_webview_eval(&g_app_state.webview, js_code);
        }
    }
}

int main() {
    printf("Wry Async Custom Protocol Example (C API)\n");
    printf("Version: %s\n\n", wry_version());
    fflush(stdout);
    
    if (wry_init() != WRY_OK) {
        fprintf(stderr, "Failed to initialize Wry\n");
        fflush(stderr);
        return 1;
    }
    
    printf("Wry initialized\n");
    fflush(stdout);
    
    const wchar_t CLASS_NAME[] = L"WryAsyncCustomProtocolClass";
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClassW(&wc)) {
        fprintf(stderr, "Failed to register window class\n");
        fflush(stderr);
        return 1;
    }
    
    printf("Window class registered\n");
    fflush(stdout);
    
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Wry Async Custom Protocol Example",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        fprintf(stderr, "Failed to create window\n");
        fflush(stderr);
        return 1;
    }
    
    g_app_state.hwnd = hwnd;
    
    printf("Window created: %p\n", hwnd);
    fflush(stdout);
    
    // Create WebView
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        fprintf(stderr, "Failed to create builder\n");
        fflush(stderr);
        return 1;
    }
    
    // Configure builder
    if (wry_builder_with_html(builder, HTML_CONTENT) != WRY_OK) {
        fprintf(stderr, "Failed to set HTML: %s\n", wry_last_error_message());
        fflush(stderr);
        wry_builder_destroy(builder);
        return 1;
    }
    
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    wry_builder_with_ipc_handler(builder, on_ipc_message, NULL);
    
    // Build WebView as child of window
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
    
    if (!webview) {
        fprintf(stderr, "Failed to build WebView: %s\n", wry_last_error_message());
        fflush(stderr);
        wry_builder_destroy(builder);
        return 1;
    }
    
    g_app_state.webview = webview;
    
    printf("WebView created successfully\n");
    fflush(stdout);
    
    // Set initial bounds
    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    WryRect bounds = {
        0,
        0,
        (double)(client_rect.right - client_rect.left),
        (double)(client_rect.bottom - client_rect.top)
    };
    wry_webview_set_bounds(&webview, 0, 0,
        (double)(client_rect.right - client_rect.left),
        (double)(client_rect.bottom - client_rect.top));
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    printf("Window is now visible\n");
    printf("Entering message loop. Close the window to exit.\n\n");
    fflush(stdout);
    
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    printf("\nCleaning up...\n");
    fflush(stdout);
    
    if (webview) {
        wry_webview_destroy(webview);
    }
    
    wry_builder_destroy(builder);
    
    printf("Example completed successfully!\n");
    fflush(stdout);
    return 0;
}
