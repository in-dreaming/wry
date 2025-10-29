# Makefile for Wry C API Examples

# 配置
CXX = gcc
CXXFLAGS = -std=c11 -Wall -Wextra
INCLUDES = -I../../include
LIBDIR = ../../target/release
LIBS = -lwry

# 目标文件
EXAMPLES = simple ipc_example custom_protocol full_features

# 默认目标
all: $(EXAMPLES)

# 通用编译规则
%: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< -L$(LIBDIR) $(LIBS)

# 清理
clean:
	rm -f $(EXAMPLES) $(addsuffix .exe, $(EXAMPLES))

# 安装（将示例复制到路径）
install: $(EXAMPLES)
	@echo "Installing examples..."
	@mkdir -p ~/bin
	@cp $(EXAMPLES) ~/bin/ 2>/dev/null || true

# 运行示例
run-simple: simple
	LD_LIBRARY_PATH=$(LIBDIR) ./simple

run-ipc: ipc_example
	LD_LIBRARY_PATH=$(LIBDIR) ./ipc_example

run-custom: custom_protocol
	LD_LIBRARY_PATH=$(LIBDIR) ./custom_protocol

run-full: full_features
	LD_LIBRARY_PATH=$(LIBDIR) ./full_features

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  all          - Build all examples"
	@echo "  simple       - Build simple example"
	@echo "  ipc_example  - Build IPC example"
	@echo "  custom_protocol - Build custom protocol example"
	@echo "  full_features - Build full features example"
	@echo "  clean        - Remove built files"
	@echo "  help         - Show this help message"

.PHONY: all clean install help run-simple run-ipc run-custom run-full


