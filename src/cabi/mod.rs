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
    
    #[test]
    fn test_version() {
        let version = unsafe { CStr::from_ptr(wry_version()) };
        assert!(!version.to_str().unwrap().is_empty());
    }
}


