// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! WebContext C API

use std::ffi::CStr;
use std::os::raw::{c_char, c_int};
use crate::WebContext;
use super::error::{WryResult, WryErrorCode, set_error_message};

pub type WryWebContext = Box<WebContext>;

/// C API: 创建新的 WebContext
#[no_mangle]
pub extern "C" fn wry_context_new(data_dir: *const c_char) -> *mut WryWebContext {
    let context = if !data_dir.is_null() {
        unsafe {
            let path = CStr::from_ptr(data_dir).to_string_lossy();
            WebContext::new(Some(std::path::PathBuf::from(path.as_ref())))
        }
    } else {
        WebContext::new(None)
    };
    
    Box::into_raw(Box::new(Box::new(context)))
}

/// C API: 销毁 WebContext
#[no_mangle]
pub extern "C" fn wry_context_destroy(context: *mut WryWebContext) {
    if !context.is_null() {
        unsafe {
            drop(Box::from_raw(context));
        }
    }
}

/// C API: 设置是否允许自动化
#[no_mangle]
pub extern "C" fn wry_context_set_allows_automation(
    context: *mut WryWebContext,
    flag: c_int,
) -> WryResult {
    if context.is_null() {
        set_error_message("Context is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        (**context).set_allows_automation(flag != 0);
    }
    
    WryErrorCode::WRY_OK
}

/// C API: 检查协议是否已注册
#[no_mangle]
pub extern "C" fn wry_context_is_protocol_registered(
    context: *const WryWebContext,
    name: *const c_char,
) -> c_int {
    if context.is_null() || name.is_null() {
        return 0;
    }
    
    unsafe {
        let name_str = CStr::from_ptr(name).to_string_lossy();
        (**context).is_custom_protocol_registered(&name_str) as c_int
    }
}

