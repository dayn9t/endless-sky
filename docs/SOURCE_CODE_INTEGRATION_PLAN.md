# 源代码集成计划

## 需要修改的文件

### 1. main.cpp - 初始化 Locale 系统
```cpp
#include "Locale.h"

int main(int argc, char *argv[]) {
    // ... existing code ...

    // Initialize localization
    Locale::Init("zh_CN");

    // ... rest of main ...
}
```

### 2. TradingPanel.cpp - 示例 UI 文件
- 将硬编码字符串替换为 `Locale::Get("key")`
- 例如: `"Trading"` → `Locale::Get("ui.trading_panel.title")`

### 3. 其他关键 UI 文件
- MapDetailPanel.cpp
- ConversationPanel.cpp
- BankPanel.cpp
- MissionPanel.cpp

## 实施步骤

1. **Phase 1**: 修改 main.cpp 初始化 Locale
2. **Phase 2**: 修改 1-2 个关键 UI 文件作为示例
3. **Phase 3**: 测试构建并验证中文显示
4. **Phase 4**: 根据结果继续修改其他文件

## 需要用户授权

根据 CLAUDE.md 规则，修改源代码需要用户明确授权。

**请确认**: 我可以开始修改上述源文件吗？
