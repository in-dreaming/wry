// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! Cookie 管理 C API

use std::os::raw::c_char;
use super::{error::*};
use super::builder::WryWebView;

/// C Cookie 结构
#[repr(C)]
pub struct WryCookie {
    pub name: *mut c_char,
    pub value: *mut c_char,
    pub domain: *mut c_char,
    pub path: *mut c_char,
}

/// C API: 获取所有 cookies
/// @note 调用者必须调用 wry_cookies_free() 释放返回的 cookies 数组
#[no_mangle]
pub extern "C" fn wry_webview_cookies(
    webview: *const WryWebView,
    out_cookies: *mut *mut WryCookie,
    out_count: *mut usize,
) -> WryResult {
    if webview.is_null() {
        set_error_message("webview pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if out_cookies.is_null() {
        set_error_message("out_cookies pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if out_count.is_null() {
        set_error_message("out_count pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        match (**webview).cookies() {
            Ok(cookies) => {
                let count = cookies.len();
                let mut c_cookies = Vec::with_capacity(count);
                
                for cookie in cookies {
                    c_cookies.push(WryCookie {
                        name: std::ffi::CString::new(cookie.name().to_string()).unwrap().into_raw(),
                        value: std::ffi::CString::new(cookie.value().to_string()).unwrap().into_raw(),
                        domain: std::ffi::CString::new(cookie.domain().unwrap_or("")).unwrap().into_raw(),
                        path: std::ffi::CString::new(cookie.path().unwrap_or("")).unwrap().into_raw(),
                    });
                }
                
                let ptr = c_cookies.leak();
                *out_cookies = ptr.as_mut_ptr();
                *out_count = count;
                WryErrorCode::WRY_OK
            }
            Err(err) => {
                set_last_error(err);
                WryErrorCode::WRY_ERROR_GENERIC
            }
        }
    }
}

/// C API: 释放 cookie
#[no_mangle]
pub extern "C" fn wry_cookie_free(cookie: *mut WryCookie) {
    if !cookie.is_null() {
        unsafe {
            drop(std::ffi::CString::from_raw((*cookie).name));
            drop(std::ffi::CString::from_raw((*cookie).value));
            drop(std::ffi::CString::from_raw((*cookie).domain));
            drop(std::ffi::CString::from_raw((*cookie).path));
        }
    }
}

/// C API: 释放 cookies 数组
#[no_mangle]
pub extern "C" fn wry_cookies_free(cookies: *mut WryCookie, count: usize) {
    if !cookies.is_null() {
        unsafe {
            for i in 0..count {
                wry_cookie_free(cookies.add(i));
            }
            drop(Vec::from_raw_parts(cookies, count, count));
        }
    }
}

/// C API: 设置 Cookie
#[no_mangle]
pub extern "C" fn wry_webview_set_cookie(
    webview: WryWebView,
    name: *const std::os::raw::c_char,
    value: *const std::os::raw::c_char,
    domain: *const std::os::raw::c_char,
    path: *const std::os::raw::c_char,
) -> WryResult {
    if webview.is_null() {
        set_error_message("webview pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if name.is_null() {
        set_error_message("name pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if value.is_null() {
        set_error_message("value pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let name_str = std::ffi::CStr::from_ptr(name).to_string_lossy().into_owned();
        let value_str = std::ffi::CStr::from_ptr(value).to_string_lossy().into_owned();
        let domain_str = if !domain.is_null() {
            std::ffi::CStr::from_ptr(domain).to_string_lossy().into_owned()
        } else {
            String::new()
        };
        let path_str = if !path.is_null() {
            std::ffi::CStr::from_ptr(path).to_string_lossy().into_owned()
        } else {
            String::new()
        };
        
        let mut cookie_builder = cookie::Cookie::build((name_str.clone(), value_str.clone()));
        
        if !domain_str.is_empty() {
            cookie_builder = cookie_builder.domain(domain_str.clone());
        }
        
        if !path_str.is_empty() {
            cookie_builder = cookie_builder.path(path_str.clone());
        }
        
        let cookie = cookie_builder.build();
        let webview_ref = &*webview;
        
        match webview_ref.set_cookie(&cookie) {
            Ok(_) => WryErrorCode::WRY_OK,
            Err(err) => {
                set_last_error(err);
                WryErrorCode::WRY_ERROR_GENERIC
            }
        }
    }
}

/// C API: 删除 Cookie
#[no_mangle]
pub extern "C" fn wry_webview_delete_cookie(
    webview: WryWebView,
    name: *const std::os::raw::c_char,
) -> WryResult {
    if webview.is_null() {
        set_error_message("webview pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    if name.is_null() {
        set_error_message("name pointer is null");
        return WryErrorCode::WRY_ERROR_INVALID_ARGUMENT;
    }
    
    unsafe {
        let name_str = std::ffi::CStr::from_ptr(name).to_string_lossy().into_owned();
        let cookie_obj = cookie::Cookie::build((name_str.clone(), String::new())).build();
        let webview_ref = &*webview;
        
        match webview_ref.delete_cookie(&cookie_obj) {
            Ok(_) => WryErrorCode::WRY_OK,
            Err(err) => {
                set_last_error(err);
                WryErrorCode::WRY_ERROR_GENERIC
            }
        }
    }
}


