// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! WebView C API

use std::ffi::CStr;
use std::os::raw::{c_char, c_double, c_int};
use super::{error::*};
use super::builder::WryWebView;

/// C API: 销毁 WebView
#[no_mangle]
pub extern "C" fn wry_webview_destroy(webview: WryWebView) {
    if !webview.is_null() {
        unsafe {
            drop(Box::from_raw(webview));
        }
    }
}

/// C API: 获取 WebView ID
#[no_mangle]
pub extern "C" fn wry_webview_id(
    webview: WryWebView,
    out_id: *mut c_char,
    len: usize,
) -> WryResult {
    if webview.is_null() || out_id.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ref = &*webview;
        let id = webview_ref.id();
        let id_bytes = id.as_bytes();
        
        if id_bytes.len() >= len {
            set_error_message("Buffer too small");
            return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
        }
        
        std::ptr::copy_nonoverlapping(id_bytes.as_ptr(), out_id as *mut u8, id_bytes.len());
        *out_id.add(id_bytes.len()) = 0;
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 获取当前 URL
#[no_mangle]
pub extern "C" fn wry_webview_url(webview: *const WryWebView, out_url: *mut *mut c_char) -> WryResult {
    if webview.is_null() || out_url.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        match webview_ref.url() {
            Ok(url) => {
                *out_url = std::ffi::CString::new(url).unwrap().into_raw();
                WryErrorCode::WRY_OK
            }
            Err(err) => {
                set_last_error(err);
                WryErrorCode::WRY_ERROR_GENERIC
            }
        }
    }
}

/// C API: 执行 JavaScript
#[no_mangle]
pub extern "C" fn wry_webview_eval(
    webview: *mut WryWebView,
    js: *const c_char,
) -> WryResult {
    if webview.is_null() || js.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let js_str = CStr::from_ptr(js).to_string_lossy();
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.evaluate_script(&js_str))
    }
}

/// C API: 加载 URL
#[no_mangle]
pub extern "C" fn wry_webview_load_url(
    webview: *mut WryWebView,
    url: *const c_char,
) -> WryResult {
    if webview.is_null() || url.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let url_str = CStr::from_ptr(url).to_string_lossy();
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.load_url(&url_str))
    }
}

/// C API: 加载 HTML
#[no_mangle]
pub extern "C" fn wry_webview_load_html(
    webview: *mut WryWebView,
    html: *const c_char,
) -> WryResult {
    if webview.is_null() || html.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let html_str = CStr::from_ptr(html).to_string_lossy();
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.load_html(&html_str))
    }
}

/// C API: 重新加载
#[no_mangle]
pub extern "C" fn wry_webview_reload(webview: *mut WryWebView) -> WryResult {
    if webview.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.reload())
    }
}

/// C API: 打印
#[no_mangle]
pub extern "C" fn wry_webview_print(webview: *mut WryWebView) -> WryResult {
    if webview.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.print())
    }
}

/// C API: 设置缩放
#[no_mangle]
pub extern "C" fn wry_webview_zoom(
    webview: *mut WryWebView,
    scale: c_double,
) -> WryResult {
    if webview.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.zoom(scale))
    }
}

/// C API: 设置可见性
#[no_mangle]
pub extern "C" fn wry_webview_set_visible(
    webview: *mut WryWebView,
    visible: c_int,
) -> WryResult {
    if webview.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.set_visible(visible != 0))
    }
}

/// C API: 获取焦点
#[no_mangle]
pub extern "C" fn wry_webview_focus(webview: *mut WryWebView) -> WryResult {
    if webview.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let webview_ptr = webview.as_ref().unwrap();
        let webview_ref = webview_ptr.as_ref().unwrap();
        check_result(webview_ref.focus())
    }
}

/// C API: 打开开发者工具
#[no_mangle]
pub extern "C" fn wry_webview_open_devtools(webview: *mut WryWebView) {
    if !webview.is_null() {
        unsafe {
            if let Some(webview_ptr) = webview.as_ref() {
                if let Some(webview_ref) = webview_ptr.as_ref() {
                    webview_ref.open_devtools();
                }
            }
        }
    }
}

/// C API: 关闭开发者工具
#[no_mangle]
pub extern "C" fn wry_webview_close_devtools(webview: *mut WryWebView) {
    if !webview.is_null() {
        unsafe {
            if let Some(webview_ptr) = webview.as_ref() {
                if let Some(webview_ref) = webview_ptr.as_ref() {
                    webview_ref.close_devtools();
                }
            }
        }
    }
}

/// C API: 检查开发者工具是否打开
#[no_mangle]
pub extern "C" fn wry_webview_is_devtools_open(webview: *const WryWebView) -> c_int {
    if webview.is_null() {
        return 0;
    }
    
    unsafe {
        if let Some(webview_ptr) = webview.as_ref() {
            if let Some(webview_ref) = webview_ptr.as_ref() {
                webview_ref.is_devtools_open() as c_int
            } else {
                0
            }
        } else {
            0
        }
    }
}

/// C API: 设置 WebView 边界（位置和大小）
#[no_mangle]
pub extern "C" fn wry_webview_set_bounds(
    webview: WryWebView,
    bounds: *const super::types::WryRect,
) -> WryResult {
    if webview.is_null() || bounds.is_null() {
        set_error_message("Invalid argument");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let rect = bounds.as_ref().unwrap();
        let wry_rect = crate::Rect {
            position: crate::dpi::LogicalPosition::new(rect.x, rect.y).into(),
            size: crate::dpi::LogicalSize::new(rect.width, rect.height).into(),
        };
        
        // webview is WryWebView = *mut WebView
        let webview_ref = &*webview;
        check_result(webview_ref.set_bounds(wry_rect))
    }
}


