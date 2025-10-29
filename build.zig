const std = @import("std");
const builtin = @import("builtin");

pub fn build(b: *std.Build) void {
    // 标准构建选项
    const optimize = b.standardOptimizeOption(.{});
    const target = b.standardTargetOptions(.{});

    // ========== 配置选项 ==========
    const cabi_feature = b.option(bool, "cabi", "Enable C ABI support") orelse false;

    // ========== 主构建步骤 ==========
    const build_step = b.step("build", "Build the library");

    // ========== Rust 库构建 ==========

    // 使用 cargo 构建主库
    const build_cargo = b.addSystemCommand(&.{ "cargo", "build", "--lib" });
    if (cabi_feature) {
        build_cargo.addArgs(&.{ "--features", "cabi" });
    }
    build_step.dependOn(&build_cargo.step);

    // 添加 Rust 库构建步骤
    const lib_step = b.step("build-lib", "Build the Rust library");
    lib_step.dependOn(&build_cargo.step);

    // ========== C ABI 构建 ==========

    if (cabi_feature) {
        // 复制头文件
        const header_file = b.addInstallFileWithDir(.{ .cwd_relative = "include/wry.h" }, .header, "wry.h");
        b.getInstallStep().dependOn(&header_file.step);

        const cabi_step = b.step("build-cabi", "Build the C ABI layer");
        cabi_step.dependOn(lib_step);

        // Install wry.dll into zig-out/bin on Windows so examples can run
        var dll_install_step: ?*std.Build.Step = null;
        if (builtin.os.tag == .windows) {
            const dll_install = b.addInstallFileWithDir(.{ .cwd_relative = "target/debug/wry.dll" }, .bin, "wry.dll");
            dll_install.step.dependOn(&build_cargo.step);
            b.getInstallStep().dependOn(&dll_install.step);
            dll_install_step = &dll_install.step;
        }

        // ========== C API 示例构建 ==========

        // 确保先构建 Rust 库
        const build_rust_dep = b.addSystemCommand(&.{ "cargo", "build", "--lib" });
        if (cabi_feature) {
            build_rust_dep.addArgs(&.{ "--features", "cabi" });
        }

        // simple.c 示例
        const simple_exe = b.addExecutable(.{
            .name = "simple",
            .target = target,
            .optimize = optimize,
        });
        simple_exe.addCSourceFile(.{ .file = .{ .cwd_relative = "examples/c_api/simple.c" }, .flags = &.{ "-std=c11", "-Wall", "-Wextra" } });
        simple_exe.linkLibC();
        simple_exe.addIncludePath(.{ .cwd_relative = "include" });
        simple_exe.addLibraryPath(.{ .cwd_relative = "target/debug" });
        simple_exe.linkSystemLibrary("wry");
        simple_exe.step.dependOn(&build_rust_dep.step);
        if (dll_install_step) |s| simple_exe.step.dependOn(s);
        b.installArtifact(simple_exe);

        // ipc_example.c 示例
        const ipc_exe = b.addExecutable(.{
            .name = "ipc_example",
            .target = target,
            .optimize = optimize,
        });
        ipc_exe.addCSourceFile(.{ .file = .{ .cwd_relative = "examples/c_api/ipc_example.c" }, .flags = &.{ "-std=c11", "-Wall", "-Wextra" } });
        ipc_exe.linkLibC();
        ipc_exe.addIncludePath(.{ .cwd_relative = "include" });
        ipc_exe.addLibraryPath(.{ .cwd_relative = "target/debug" });
        ipc_exe.linkSystemLibrary("wry");
        ipc_exe.step.dependOn(&build_rust_dep.step);
        if (dll_install_step) |s| ipc_exe.step.dependOn(s);
        b.installArtifact(ipc_exe);

        // custom_protocol.c 示例
        const protocol_exe = b.addExecutable(.{
            .name = "custom_protocol",
            .target = target,
            .optimize = optimize,
        });
        protocol_exe.addCSourceFile(.{ .file = .{ .cwd_relative = "examples/c_api/custom_protocol.c" }, .flags = &.{ "-std=c11", "-Wall", "-Wextra" } });
        protocol_exe.linkLibC();
        protocol_exe.addIncludePath(.{ .cwd_relative = "include" });
        protocol_exe.addLibraryPath(.{ .cwd_relative = "target/debug" });
        protocol_exe.linkSystemLibrary("wry");
        protocol_exe.step.dependOn(&build_rust_dep.step);
        if (dll_install_step) |s| protocol_exe.step.dependOn(s);
        b.installArtifact(protocol_exe);

        // full_features.c 示例
        const full_exe = b.addExecutable(.{
            .name = "full_features",
            .target = target,
            .optimize = optimize,
        });
        full_exe.addCSourceFile(.{ .file = .{ .cwd_relative = "examples/c_api/full_features.c" }, .flags = &.{ "-std=c11", "-Wall", "-Wextra" } });
        full_exe.linkLibC();
        full_exe.addIncludePath(.{ .cwd_relative = "include" });
        full_exe.addLibraryPath(.{ .cwd_relative = "target/debug" });
        full_exe.linkSystemLibrary("wry");
        full_exe.step.dependOn(&build_rust_dep.step);
        if (dll_install_step) |s| full_exe.step.dependOn(s);
        b.installArtifact(full_exe);

        // multiwebview.c 示例
        const multiwebview_exe = b.addExecutable(.{
            .name = "multiwebview",
            .target = target,
            .optimize = optimize,
        });
        multiwebview_exe.addCSourceFile(.{ .file = .{ .cwd_relative = "examples/c_api/multiwebview.c" }, .flags = &.{ "-std=c11", "-Wall", "-Wextra" } });
        multiwebview_exe.linkLibC();
        multiwebview_exe.addIncludePath(.{ .cwd_relative = "include" });
        multiwebview_exe.addLibraryPath(.{ .cwd_relative = "target/debug" });
        multiwebview_exe.linkSystemLibrary("wry");
        multiwebview_exe.step.dependOn(&build_rust_dep.step);
        if (dll_install_step) |s| multiwebview_exe.step.dependOn(s);
        b.installArtifact(multiwebview_exe);

        // 添加构建示例的步骤
        const examples_step = b.step("build-examples", "Build C API examples");
        examples_step.dependOn(&simple_exe.step);
        examples_step.dependOn(&ipc_exe.step);
        examples_step.dependOn(&protocol_exe.step);
        examples_step.dependOn(&full_exe.step);
        examples_step.dependOn(&multiwebview_exe.step);

        // 运行示例的步骤
        const run_simple = b.addRunArtifact(simple_exe);
        const run_ipc = b.addRunArtifact(ipc_exe);
        const run_protocol = b.addRunArtifact(protocol_exe);
        const run_full = b.addRunArtifact(full_exe);
        const run_multiwebview = b.addRunArtifact(multiwebview_exe);

        const run_step = b.step("run", "Run examples");
        run_step.dependOn(&run_simple.step);
        run_step.dependOn(&run_ipc.step);
        run_step.dependOn(&run_protocol.step);
        run_step.dependOn(&run_full.step);
        run_step.dependOn(&run_multiwebview.step);
    }

    // ========== 测试步骤 ==========

    const test_cargo = b.addSystemCommand(&.{ "cargo", "test" });
    if (cabi_feature) {
        test_cargo.addArgs(&.{ "--features", "cabi" });
    }

    const test_step = b.step("test", "Run tests");
    test_step.dependOn(&test_cargo.step);
}
