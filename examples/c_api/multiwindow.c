/**
 * Multi-Window Example with Debug Logging
 * 
 * This example demonstrates creating and managing multiple windows with WebViews.
 * Based on the Rust multiwindow.rs example, using a simpler approach with
 * direct window management instead of complex IPC.
 * 
 * Compile: zig cc -o multiwindow.exe multiwindow.c -I../../include -L../../target/debug -lwry
 * Run: ./multiwindow.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define MAX_WINDOWS 10
#define WM_CREATE_WEBVIEW (WM_USER + 1)
#define WM_NEW_WINDOW (WM_USER + 2)

typedef struct {
    HWND hwnd;
    WryWebView* webview;
    int window_number;
    int is_active;
    int webview_created;
} WindowInfo;

typedef struct {
    WindowInfo windows[MAX_WINDOWS];
    int window_count;
    HWND main_hwnd;
} AppState;

AppState g_app_state = {0};

// Forward declarations
WindowInfo* create_new_window(int window_number);

// Color gradients for different windows
const char* GRADIENTS[][2] = {
    {"#667eea", "#764ba2"},  // Purple
    {"#f093fb", "#f5576c"},  // Pink
    {"#4facfe", "#00f2fe"},  // Cyan
    {"#43e97b", "#38f9d7"},  // Green
    {"#fa709a", "#fee140"},  // Orange
    {"#30cfd0", "#330867"},  // Teal
    {"#a8edea", "#fed6e3"},  // Light
    {"#ff9a56", "#ff6a88"},  // Red
    {"#2e2e78", "#662d91"},  // Dark Purple
    {"#0093e9", "#80d0c7"},  // Blue
};

// IPC callback for each window
void on_ipc_message(const char* message, size_t len, void* userdata) {
    WindowInfo* win_info = (WindowInfo*)userdata;
    if (!win_info) {
        fprintf(stderr, "[ERROR] on_ipc_message: win_info is NULL\n");
        fflush(stderr);
        return;
    }
    
    printf("[IPC] Window %d received message (%zu bytes): %.*s\n", win_info->window_number, len, (int)len, message);
    fflush(stdout);
    
    if (strncmp(message, "new-window", len) == 0) {
        printf("[IPC] Creating new window from window %d\n", win_info->window_number);
        fflush(stdout);
        if (g_app_state.main_hwnd) {
            printf("[IPC] Posting WM_NEW_WINDOW to main window %p\n", g_app_state.main_hwnd);
            fflush(stdout);
            PostMessageW(g_app_state.main_hwnd, WM_NEW_WINDOW, 0, 0);
        } else {
            fprintf(stderr, "[ERROR] main_hwnd is NULL\n");
            fflush(stderr);
        }
    } else if (strncmp(message, "close", len) == 0) {
        printf("[IPC] Closing window %d\n", win_info->window_number);
        fflush(stdout);
        PostMessageW(win_info->hwnd, WM_CLOSE, 0, 0);
    } else if (strncmp(message, "change-title:", 13) == 0) {
        const char* title_start = message + 13;
        size_t title_len = len - 13;
        printf("[IPC] Changing title for window %d to: %.*s\n", win_info->window_number, (int)title_len, title_start);
        fflush(stdout);
        
        wchar_t wide_title[256];
        int wide_len = MultiByteToWideChar(CP_UTF8, 0, title_start, (int)title_len, wide_title, 255);
        wide_title[wide_len] = L'\0';
        SetWindowTextW(win_info->hwnd, wide_title);
    } else {
        printf("[IPC] Unknown message type\n");
        fflush(stdout);
    }
}

// Create WebView for a window
void create_webview_for_window(WindowInfo* win_info) {
    if (!win_info) {
        fprintf(stderr, "[ERROR] create_webview_for_window: win_info is NULL\n");
        fflush(stderr);
        return;
    }
    
    if (win_info->webview_created) {
        printf("[DEBUG] WebView for window %d already created\n", win_info->window_number);
        fflush(stdout);
        return;
    }
    
    printf("[WEBVIEW] Creating WebView for window %d (hwnd=%p)\n", win_info->window_number, win_info->hwnd);
    fflush(stdout);
    
    printf("[WEBVIEW] Creating builder...\n");
    fflush(stdout);
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        fprintf(stderr, "[ERROR] Failed to create builder for window %d: %s\n", win_info->window_number, wry_last_error_message());
        fflush(stderr);
        return;
    }
    printf("[WEBVIEW] Builder created successfully\n");
    fflush(stdout);
    
    // Generate HTML content for this window
    int gradient_idx = win_info->window_number % 10;
    char html_content[2048];
    snprintf(html_content, sizeof(html_content),
        "<html><head><style>"
        "body { font-family: Arial; margin: 0; padding: 20px; "
        "background: linear-gradient(135deg, %s 0%%, %s 100%%);"
        "color: white; min-height: 100vh; display: flex; "
        "flex-direction: column; justify-content: center; align-items: center; }"
        
        ".container { background: rgba(0,0,0,0.3); padding: 30px; "
        "border-radius: 10px; text-align: center; max-width: 400px; }"
        "h1 { margin-top: 0; font-size: 2em; }"
        "input { width: 100%%; padding: 10px; margin: 10px 0; "
        "border: none; border-radius: 5px; box-sizing: border-box; }"
        "button { background: linear-gradient(135deg, #43e97b 0%%, #38f9d7 100%%);"
        "color: #333; border: none; padding: 10px 20px; border-radius: 5px; "
        "cursor: pointer; font-weight: bold; margin: 5px; width: 100%%; "
        "box-sizing: border-box; }"
        "</style></head><body><div class=\"container\"><h1>Window %d</h1>"
        "<button onclick=\"window.ipc.postMessage('new-window')\">Open a new window</button>"
        "<button onclick=\"window.ipc.postMessage('close')\">Close current window</button>"
        "<input oninput=\"window.ipc.postMessage('change-title:' + this.value)\" placeholder=\"Enter new title\" />"
        "</div></body></html>",
        GRADIENTS[gradient_idx][0], GRADIENTS[gradient_idx][1],
        win_info->window_number);
    
    printf("[WEBVIEW] Setting HTML content...\n");
    fflush(stdout);
    if (wry_builder_with_html(builder, html_content) != WRY_OK) {
        fprintf(stderr, "[ERROR] Failed to set HTML for window %d: %s\n", win_info->window_number, wry_last_error_message());
        fflush(stderr);
        wry_builder_destroy(builder);
        return;
    }
    printf("[WEBVIEW] HTML set successfully\n");
    fflush(stdout);
    
    printf("[WEBVIEW] Configuring builder (devtools, visible, IPC)...\n");
    fflush(stdout);
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    wry_builder_with_ipc_handler(builder, on_ipc_message, (void*)win_info);
    printf("[WEBVIEW] Builder configured\n");
    fflush(stdout);
    
    // Build WebView
    printf("[WEBVIEW] Building WebView as child of hwnd=%p...\n", win_info->hwnd);
    fflush(stdout);
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)win_info->hwnd);
    
    if (!webview) {
        fprintf(stderr, "[ERROR] Failed to build WebView for window %d: %s\n", win_info->window_number, wry_last_error_message());
        fflush(stderr);
        wry_builder_destroy(builder);
        return;
    }
    printf("[WEBVIEW] WebView built successfully\n");
    fflush(stdout);
    
    win_info->webview = webview;
    win_info->webview_created = 1;
    
    printf("[WEBVIEW] Window %d WebView created successfully\n", win_info->window_number);
    fflush(stdout);
    
    // Set initial bounds
    printf("[WEBVIEW] Setting bounds for window %d...\n", win_info->window_number);
    fflush(stdout);
    RECT client_rect;
    GetClientRect(win_info->hwnd, &client_rect);
    WryRect bounds = {
        0,
        0,
        (double)(client_rect.right - client_rect.left),
        (double)(client_rect.bottom - client_rect.top)
    };
    printf("[WEBVIEW] Bounds: x=0, y=0, width=%f, height=%f\n", bounds.width, bounds.height);
    fflush(stdout);
    wry_webview_set_bounds(webview, &bounds);
    
    printf("[WEBVIEW] Window %d is now visible\n\n", win_info->window_number);
    fflush(stdout);
    
    // Note: Do NOT destroy the builder - it's managed by Wry internally
    printf("[WEBVIEW] WebView creation complete, returning to message loop\n");
    fflush(stdout);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Find the window info for this hwnd
    WindowInfo* win_info = NULL;
    for (int i = 0; i < g_app_state.window_count; i++) {
        if (g_app_state.windows[i].hwnd == hwnd) {
            win_info = &g_app_state.windows[i];
            break;
        }
    }
    
    // Log important messages
    if (uMsg == WM_CREATE_WEBVIEW || uMsg == WM_NEW_WINDOW || uMsg == WM_SIZE || uMsg == WM_CLOSE || uMsg == WM_DESTROY) {
        printf("[WINDOW] Message: hwnd=%p, uMsg=%u, win_info=%p\n", hwnd, uMsg, win_info);
        fflush(stdout);
    }
    
    switch (uMsg) {
        case WM_CREATE_WEBVIEW:
            printf("[WINDOW] WM_CREATE_WEBVIEW received\n");
            fflush(stdout);
            if (win_info && !win_info->webview_created) {
                printf("[WINDOW] Creating WebView for window %d\n", win_info->window_number);
                fflush(stdout);
                create_webview_for_window(win_info);
            } else {
                if (!win_info) {
                    fprintf(stderr, "[ERROR] win_info is NULL for WM_CREATE_WEBVIEW\n");
                } else {
                    printf("[DEBUG] WebView already created for window %d\n", win_info->window_number);
                }
                fflush(stdout);
            }
            return 0;
            
        case WM_NEW_WINDOW:
            printf("[WINDOW] WM_NEW_WINDOW received\n");
            fflush(stdout);
            if (g_app_state.window_count < MAX_WINDOWS) {
                int new_window_num = g_app_state.window_count + 1;
                printf("[WINDOW] Creating new window %d\n", new_window_num);
                fflush(stdout);
                create_new_window(new_window_num);
            } else {
                fprintf(stderr, "[ERROR] Maximum windows reached\n");
                fflush(stderr);
            }
            return 0;
            
        case WM_SIZE:
            if (win_info && win_info->webview) {
                RECT client_rect;
                GetClientRect(hwnd, &client_rect);
                WryRect bounds = {
                    0,
                    0,
                    (double)(client_rect.right - client_rect.left),
                    (double)(client_rect.bottom - client_rect.top)
                };
                wry_webview_set_bounds(win_info->webview, &bounds);
            }
            return 0;
            
        case WM_CLOSE:
            printf("[WINDOW] WM_CLOSE received for window %p\n", hwnd);
            fflush(stdout);
            if (win_info) {
                printf("[WINDOW] Closing window %d\n", win_info->window_number);
                fflush(stdout);
                win_info->is_active = 0;
                if (win_info->webview) {
                    printf("[WINDOW] Destroying WebView for window %d\n", win_info->window_number);
                    fflush(stdout);
                    wry_webview_destroy(win_info->webview);
                    win_info->webview = NULL;
                }
            }
            DestroyWindow(hwnd);
            return 0;
            
        case WM_DESTROY:
            printf("[WINDOW] WM_DESTROY received for window %p\n", hwnd);
            fflush(stdout);
            if (win_info) {
                printf("[WINDOW] Marking window %d as destroyed\n", win_info->window_number);
                fflush(stdout);
                win_info->hwnd = NULL;
            }
            // Check if all windows are closed
            int active_count = 0;
            for (int i = 0; i < g_app_state.window_count; i++) {
                if (g_app_state.windows[i].is_active) {
                    active_count++;
                }
            }
            printf("[WINDOW] Active windows remaining: %d\n", active_count);
            fflush(stdout);
            if (active_count == 0) {
                printf("[WINDOW] All windows closed, posting WM_QUIT\n");
                fflush(stdout);
                PostQuitMessage(0);
            }
            return 0;
            
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

WindowInfo* create_new_window(int window_number) {
    printf("[CREATE] Creating window %d (current count: %d)\n", window_number, g_app_state.window_count);
    fflush(stdout);
    
    if (g_app_state.window_count >= MAX_WINDOWS) {
        fprintf(stderr, "[ERROR] Maximum number of windows reached (%d)\n", MAX_WINDOWS);
        fflush(stderr);
        return NULL;
    }
    
    const wchar_t CLASS_NAME[] = L"WryMultiWindowClass";
    
    // Register window class (only once)
    static int class_registered = 0;
    if (!class_registered) {
        printf("[CREATE] Registering window class...\n");
        fflush(stdout);
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = WindowProc;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        
        if (!RegisterClassW(&wc)) {
            fprintf(stderr, "[ERROR] Failed to register window class\n");
            fflush(stderr);
            return NULL;
        }
        printf("[CREATE] Window class registered\n");
        fflush(stdout);
        class_registered = 1;
    }
    
    // Create window
    printf("[CREATE] Creating window with CreateWindowExW...\n");
    fflush(stdout);
    wchar_t title[256];
    swprintf(title, 256, L"Window %d", window_number);
    
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        fprintf(stderr, "[ERROR] Failed to create window %d\n", window_number);
        fflush(stderr);
        return NULL;
    }
    
    printf("[CREATE] Window %d created: hwnd=%p\n", window_number, hwnd);
    fflush(stdout);
    
    // Store window info
    printf("[CREATE] Storing window info at index %d\n", g_app_state.window_count);
    fflush(stdout);
    WindowInfo* win_info = &g_app_state.windows[g_app_state.window_count];
    win_info->window_number = window_number;
    win_info->hwnd = hwnd;
    win_info->is_active = 1;
    win_info->webview_created = 0;
    g_app_state.window_count++;
    printf("[CREATE] Window info stored, total windows: %d\n", g_app_state.window_count);
    fflush(stdout);
    
    printf("[CREATE] Showing window %d...\n", window_number);
    fflush(stdout);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    printf("[CREATE] Window %d is now visible\n", window_number);
    fflush(stdout);
    
    // Post a custom message to create WebView after the window is shown
    printf("[CREATE] Posting WM_CREATE_WEBVIEW for window %d\n", window_number);
    fflush(stdout);
    PostMessageW(hwnd, WM_CREATE_WEBVIEW, 0, 0);
    
    printf("[CREATE] Window %d creation complete\n\n", window_number);
    fflush(stdout);
    
    return win_info;
}

int main() {
    printf("========================================\n");
    printf("Wry Multi-Window Example (C API)\n");
    printf("Version: %s\n", wry_version());
    printf("========================================\n\n");
    fflush(stdout);
    
    printf("[MAIN] Initializing Wry...\n");
    fflush(stdout);
    if (wry_init() != WRY_OK) {
        fprintf(stderr, "[ERROR] Failed to initialize Wry: %s\n", wry_last_error_message());
        fflush(stderr);
        return 1;
    }
    
    printf("[MAIN] Wry initialized successfully\n");
    fflush(stdout);
    
    // Create first window
    printf("[MAIN] Creating first window...\n");
    fflush(stdout);
    WindowInfo* first_window = create_new_window(1);
    if (!first_window) {
        fprintf(stderr, "[ERROR] Failed to create first window\n");
        fflush(stderr);
        return 1;
    }
    
    g_app_state.main_hwnd = first_window->hwnd;
    printf("[MAIN] Main window set to: %p\n", g_app_state.main_hwnd);
    fflush(stdout);
    
    printf("[MAIN] Entering message loop. Close all windows to exit.\n\n");
    fflush(stdout);
    
    MSG msg = {0};
    int msg_count = 0;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        msg_count++;
        if (msg_count % 100 == 0) {
            printf("[MAIN] Processed %d messages\n", msg_count);
            fflush(stdout);
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    printf("\n[MAIN] Message loop exited after %d messages\n", msg_count);
    fflush(stdout);
    printf("[MAIN] Cleaning up...\n");
    fflush(stdout);
    
    // Clean up all windows
    for (int i = 0; i < g_app_state.window_count; i++) {
        if (g_app_state.windows[i].webview) {
            printf("[MAIN] Destroying WebView %d\n", i + 1);
            fflush(stdout);
            wry_webview_destroy(g_app_state.windows[i].webview);
        }
    }
    
    printf("[MAIN] Example completed successfully!\n");
    printf("========================================\n");
    fflush(stdout);
    return 0;
}
