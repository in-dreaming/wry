新增了wry的cabi实现，且使用zig build

# 使用 Zig 构建系统（推荐）
zig build -Dcabi=true         # 构建库和 C ABI
zig build build-examples      # 构建 C 示例
zig build run                 # 运行示例
zig build test                # 运行测试

# 或使用传统 Cargo
cargo build --lib --features cabi
cd examples/c_api && make