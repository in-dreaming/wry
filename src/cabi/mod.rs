// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! C ABI 模块
//! 
//! 提供 wry WebView 库的 C 语言绑定

#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::missing_safety_doc)]

pub mod error;
pub mod types;
pub mod callbacks;
pub mod context;
pub mod builder;
pub mod webview;
pub mod cookies;

use std::ffi::CString;
use std::os::raw::c_char;
use std::sync::OnceLock;

// Re-export for convenience
pub use error::*;
pub use types::*;

/// C API: 获取 wry 版本号
#[no_mangle]
pub extern "C" fn wry_version() -> *const c_char {
    static VERSION: &str = env!("CARGO_PKG_VERSION");
    static VERSION_CSTR: OnceLock<CString> = OnceLock::new();
    
    VERSION_CSTR.get_or_init(|| CString::new(VERSION).unwrap()).as_ptr()
}

/// C API: 初始化（Linux GTK）
#[cfg(gtk)]
#[no_mangle]
pub extern "C" fn wry_init() -> WryResult {
    use gtk::glib;
    
    let result = glib::init();
    match result {
        Ok(_) => WryErrorCode::WRY_OK,
        Err(err) => {
            set_error_message(format!("Failed to initialize GTK: {}", err));
            WryErrorCode::WRY_ERROR_INIT_FAILED
        }
    }
}

/// C API: 初始化（非 Linux 平台为空操作）
#[cfg(not(gtk))]
#[no_mangle]
pub extern "C" fn wry_init() -> WryResult {
    WryErrorCode::WRY_OK
}

/// C API: 释放字符串
#[no_mangle]
pub extern "C" fn wry_string_free(ptr: *mut c_char) {
    if !ptr.is_null() {
        unsafe {
            drop(CString::from_raw(ptr));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CStr;
    
    #[test]
    fn test_version() {
        let version = unsafe { CStr::from_ptr(wry_version()) };
        assert!(!version.to_str().unwrap().is_empty());
    }
    
    #[test]
    fn test_builder_creation() {
        let builder = builder::wry_builder_new();
        assert!(!builder.is_null(), "Builder should not be null");
        builder::wry_builder_destroy(builder);
    }

    #[test]
    fn test_builder_with_url() {
        let builder = builder::wry_builder_new();
        assert!(!builder.is_null());

        let url = CString::new("https://example.com").unwrap();
        let result = builder::wry_builder_with_url(builder, url.as_ptr());
        assert_eq!(result, error::WryErrorCode::WRY_OK);

        builder::wry_builder_destroy(builder);
    }

    #[test]
    fn test_builder_with_html() {
        let builder = builder::wry_builder_new();
        assert!(!builder.is_null());

        let html = CString::new("<html><body>Test</body></html>").unwrap();
        let result = builder::wry_builder_with_html(builder, html.as_ptr());
        assert_eq!(result, error::WryErrorCode::WRY_OK);

        builder::wry_builder_destroy(builder);
    }

    #[test]
    fn test_builder_with_bounds() {
        let builder = builder::wry_builder_new();
        assert!(!builder.is_null());

        let result = builder::wry_builder_with_bounds(builder, 100.0, 100.0, 800.0, 600.0);
        assert_eq!(result, error::WryErrorCode::WRY_OK);

        builder::wry_builder_destroy(builder);
    }

    #[test]
    fn test_builder_null_checks() {
        let url = CString::new("https://example.com").unwrap();
        let result = builder::wry_builder_with_url(std::ptr::null_mut(), url.as_ptr());
        assert_ne!(result, error::WryErrorCode::WRY_OK);
    }

    #[test]
    fn test_types_header() {
        let key = CString::new("Authorization").unwrap();
        let value = CString::new("Bearer token").unwrap();

        let header = types::WryHeader {
            key: key.as_ptr(),
            value: value.as_ptr(),
        };

        assert_eq!(unsafe { CStr::from_ptr(header.key).to_string_lossy() }, "Authorization");
        assert_eq!(unsafe { CStr::from_ptr(header.value).to_string_lossy() }, "Bearer token");
    }

    #[test]
    fn test_types_rect() {
        let rect = types::WryRect {
            x: 100.0,
            y: 200.0,
            width: 800.0,
            height: 600.0,
        };

        assert_eq!(rect.x, 100.0);
        assert_eq!(rect.y, 200.0);
        assert_eq!(rect.width, 800.0);
        assert_eq!(rect.height, 600.0);
    }
}


