// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! WebViewBuilder C API

use std::ffi::CStr;
use std::os::raw::{c_char, c_int, c_void};
use crate::{WebView, WebViewBuilder};
use super::error::{WryResult, WryErrorCode, set_error_message, set_last_error};
use super::callbacks::{IpcHandlerWrapper, NavigationHandlerWrapper};
use http::Request;

// For C API, use raw pointers and manage ownership manually
pub type WryWebViewBuilder = *mut WebViewBuilder<'static>;
pub type WryWebView = *mut WebView;

/// C API: 创建新的 WebViewBuilder
#[no_mangle]
pub extern "C" fn wry_builder_new() -> WryWebViewBuilder {
    Box::into_raw(Box::new(WebViewBuilder::new()))
}

/// C API: 用 WebContext 创建 WebViewBuilder
#[no_mangle]
pub extern "C" fn wry_builder_new_with_context(
    context: *mut super::context::WryWebContext,
) -> WryWebViewBuilder {
    if context.is_null() {
        return std::ptr::null_mut();
    }
    
    unsafe {
        let builder = WebViewBuilder::new_with_web_context(&mut **context);
        Box::into_raw(Box::new(builder))
    }
}

/// C API: 设置 URL
#[no_mangle]
pub extern "C" fn wry_builder_with_url(
    builder: WryWebViewBuilder,
    url: *const c_char,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if url.is_null() {
        set_error_message("url pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let url_str = CStr::from_ptr(url).to_string_lossy();
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_url(url_str.as_ref());
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置 HTML
#[no_mangle]
pub extern "C" fn wry_builder_with_html(
    builder: WryWebViewBuilder,
    html: *const c_char,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if html.is_null() {
        set_error_message("html pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let html_str = CStr::from_ptr(html).to_string_lossy();
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_html(html_str.as_ref());
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置透明度
#[no_mangle]
pub extern "C" fn wry_builder_with_transparent(
    builder: WryWebViewBuilder,
    transparent: c_int,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_transparent(transparent != 0);
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置可见性
#[no_mangle]
pub extern "C" fn wry_builder_with_visible(
    builder: WryWebViewBuilder,
    visible: c_int,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_visible(visible != 0);
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置开发者工具
#[no_mangle]
pub extern "C" fn wry_builder_with_devtools(
    builder: WryWebViewBuilder,
    devtools: c_int,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_devtools(devtools != 0);
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置用户代理
#[no_mangle]
pub extern "C" fn wry_builder_with_user_agent(
    builder: WryWebViewBuilder,
    user_agent: *const c_char,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if user_agent.is_null() {
        set_error_message("user_agent pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let ua_str = CStr::from_ptr(user_agent).to_string_lossy();
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_user_agent(ua_str.as_ref());
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置 IPC 处理器
/// @note userdata 指针必须在 WebView 销毁前保持有效
#[no_mangle]
pub extern "C" fn wry_builder_with_ipc_handler(
    builder: WryWebViewBuilder,
    callback: extern "C" fn(*const c_char, usize, *mut c_void),
    userdata: *mut c_void,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if (callback as *const c_void).is_null() {
        set_error_message("callback function pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_ipc_handler(move |request: Request<String>| {
            let handler = IpcHandlerWrapper::new(callback, userdata);
            handler.call(&request.body());
        });
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置导航处理器
/// @note userdata 指针必须在 WebView 销毁前保持有效
#[no_mangle]
pub extern "C" fn wry_builder_with_navigation_handler(
    builder: WryWebViewBuilder,
    callback: extern "C" fn(*const c_char, *mut c_void) -> u8,
    userdata: *mut c_void,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if (callback as *const c_void).is_null() {
        set_error_message("callback function pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_navigation_handler(move |url: String| {
            let handler = NavigationHandlerWrapper::new(callback, userdata);
            handler.call(&url)
        });
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 设置 WebView 边界（在构建前）
#[no_mangle]
pub extern "C" fn wry_builder_with_bounds(
    builder: WryWebViewBuilder,
    x: f64,
    y: f64,
    width: f64,
    height: f64,
) -> WryResult {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let bounds = crate::Rect {
            position: crate::dpi::LogicalPosition::new(x, y).into(),
            size: crate::dpi::LogicalSize::new(width, height).into(),
        };
        let builder_val = std::ptr::read(builder);
        let updated_builder = builder_val.with_bounds(bounds);
        std::ptr::write(builder, updated_builder);
    }
    
    WryErrorCode::WRY_OK
}

/// Wrapper for window handle to implement HasWindowHandle
#[cfg(target_os = "windows")]
struct WindowHandleWrapper {
    hwnd: std::num::NonZeroIsize,
}

#[cfg(target_os = "windows")]
impl raw_window_handle::HasWindowHandle for WindowHandleWrapper {
    fn window_handle(&self) -> std::result::Result<raw_window_handle::WindowHandle<'_>, raw_window_handle::HandleError> {
        use raw_window_handle::{RawWindowHandle, WindowHandle};
        let raw_handle = RawWindowHandle::Win32(raw_window_handle::Win32WindowHandle::new(self.hwnd));
        Ok(unsafe { WindowHandle::borrow_raw(raw_handle) })
    }
}

/// C API: 构建 WebView 作为子窗口
/// @param builder WebViewBuilder 指针（消费 builder）
/// @param parent_hwnd 父窗口句柄（HWND on Windows, GtkWidget* on Linux, NSView* on macOS）
/// @return WebView 指针，失败返回 NULL
#[no_mangle]
pub extern "C" fn wry_builder_build_as_child(
    builder: WryWebViewBuilder,
    parent_hwnd: *mut std::os::raw::c_void,
) -> WryWebView {
    if builder.is_null() {
        set_error_message("builder pointer is null");
        return std::ptr::null_mut();
    }
    if parent_hwnd.is_null() {
        set_error_message("parent_hwnd pointer is null");
        return std::ptr::null_mut();
    }
    
    unsafe {
        let builder_val = Box::from_raw(builder);
        
        #[cfg(target_os = "windows")]
        {
            use std::num::NonZeroIsize;
            
            let hwnd = parent_hwnd as isize;
            if let Ok(handle) = NonZeroIsize::try_from(hwnd) {
                let wrapper = WindowHandleWrapper { hwnd: handle };
                
                match builder_val.build_as_child(&wrapper) {
                    Ok(webview) => {
                        let boxed = Box::new(webview);
                        Box::into_raw(boxed)
                    },
                    Err(err) => {
                        set_last_error(err);
                        std::ptr::null_mut()
                    }
                }
            } else {
                set_error_message("Invalid window handle");
                std::ptr::null_mut()
            }
        }
        
        #[cfg(not(target_os = "windows"))]
        {
            set_error_message("build_as_child is only supported on Windows");
            std::ptr::null_mut()
        }
    }
}

/// C API: 销毁 Builder
#[no_mangle]
pub extern "C" fn wry_builder_destroy(builder: WryWebViewBuilder) {
    if !builder.is_null() {
        unsafe {
            drop(Box::from_raw(builder));
        }
    }
}

