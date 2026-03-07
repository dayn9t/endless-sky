# Endless Sky 中文插件系统设计

**日期**: 2026-03-07
**状态**: 设计完成，待实施

---

## 一、翻译完整度报告

| 指标 | 数值 |
|------|------|
| 总翻译条目 | 20,005 |
| 有效翻译（非空/非模板） | 19,993 |
| 空白/模板变量条目 | 12 |
| 有效翻译率 | **100%** |
| 可精确匹配到源文件 | 20,005 / 20,005 (100%) |
| 涉及源文件数 | 154 个 |
| UI 翻译条目 | 100 条（locales/zh_CN.json） |

### 翻译类型分布

| 类型 | 条目数 | 说明 |
|------|--------|------|
| backtick 文本（对话/任务叙述） | 17,817 | 反引号包裹的段落文本 |
| description 字段 | 2,188 | outfit/ship/planet 描述 |

### 涉及的顶层块类型

| 块类型 | 数量 | overwrite 需求 |
|--------|------|----------------|
| mission | 2,245 | ✅ 必须加 overwrite |
| outfit | 870 | ❌ 无需（支持重复加载） |
| ship | 623 | ❌ 无需（支持重复加载） |
| phrase | 503 | ✅ 必须加 overwrite |
| event | 397 | ❌ 无需 |
| planet | 242 | ❌ 无需（description 字段覆盖） |
| conversation | 51 | ❌ 无需（nodes.clear()） |
| fleet | 40 | ❌ 无需 |

---

## 二、方案分析

### 方案 A：运行时翻译映射（修改 C++ 引擎）

在游戏引擎中加载 `translated_data.json`，在渲染文本时动态查找替换。

**优点**：
- 翻译文件独立，易于更新
- 不需要修改数据文件
- 支持增量更新

**缺点**：
- 需要修改多处 C++ 渲染代码（Conversation、Mission、Planet 等）
- 运行时性能开销（每次渲染都要查找映射）
- 翻译粒度是段落级，可能有边界问题

**评估**：实现复杂，维护成本高。❌ 不推荐

---

### 方案 B：生成完整翻译数据文件（插件覆盖）⭐ 推荐

将 `translated_data.json` 中的翻译注入回原始数据文件，生成翻译后的完整数据文件，放入插件目录。游戏加载时，插件数据覆盖原始数据。

**优点**：
- 无需修改 C++ 代码
- 符合游戏插件规范
- 100% 可精确替换（已验证）
- 插件可独立分发

**缺点**：
- 插件包含 154 个数据文件（约 50MB）
- 原游戏更新后需要重新生成插件

**评估**：实现简单，效果完整。✅ **推荐**

---

### 方案 C：仅覆盖含翻译的块（最小化插件）

只生成包含翻译内容的块，而非完整文件。每个块前加 `overwrite` 关键字。

**优点**：
- 插件体积小（只包含有翻译的块）
- 更新时只需重新生成有变化的块

**缺点**：
- 需要精确解析原始文件，提取完整块结构
- mission 块内嵌 conversation，结构复杂
- 实现难度高于方案 B

**评估**：体积优势有限，实现复杂度高。⚠️ 备选

---

## 三、推荐方案详细设计（方案 B）

### 3.1 插件目录结构

```
plugins/zh_CN/
├── plugin.txt              # 插件元数据
├── data/                   # 翻译后的数据文件（154个）
│   ├── human/
│   │   ├── human missions.txt
│   │   ├── intro missions.txt
│   │   └── ...
│   ├── coalition/
│   │   ├── coalition missions.txt
│   │   └── ...
│   ├── map planets.txt
│   └── ...（共154个文件）
└── locales/
    └── zh_CN.json          # UI 翻译（已有）
```

### 3.2 plugin.txt 内容

```
name "Chinese Localization"
version "1.0.0"
about "Endless Sky 简体中文本地化插件。翻译覆盖所有主线任务、对话、装备和行星描述。"
```

### 3.3 文件生成规则

对每个源文件 `data/<path>/<file>.txt`：

1. 读取原始文件内容
2. 对每条翻译条目：
   - **backtick 文本**：将 `` `原文` `` 替换为 `` `译文` ``
   - **description 字段**：将 `原文` 替换为 `译文`（精确字符串替换）
3. 对 `mission` 和 `phrase` 块，在块定义前插入 `overwrite` 关键字
4. 将结果写入 `plugins/zh_CN/data/<path>/<file>.txt`

### 3.4 overwrite 插入规则

```python
# 在 mission 和 phrase 块前插入 overwrite
content = re.sub(
    r'^(mission|phrase)\s+"',
    r'overwrite\n\1 "',
    content,
    flags=re.MULTILINE
)
```

### 3.5 加载机制

游戏加载顺序（`GameData::LoadSources`）：
1. `resources/data/`（原始英文数据）
2. `plugins/zh_CN/data/`（翻译后数据，后加载覆盖）

各类型覆盖行为：
- `mission`：需要 `overwrite` 关键字（`trueName` 非空时直接 return）
- `phrase`：需要 `overwrite` 关键字（追加模式）
- `conversation`：直接覆盖（`nodes.clear()`）
- `planet`：直接覆盖（`description` 字段在 `shouldOverwrite` 集合中）
- `outfit`/`ship`：直接覆盖（支持重复加载）

### 3.6 安装方式

**开发环境**（全局插件）：
```bash
# 插件放在游戏资源目录
cp -r plugins/zh_CN /path/to/endless-sky/plugins/
```

**用户环境**（用户插件）：
```bash
# Linux
cp -r plugins/zh_CN ~/.local/share/endless-sky/plugins/
# macOS
cp -r plugins/zh_CN ~/Library/Application\ Support/endless-sky/plugins/
# Windows
cp -r plugins/zh_CN %APPDATA%\endless-sky\plugins\
```

---

## 四、实现步骤

### Step 1：在 es-translator 中实现 `generate --type data`

在 `es-translator/src/generator.rs` 中实现 `generate_data()` 函数：

```rust
fn generate_data(input: &str) -> Result<()> {
    // 1. 读取 translated_data.json
    // 2. 按文件分组翻译条目
    // 3. 对每个文件：读取原文 -> 替换 -> 添加 overwrite -> 写入插件目录
    // 4. 生成 plugin.txt
}
```

### Step 2：生成插件

```bash
cd /home/jiang/cc/games/endless-sky
./es-translator/target/release/es-translator generate \
    --type data \
    --input es-translator/translated_data.json
# 输出到 plugins/zh_CN/
```

### Step 3：安装并测试

```bash
# 安装到用户插件目录
cp -r plugins/zh_CN ~/.local/share/endless-sky/plugins/

# 启动游戏验证
./build/endless-sky
```

### Step 4：验证覆盖效果

- 检查任务描述是否显示中文
- 检查对话内容是否显示中文
- 检查装备/飞船描述是否显示中文
- 检查行星描述是否显示中文

---

## 五、已知限制

1. **字体问题**：游戏使用位图字体（`ubuntu14r.png`/`ubuntu18r.png`），需要确认字体包含中文字符集。若不支持，需要替换字体或使用 FreeType 渲染。

2. **游戏更新兼容性**：原游戏数据文件更新后，插件需要重新生成。建议在 CI 中自动化此流程。

3. **部分内容未翻译**：以下内容不在翻译范围内：
   - 系统/星球名称（专有名词，保留英文）
   - 政府名称（保留英文）
   - 飞船/装备名称（保留英文）
   - 游戏内 log 条目（部分）

4. **log 条目**：`mission` 中的 `log` 字段文本未被提取翻译，需要后续补充。
