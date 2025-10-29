// Copyright 2020-2023 Tauri Programme within The Commons Conservancy
// SPDX-License-Identifier: Apache-2.0
// SPDX-License-Identifier: MIT

//! C ABI 类型定义

use std::os::raw::{c_char, c_void};

/// 矩形结构
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct WryRect {
    pub x: f64,
    pub y: f64,
    pub width: f64,
    pub height: f64,
}

/// RGBA 颜色结构
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct WryRGBA {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

/// 窗口句柄类型
#[repr(C)]
pub enum WryWindowHandleKind {
    WRY_WINDOW_HANDLE_X11,
    WRY_WINDOW_HANDLE_WIN32,
    WRY_WINDOW_HANDLE_APPLE,
}

/// 通用窗口句柄
#[repr(C)]
pub struct WryWindowHandle {
    pub kind: WryWindowHandleKind,
    pub handle: *mut c_void,
}

/// 代理配置类型
#[repr(C)]
pub enum WryProxyType {
    WRY_PROXY_HTTP,
    WRY_PROXY_SOCKS5,
}

/// 代理终端点
#[repr(C)]
pub struct WryProxyEndpoint {
    pub host: *const c_char,
    pub port: *const c_char,
}

/// 代理配置
#[repr(C)]
pub struct WryProxyConfig {
    pub proxy_type: WryProxyType,
    pub endpoint: WryProxyEndpoint,
}

impl From<crate::Rect> for WryRect {
    fn from(rect: crate::Rect) -> Self {
        use crate::dpi;
        let x = match rect.position {
            dpi::Position::Physical(p) => p.x as f64,
            dpi::Position::Logical(l) => l.x as f64,
        };
        let y = match rect.position {
            dpi::Position::Physical(p) => p.y as f64,
            dpi::Position::Logical(l) => l.y as f64,
        };
        let w = match rect.size {
            dpi::Size::Physical(p) => p.width as f64,
            dpi::Size::Logical(l) => l.width as f64,
        };
        let h = match rect.size {
            dpi::Size::Physical(p) => p.height as f64,
            dpi::Size::Logical(l) => l.height as f64,
        };
        Self { x, y, width: w, height: h }
    }
}

impl From<WryRect> for crate::Rect {
    fn from(rect: WryRect) -> Self {
        use crate::dpi::{PhysicalPosition, PhysicalSize};
        Self {
            position: PhysicalPosition::new(rect.x as f64, rect.y as f64).into(),
            size: PhysicalSize::new(rect.width as f64, rect.height as f64).into(),
        }
    }
}

impl From<crate::RGBA> for WryRGBA {
    fn from(rgba: crate::RGBA) -> Self {
        Self {
            r: rgba.0,
            g: rgba.1,
            b: rgba.2,
            a: rgba.3,
        }
    }
}

impl From<WryRGBA> for crate::RGBA {
    fn from(rgba: WryRGBA) -> Self {
        (rgba.r, rgba.g, rgba.b, rgba.a)
    }
}
