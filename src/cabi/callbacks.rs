// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! 回调函数管理

use std::ffi::CString;
use std::os::raw::{c_char, c_void};

/// IPC 回调函数类型
pub type WryIpcCallback = extern "C" fn(*const c_char, usize, *mut c_void);

/// 导航回调函数类型
pub type WryNavigationCallback = extern "C" fn(*const c_char, *mut c_void) -> u8; // bool as u8

/// IPC 处理器包装器
pub struct IpcHandlerWrapper {
    callback: WryIpcCallback,
    userdata: *mut c_void,
}

unsafe impl Send for IpcHandlerWrapper {}
unsafe impl Sync for IpcHandlerWrapper {}

impl IpcHandlerWrapper {
    pub fn new(callback: WryIpcCallback, userdata: *mut c_void) -> Self {
        Self { callback, userdata }
    }
    
    pub fn call(&self, message: &str) {
        let c_str = CString::new(message).unwrap_or_default();
        (self.callback)(c_str.as_ptr(), c_str.as_bytes().len(), self.userdata);
    }
}

/// 导航处理器包装器
pub struct NavigationHandlerWrapper {
    callback: WryNavigationCallback,
    userdata: *mut c_void,
}

unsafe impl Send for NavigationHandlerWrapper {}
unsafe impl Sync for NavigationHandlerWrapper {}

impl NavigationHandlerWrapper {
    pub fn new(callback: WryNavigationCallback, userdata: *mut c_void) -> Self {
        Self { callback, userdata }
    }
    
    pub fn call(&self, url: &str) -> bool {
        let c_str = CString::new(url).unwrap_or_default();
        (self.callback)(c_str.as_ptr(), self.userdata) != 0
    }
}

/// 自定义协议回调函数类型（简化版）
pub type WryCustomProtocolCallback = extern "C" fn(
    *const c_char,  // url
    *const c_char,   // method
    *const c_char,   // body
    usize,           // body length
    *mut c_void,     // response sender
    *mut c_void,     // userdata
);

/// 拖放回调函数类型
pub type WryDragDropCallback = extern "C" fn(*const c_char, *const c_char, *mut c_void) -> u8; // bool as u8


