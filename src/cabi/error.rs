// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! C ABI 错误处理系统

use std::cell::RefCell;
use std::ffi::CString;
use crate::Error;

thread_local! {
    static LAST_ERROR: RefCell<Option<CString>> = RefCell::new(None);
}

/// 错误代码
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[allow(non_camel_case_types)]
pub enum WryErrorCode {
    WRY_OK = 0,
    WRY_ERROR_GENERIC = -1,
    WRY_ERROR_INVALID_ARGUMENT = -2,
    WRY_ERROR_INVALID_STATE = -3,
    WRY_ERROR_INIT_FAILED = -4,
    WRY_ERROR_GLIB = -5,
    WRY_ERROR_X11 = -6,
    WRY_ERROR_SCRIPT = -7,
    WRY_ERROR_RPC = -8,
    WRY_ERROR_IO = -9,
    WRY_ERROR_WEBVIEW2 = -10,
    WRY_ERROR_HTTP = -11,
    WRY_ERROR_WINDOW_HANDLE = -12,
    WRY_ERROR_UNSUPPORTED = -13,
    WRY_ERROR_MEMORY = -14,
    WRY_ERROR_PROTOCOL = -15,
}

/// C API 结果类型
pub type WryResult = WryErrorCode;

/// 设置最后一个错误
pub fn set_last_error(error: Error) {
    let (code, message) = convert_error(error);
    
    LAST_ERROR.with(|err| {
        *err.borrow_mut() = Some(CString::new(message).unwrap_or_else(|_| {
            CString::new(format!("Error code: {}", code as i32)).unwrap()
        }));
    });
}

/// 设置自定义错误消息
pub fn set_error_message<T: Into<String>>(message: T) {
    LAST_ERROR.with(|err| {
        *err.borrow_mut() = Some(
            CString::new(message.into())
                .unwrap_or_else(|_| CString::new("Invalid error message").unwrap())
        );
    });
}

/// 获取最后一个错误消息
pub fn last_error_message() -> Option<CString> {
    LAST_ERROR.with(|err| err.borrow().clone())
}

/// 清理错误消息
pub fn clear_last_error() {
    LAST_ERROR.with(|err| *err.borrow_mut() = None);
}

/// 将 Rust Error 转换为错误代码和消息
pub fn convert_error(error: Error) -> (WryErrorCode, String) {
    let code = match &error {
        #[cfg(gtk)]
        Error::GlibError(_) | Error::GlibBoolError(_) => WryErrorCode::WRY_ERROR_GLIB,
        
        #[cfg(gtk)]
        Error::XlibError(_) | Error::X11DisplayNotFound => WryErrorCode::WRY_ERROR_X11,
        
        Error::InitScriptError => WryErrorCode::WRY_ERROR_SCRIPT,
        Error::RpcScriptError(_, _) => WryErrorCode::WRY_ERROR_RPC,
        
        Error::Io(_) => WryErrorCode::WRY_ERROR_IO,
        
        #[cfg(target_os = "windows")]
        Error::WebView2Error(_) => WryErrorCode::WRY_ERROR_WEBVIEW2,
        
        Error::HttpError(_) => WryErrorCode::WRY_ERROR_HTTP,
        Error::WindowHandleError(_) | Error::UnsupportedWindowHandle => WryErrorCode::WRY_ERROR_WINDOW_HANDLE,
        
        Error::UrlSchemeRegisterError(_) | Error::DuplicateCustomProtocol(_) | Error::ContextDuplicateCustomProtocol(_) => WryErrorCode::WRY_ERROR_PROTOCOL,
        
        Error::CustomProtocolTaskInvalid => WryErrorCode::WRY_ERROR_INVALID_STATE,
        
        _ => WryErrorCode::WRY_ERROR_GENERIC,
    };
    
    (code, error.to_string())
}

/// 检查结果并设置错误
pub fn check_result<T>(result: Result<T, Error>) -> WryResult {
    match result {
        Ok(_) => WryErrorCode::WRY_OK,
        Err(err) => {
            set_last_error(err);
            WryErrorCode::WRY_ERROR_GENERIC
        }
    }
}

/// 将 Rust 结果转换为 C 错误代码
pub fn map_result<T>(result: Result<T, Error>) -> (WryResult, Option<T>) {
    match result {
        Ok(value) => (WryErrorCode::WRY_OK, Some(value)),
        Err(err) => {
            set_last_error(err);
            (WryErrorCode::WRY_ERROR_GENERIC, None)
        }
    }
}

/// C API: 获取最后一个错误消息
#[no_mangle]
pub extern "C" fn wry_last_error_message() -> *const std::os::raw::c_char {
    LAST_ERROR.with(|err| {
        err.borrow()
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or_else(|| b"\0".as_ptr() as *const i8)
    })
}

/// C API: 释放错误消息
#[no_mangle]
pub extern "C" fn wry_clear_last_error() {
    clear_last_error();
}

/// 检查错误代码并转换结果
pub fn check_error_code<T: std::fmt::Debug>(
    result: Result<T, Error>,
    expected_code: WryErrorCode,
) -> bool {
    if result.is_ok() {
        return false;
    }
    let (code, _) = convert_error(result.unwrap_err());
    code == expected_code
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_error_conversion() {
        let err = Error::InitScriptError;
        let (code, msg) = convert_error(err);
        assert_eq!(code, WryErrorCode::WRY_ERROR_SCRIPT);
        assert!(!msg.is_empty());
    }
    
    #[test]
    fn test_last_error() {
        set_error_message("Test error");
        assert!(last_error_message().is_some());
        clear_last_error();
        assert!(last_error_message().is_none());
    }
}

