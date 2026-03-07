# Endless Sky Chinese Localization Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add complete Chinese (Simplified) language support to Endless Sky using AI-powered translation.

**Architecture:** Hybrid approach - Locale class + JSON for UI strings, Plugin system for data files, Rust CLI tool (es-translator) for AI translation via Qwen API.

**Tech Stack:** C++20 (game), Rust (translation tool), JSON (translations), Qwen 3.5 API

---

## Phase 1: AI Translation Tool (es-translator)

### Task 1.1: Initialize Rust Project

**Files:**
- Create: `es-translator/Cargo.toml`
- Create: `es-translator/src/main.rs`

**Step 1: Create project directory and initialize**

```bash
cd /home/jiang/cc/games/endless-sky
mkdir -p es-translator/src
cd es-translator
```

**Step 2: Create Cargo.toml**

```toml
[package]
name = "es-translator"
version = "0.1.0"
edition = "2021"
description = "Endless Sky AI-powered translation tool"

[dependencies]
clap = { version = "4.5", features = ["derive"] }
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
reqwest = { version = "0.12", features = ["json", "rustls-tls"] }
tokio = { version = "1.0", features = ["full"] }
anyhow = "1.0"
regex = "1.10"
walkdir = "2.5"
dotenv = "0.15"
tracing = "0.1"
tracing-subscriber = "0.3"
```

**Step 3: Create main.rs skeleton**

```rust
mod extractor;
mod translator;
mod generator;

use clap::{Parser, Subcommand};

#[derive(Parser)]
#[command(name = "es-translator")]
#[command(about = "Endless Sky AI-powered translation tool", long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Extract translatable text from source files
    Extract {
        /// Source type: ui or data
        #[arg(short, long)]
        source: String,
        /// Output file path
        #[arg(short, long, default_value = "pending.json")]
        output: String,
    },
    /// Translate extracted text using Qwen API
    Translate {
        /// Input file path
        #[arg(short, long)]
        input: String,
        /// Output file path
        #[arg(short, long, default_value = "translated.json")]
        output: String,
    },
    /// Generate translation files
    Generate {
        /// Type: ui or data
        #[arg(short, long)]
        r#type: String,
        /// Input file path
        #[arg(short, long)]
        input: String,
    },
    /// Run full pipeline
    Run {
        /// Translate all (ui + data)
        #[arg(long)]
        all: bool,
    },
}

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    dotenv::dotenv().ok();
    tracing_subscriber::fmt::init();

    let cli = Cli::parse();

    match cli.command {
        Commands::Extract { source, output } => {
            extractor::extract(&source, &output)?;
        }
        Commands::Translate { input, output } => {
            translator::translate(&input, &output).await?;
        }
        Commands::Generate { r#type, input } => {
            generator::generate(&r#type, &input)?;
        }
        Commands::Run { all } => {
            if all {
                // TODO: Full pipeline
            }
        }
    }

    Ok(())
}
```

**Step 4: Build and verify**

```bash
cd es-translator
cargo build
```

Expected: Build succeeds with no errors.

**Step 5: Commit**

```bash
cd /home/jiang/cc/games/endless-sky
git add es-translator/Cargo.toml es-translator/src/main.rs
git commit -m "feat(es-translator): initialize Rust project structure"
```

---

### Task 1.2: Create Data Types Module

**Files:**
- Create: `es-translator/src/types.rs`
- Modify: `es-translator/src/main.rs` (add mod declaration)

**Step 1: Create types.rs**

```rust
use serde::{Deserialize, Serialize};

/// A single translatable item
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TranslateItem {
    /// Unique identifier (e.g., "outfits.show_for_sale")
    pub id: String,
    /// Original English text
    pub text: String,
    /// Context hint for translator
    #[serde(skip_serializing_if = "Option::is_none")]
    pub context: Option<String>,
    /// Source file path
    #[serde(skip_serializing_if = "Option::is_none")]
    pub source: Option<String>,
}

/// Collection of items to translate
#[derive(Debug, Serialize, Deserialize)]
pub struct PendingTranslations {
    pub source: String,  // "ui" or "data"
    pub items: Vec<TranslateItem>,
}

/// Collection of translated items
#[derive(Debug, Serialize, Deserialize)]
pub struct TranslatedItems {
    pub source: String,
    pub items: Vec<TranslatedItem>,
}

/// A translated item
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TranslatedItem {
    pub id: String,
    pub original: String,
    pub translated: String,
}

/// Qwen API request
#[derive(Debug, Serialize)]
pub struct QwenRequest {
    pub model: String,
    pub messages: Vec<Message>,
}

#[derive(Debug, Serialize)]
pub struct Message {
    pub role: String,
    pub content: String,
}

/// Qwen API response
#[derive(Debug, Deserialize)]
pub struct QwenResponse {
    pub choices: Vec<QwenChoice>,
}

#[derive(Debug, Deserialize)]
pub struct QwenChoice {
    pub message: QwenMessage,
}

#[derive(Debug, Deserialize)]
pub struct QwenMessage {
    pub content: String,
}
```

**Step 2: Add mod declaration to main.rs**

Add at the top of main.rs:
```rust
mod types;
```

**Step 3: Build and verify**

```bash
cargo build
```

Expected: Build succeeds.

**Step 4: Commit**

```bash
git add es-translator/src/types.rs es-translator/src/main.rs
git commit -m "feat(es-translator): add data types for translation"
```

---

### Task 1.3: Implement Text Extractor (UI Strings)

**Files:**
- Create: `es-translator/src/extractor.rs`

**Step 1: Create extractor.rs**

```rust
use crate::types::{PendingTranslations, TranslateItem};
use anyhow::Result;
use regex::Regex;
use std::fs;
use std::path::Path;
use walkdir::WalkDir;

/// Extract translatable text from source
pub fn extract(source: &str, output: &str) -> Result<()> {
    match source {
        "ui" => extract_ui(output),
        "data" => extract_data(output),
        _ => anyhow::bail!("Unknown source type: {}", source),
    }
}

/// Extract UI strings from C++ source files
fn extract_ui(output: &str) -> Result<()> {
    let source_dir = "source";
    let mut items = Vec::new();

    // Pattern to match string literals in Draw calls
    // font.Draw("string", ...) or similar patterns
    let string_pattern = Regex::new(r#"(font|Font)\.Draw\s*\(\s*"([^"]+)""#)?;
    let return_pattern = Regex::new(r#"return\s+"([^"]+)""#)?;
    let error_pattern = Regex::new(r#"(error|Error|ERROR)[^"]*"([^"]+)""#)?;

    for entry in WalkDir::new(source_dir)
        .into_iter()
        .filter_map(|e| e.ok())
        .filter(|e| e.path().extension().map_or(false, |ext| ext == "cpp"))
    {
        let path = entry.path();
        let content = fs::read_to_string(path)?;
        let relative_path = path.strip_prefix(source_dir)?.to_string_lossy();

        // Extract font.Draw strings
        for cap in string_pattern.captures_iter(&content) {
            let text = cap[2].to_string();
            if should_translate(&text) {
                items.push(TranslateItem {
                    id: generate_id(&relative_path, &text),
                    text,
                    context: Some(format!("UI text in {}", relative_path)),
                    source: Some(relative_path.to_string()),
                });
            }
        }

        // Extract return strings (often error messages)
        for cap in return_pattern.captures_iter(&content) {
            let text = cap[1].to_string();
            if should_translate(&text) {
                items.push(TranslateItem {
                    id: generate_id(&relative_path, &text),
                    text,
                    context: Some(format!("Return value in {}", relative_path)),
                    source: Some(relative_path.to_string()),
                });
            }
        }
    }

    // Deduplicate by id
    items.sort_by(|a, b| a.id.cmp(&b.id));
    items.dedup_by(|a, b| a.id == b.id);

    let pending = PendingTranslations {
        source: "ui".to_string(),
        items,
    };

    let json = serde_json::to_string_pretty(&pending)?;
    fs::write(output, json)?;

    println!("Extracted {} UI strings to {}", pending.items.len(), output);
    Ok(())
}

/// Extract text from data files (placeholder - see Task 1.4)
fn extract_data(_output: &str) -> Result<()> {
    anyhow::bail!("Data extraction not yet implemented")
}

/// Check if text should be translated
fn should_translate(text: &str) -> bool {
    // Skip empty strings
    if text.is_empty() {
        return false;
    }
    // Skip single characters
    if text.len() == 1 {
        return false;
    }
    // Skip pure numbers
    if text.chars().all(|c| c.is_numeric() || c == '.' || c == '-') {
        return false;
    }
    // Skip file paths
    if text.contains('/') || text.contains('\\') {
        return false;
    }
    true
}

/// Generate a unique ID for a translation item
fn generate_id(file: &str, text: &str) -> String {
    // Use first 30 chars of text, sanitized
    let sanitized: String = text
        .chars()
        .take(30)
        .filter(|c| c.is_alphanumeric() || *c == ' ')
        .collect::<String>()
        .to_lowercase()
        .replace(' ', "_");

    let file_part = file.replace('/', "_").replace(".cpp", "");

    format!("{}.{}", file_part, sanitized)
}
```

**Step 2: Build and verify**

```bash
cargo build
```

Expected: Build succeeds.

**Step 3: Test extraction**

```bash
cd /home/jiang/cc/games/endless-sky
cargo run --extract --source ui --output pending_ui.json
```

Expected: Creates `pending_ui.json` with extracted UI strings.

**Step 4: Commit**

```bash
git add es-translator/src/extractor.rs
git commit -m "feat(es-translator): implement UI string extractor"
```

---

### Task 1.4: Implement Data File Extractor

**Files:**
- Modify: `es-translator/src/extractor.rs`

**Step 1: Add data extraction function to extractor.rs**

Add this function to extractor.rs:

```rust
/// Extract text from game data files
fn extract_data(output: &str) -> Result<()> {
    let data_dir = "data";
    let mut items = Vec::new();

    // Pattern for backtick-quoted text in data files
    // `This is mission text`
    let backtick_pattern = Regex::new(r#"`([^`]+)`"#)?;

    // Pattern for display name
    let display_name_pattern = Regex::new(r#"display name\s+"([^"]+)""#)?;

    // Pattern for description
    let description_pattern = Regex::new(r#"description\s+"([^"]+)""#)?;

    for entry in WalkDir::new(data_dir)
        .into_iter()
        .filter_map(|e| e.ok())
        .filter(|e| e.path().extension().map_or(false, |ext| ext == "txt"))
    {
        let path = entry.path();
        let content = fs::read_to_string(path)?;
        let relative_path = path.strip_prefix(data_dir)?.to_string_lossy();

        // Extract backtick-quoted text (dialog, descriptions)
        for cap in backtick_pattern.captures_iter(&content) {
            let text = cap[1].to_string();
            if should_translate_data(&text) {
                items.push(TranslateItem {
                    id: format!("data.{}.{}", relative_path.replace('/', "."), items.len()),
                    text,
                    context: Some(format!("Data file: {}", relative_path)),
                    source: Some(format!("data/{}", relative_path)),
                });
            }
        }

        // Extract display names
        for cap in display_name_pattern.captures_iter(&content) {
            let text = cap[1].to_string();
            if should_translate_data(&text) {
                items.push(TranslateItem {
                    id: format!("data.{}.display.{}", relative_path.replace('/', "."), items.len()),
                    text,
                    context: Some(format!("Display name in {}", relative_path)),
                    source: Some(format!("data/{}", relative_path)),
                });
            }
        }

        // Extract descriptions
        for cap in description_pattern.captures_iter(&content) {
            let text = cap[1].to_string();
            if should_translate_data(&text) {
                items.push(TranslateItem {
                    id: format!("data.{}.desc.{}", relative_path.replace('/', "."), items.len()),
                    text,
                    context: Some(format!("Description in {}", relative_path)),
                    source: Some(format!("data/{}", relative_path)),
                });
            }
        }
    }

    // Deduplicate
    items.sort_by(|a, b| a.text.cmp(&b.text));
    items.dedup_by(|a, b| a.text == b.text);

    let pending = PendingTranslations {
        source: "data".to_string(),
        items,
    };

    let json = serde_json::to_string_pretty(&pending)?;
    fs::write(output, json)?;

    println!("Extracted {} data strings to {}", pending.items.len(), output);
    Ok(())
}

/// Check if data text should be translated
fn should_translate_data(text: &str) -> bool {
    if text.is_empty() {
        return false;
    }
    // Skip very short strings
    if text.len() < 3 {
        return false;
    }
    // Skip pure numbers or code
    if text.chars().all(|c| c.is_numeric() || c == '.' || c == '-') {
        return false;
    }
    // Skip condition syntax
    if text.starts_with('<') && text.ends_with('>') {
        return false;
    }
    true
}
```

**Step 2: Build and verify**

```bash
cargo build
```

Expected: Build succeeds.

**Step 3: Test data extraction**

```bash
cd /home/jiang/cc/games/endless-sky
cargo run --extract --source data --output pending_data.json
```

Expected: Creates `pending_data.json` with extracted data strings.

**Step 4: Commit**

```bash
git add es-translator/src/extractor.rs
git commit -m "feat(es-translator): implement data file text extractor"
```

---

### Task 1.5: Implement Qwen API Translator

**Files:**
- Create: `es-translator/src/translator.rs`

**Step 1: Create translator.rs**

```rust
use crate::types::{PendingTranslations, QwenRequest, QwenResponse, TranslatedItem, TranslatedItems, TranslateItem, Message};
use anyhow::{Context, Result};
use reqwest::Client;
use serde_json;
use std::fs;
use std::env;

const SYSTEM_PROMPT: &str = r#"你是 Endless Sky 游戏的专业翻译。

## 游戏背景
Endless Sky 是一款开源太空探索游戏。玩家是飞船船长，在银河系中贸易、战斗、探索。
人类分裂为多个派系：Republic（共和国）、Syndicate（辛迪加）、Free Worlds（自由世界）等。
还有外星种族：Hai（海伊）、Korath（科拉斯）、Quarg（夸格）等。

## 术语表
| 英文 | 中文 | 说明 |
|------|------|------|
| ship | 飞船 | |
| outfit | 装备 | 武器、引擎等可安装物品 |
| planet | 行星 | |
| system | 星系 | 包含行星的空间 |
| hyperdrive | 跃迁引擎 | 星际航行设备 |
| jump drive | 跳跃引擎 | 更高级的星际引擎 |
| credits | 积分 | 货币单位 |
| cargo | 货物 | |
| mission | 任务 | |
| hail | 呼叫 | 与其他飞船通讯 |
| outfitting | 装备店 | |
| shipyard | 船坞 | |
| spaceport | 太空港 | |
| crew | 船员 | |

## 翻译规则
1. 保持科幻游戏的语感和风格
2. 专有名词使用术语表中的翻译
3. 保留 {0}, {1}, <player> 等占位符不变
4. 对话保持角色性格和语气
5. UI 文本简洁明了
6. 不要翻译 JSON 的 key，只翻译 text 字段的值"#;

const BATCH_SIZE: usize = 20;

/// Translate extracted text using Qwen API
pub async fn translate(input: &str, output: &str) -> Result<()> {
    let api_key = env::var("DASHSCOPE_API_KEY")
        .context("DASHSCOPE_API_KEY not found in environment")?;

    let base_url = env::var("DASHSCOPE_BASE_URL")
        .unwrap_or_else(|_| "https://dashscope.aliyuncs.com/compatible-mode/v1".to_string());

    let client = Client::new();

    // Load pending translations
    let content = fs::read_to_string(input)?;
    let pending: PendingTranslations = serde_json::from_str(&content)?;

    println!("Translating {} items...", pending.items.len());

    let mut translated_items = Vec::new();

    // Process in batches
    for (batch_num, chunk) in pending.items.chunks(BATCH_SIZE).enumerate() {
        println!("Processing batch {} of {}...",
            batch_num + 1,
            (pending.items.len() + BATCH_SIZE - 1) / BATCH_SIZE);

        let batch_result = translate_batch(&client, &api_key, &base_url, chunk).await?;
        translated_items.extend(batch_result);

        // Small delay to avoid rate limiting
        tokio::time::sleep(tokio::time::Duration::from_millis(500)).await;
    }

    let translated = TranslatedItems {
        source: pending.source,
        items: translated_items,
    };

    let json = serde_json::to_string_pretty(&translated)?;
    fs::write(output, json)?;

    println!("Translation complete. Saved to {}", output);
    Ok(())
}

async fn translate_batch(
    client: &Client,
    api_key: &str,
    base_url: &str,
    items: &[TranslateItem],
) -> Result<Vec<TranslatedItem>> {
    // Build the prompt with items to translate
    let items_json = serde_json::to_string_pretty(
        &items.iter().map(|i| serde_json::json!({
            "id": i.id,
            "text": i.text,
            "context": i.context
        })).collect::<Vec<_>>()
    )?;

    let user_prompt = format!(
        "请将以下游戏文本翻译为简体中文。保持 JSON 数组格式，返回格式为：\n\
         [{{\"id\": \"原id\", \"translated\": \"中文翻译\"}}]\n\n{}\n\n\
         只返回 JSON 数组，不要有其他说明文字。",
        items_json
    );

    let request = QwenRequest {
        model: "qwen-plus".to_string(),
        messages: vec![
            Message {
                role: "system".to_string(),
                content: SYSTEM_PROMPT.to_string(),
            },
            Message {
                role: "user".to_string(),
                content: user_prompt,
            },
        ],
    };

    let response = client
        .post(format!("{}/chat/completions", base_url))
        .header("Authorization", format!("Bearer {}", api_key))
        .header("Content-Type", "application/json")
        .json(&request)
        .send()
        .await?
        .error_for_status()?;

    let qwen_response: QwenResponse = response.json().await?;

    let content = qwen_response
        .choices
        .first()
        .map(|c| c.message.content.as_str())
        .context("No response from API")?;

    // Parse the response
    let parsed: Vec<serde_json::Value> = serde_json::from_str(
        &content.trim_matches(|c| c == '`').replace("json\n", "")
    )?;

    let mut results = Vec::new();
    for item in items {
        let translated_text = parsed
            .iter()
            .find(|p| p["id"].as_str() == Some(&item.id))
            .and_then(|p| p["translated"].as_str())
            .unwrap_or(&item.text)
            .to_string();

        results.push(TranslatedItem {
            id: item.id.clone(),
            original: item.text.clone(),
            translated: translated_text,
        });
    }

    Ok(results)
}
```

**Step 2: Build and verify**

```bash
cargo build
```

Expected: Build succeeds.

**Step 3: Test translation (requires API key)**

```bash
cd /home/jiang/cc/games/endless-sky
source ~/cc/.env
cargo run -- translate --input pending_ui.json --output translated_ui.json
```

Expected: Creates `translated_ui.json` with Chinese translations.

**Step 4: Commit**

```bash
git add es-translator/src/translator.rs
git commit -m "feat(es-translator): implement Qwen API translator"
```

---

### Task 1.6: Implement Translation File Generator

**Files:**
- Create: `es-translator/src/generator.rs`

**Step 1: Create generator.rs**

```rust
use crate::types::TranslatedItems;
use anyhow::Result;
use std::collections::HashMap;
use std::fs;
use std::path::Path;

/// Generate translation files from translated items
pub fn generate(type_arg: &str, input: &str) -> Result<()> {
    match type_arg {
        "ui" => generate_ui(input),
        "data" => generate_data(input),
        _ => anyhow::bail!("Unknown type: {}", type_arg),
    }
}

/// Generate locales/zh_CN.json for UI translations
fn generate_ui(input: &str) -> Result<()> {
    let content = fs::read_to_string(input)?;
    let translated: TranslatedItems = serde_json::from_str(&content)?;

    // Build nested JSON structure
    let mut locale: HashMap<String, serde_json::Value> = HashMap::new();

    for item in &translated.items {
        // Parse the id to create nested structure
        // e.g., "OutfitterPanel.show_for_sale" -> {"outfitter_panel": {"show_for_sale": "翻译"}}
        let parts: Vec<&str> = item.id.split('.').collect();
        if parts.len() >= 2 {
            let category = parts[0].to_lowercase();
            let key = parts[1..].join(".");

            let entry = locale.entry(category).or_insert_with(|| {
                serde_json::json!({})
            });

            if let Some(obj) = entry.as_object_mut() {
                obj.insert(key, serde_json::Value::String(item.translated.clone()));
            }
        }
    }

    // Create locales directory
    fs::create_dir_all("locales")?;

    let json = serde_json::to_string_pretty(&locale)?;
    fs::write("locales/zh_CN.json", json)?;

    println!("Generated locales/zh_CN.json with {} translations", translated.items.len());
    Ok(())
}

/// Generate plugins/chinese/data/ for data translations
fn generate_data(input: &str) -> Result<()> {
    let content = fs::read_to_string(input)?;
    let translated: TranslatedItems = serde_json::from_str(&content)?;

    // Group by source file
    let mut by_file: HashMap<String, Vec<_>> = HashMap::new();
    for item in &translated.items {
        if let Some(source) = &item.source {
            by_file.entry(source.clone()).or_default().push(item);
        }
    }

    // Create plugin directory structure
    fs::create_dir_all("plugins/chinese/data")?;

    // Write about.txt
    let about = r#"name: "简体中文语言包"
version: "1.0.0"
description: "Endless Sky 简体中文翻译插件，由 AI 自动翻译生成。"
author: "ES Translator""#;
    fs::write("plugins/chinese/about.txt", about)?;

    // Note: For data files, we need to actually modify the original files
    // This is a placeholder that creates a translation mapping
    let mapping_path = "plugins/chinese/translation_mapping.json";
    let mapping = serde_json::to_string_pretty(&serde_json::json!({
        "note": "This mapping shows what needs to be translated in each file",
        "files": by_file.iter().map(|(k, v)| {
            (k.clone(), v.iter().map(|i| serde_json::json!({
                "original": i.original,
                "translated": i.translated
            })).collect::<Vec<_>>())
        }).collect::<HashMap<_, _>>()
    }))?;
    fs::write(mapping_path, mapping)?;

    println!("Generated plugins/chinese/ with translation mapping");
    println!("Note: Data file translation requires manual integration or additional tooling");
    Ok(())
}
```

**Step 2: Build and verify**

```bash
cargo build
```

Expected: Build succeeds.

**Step 3: Test generation**

```bash
cd /home/jiang/cc/games/endless-sky
cargo run -- generate --type ui --input translated_ui.json
```

Expected: Creates `locales/zh_CN.json`.

**Step 4: Commit**

```bash
git add es-translator/src/generator.rs
git commit -m "feat(es-translator): implement translation file generator"
```

---

### Task 1.7: Add Prompts Directory

**Files:**
- Create: `es-translator/prompts/system.md`

**Step 1: Create prompts directory and system.md**

```bash
mkdir -p es-translator/prompts
```

**Step 2: Create prompts/system.md**

```markdown
# Endless Sky Translation Context

## Game Overview

Endless Sky is a free, open-source 2D space exploration, trading, and combat game.
Players start as captains of a small starship and can choose their own path:
- Trading goods between star systems
- Completing missions for various factions
- Fighting pirates or becoming one
- Exploring the galaxy and discovering alien civilizations

## Factions

### Human Factions
- **Republic**: The dominant human government, bureaucratic but stable
- **Syndicate**: Corporate-controlled worlds, profit-driven
- **Free Worlds**: Alliance of independent worlds seeking freedom
- **Frontier**: Remote systems on the edge of human space

### Alien Races
- **Hai**: Ancient alien civilization with advanced technology
- **Korath**: Aggressive alien species with powerful warships
- **Quarg**: Mysterious beings of immense power
- **Coalition**: Alliance of alien species
- **Drak**: Enigmatic and rarely seen

## Common Terminology

| English | Chinese | Notes |
|---------|---------|-------|
| ship | 飞船 | Player's vessel |
| outfit | 装备 | Installable items (weapons, engines, etc.) |
| planet | 行星 | Landable celestial body |
| system | 星系 | Contains multiple planets |
| hyperspace | 超空间 | Dimension for faster-than-light travel |
| hyperdrive | 跃迁引擎 | FTL propulsion system |
| jump drive | 跳跃引擎 | Advanced FTL system |
| jump gate | 跳跃门 | Fixed FTL infrastructure |
| credits | 积分 | Currency |
| cargo | 货物 | Trade goods |
| mission | 任务 | Jobs from NPCs |
| hail | 呼叫 | Communicate with other ships |
| boarding | 登船 | Capturing another ship |
| escort | 护卫舰 | Ships following the player |
| fleet | 舰队 | Group of ships |
| outfitter | 装备店 | Shop for outfits |
| shipyard | 船坞 | Shop for ships |
| spaceport | 太空港 | Landing facility |
| hiring | 招聘 | Hiring crew members |
| bank | 银行 | Financial services |

## Tone and Style

- **Sci-fi atmosphere**: Use futuristic terminology
- **Professional but accessible**: Not too formal, not too casual
- **Character consistency**: Maintain distinct voices for different NPCs
- **Concise UI text**: Buttons and labels should be short and clear
- **Preserve formatting**: Keep line breaks, placeholders like {0}

## Translation Rules

1. Do not translate proper nouns unless there's an established translation
2. Keep placeholders like `{0}`, `{1}`, `<player>` unchanged
3. Maintain the same punctuation style where appropriate
4. For mission titles and descriptions, match the dramatic tone
5. UI elements should use consistent terminology throughout
```

**Step 3: Commit**

```bash
git add es-translator/prompts/
git commit -m "docs(es-translator): add translation context and prompts"
```

---

## Phase 2: Source Code i18n (Locale Class)

### Task 2.1: Create Locale Header

**Files:**
- Create: `source/Locale.h`

**Step 1: Create Locale.h**

```cpp
/* Locale.h
Copyright (c) 2025

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.
*/

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

// Localization system for supporting multiple languages.
// Loads translations from JSON files and provides lookup by key.
class Locale {
public:
	// Initialize the locale system with the specified language code.
	// Falls back to "en" if the language file is not found.
	static void Init(const std::string &lang = "en");

	// Get the current language code.
	static const std::string &CurrentLanguage();

	// Get a translated string by key. Returns the key if not found.
	static std::string_view Get(std::string_view key);

	// Check if a translation exists for the given key.
	static bool Has(std::string_view key);

	// Format a translated string with placeholders.
	// Usage: Locale::Format("key", arg1, arg2)
	template<typename... Args>
	static std::string Format(std::string_view key, Args&&... args);

	// Check if the locale system is initialized.
	static bool IsInitialized();

private:
	static std::string currentLang;
	static std::unordered_map<std::string, std::string> translations;
	static bool initialized;

	// Helper to replace placeholders in format string.
	static std::string FormatImpl(std::string_view text,
		const std::vector<std::string> &args);
};


// Template implementation
template<typename... Args>
std::string Locale::Format(std::string_view key, Args&&... args)
{
	std::string text(translations.contains(std::string(key))
		? translations.at(std::string(key))
		: std::string(key));

	std::vector<std::string> argList;
	(argList.push_back(std::forward<Args>(args)), ...);

	return FormatImpl(text, argList);
}
```

**Step 2: Commit**

```bash
git add source/Locale.h
git commit -m "feat(locale): add Locale class header for i18n support"
```

---

### Task 2.2: Create Locale Implementation

**Files:**
- Create: `source/Locale.cpp`

**Step 1: Create Locale.cpp**

```cpp
/* Locale.cpp
Copyright (c) 2025

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.
*/

#include "Locale.h"

#include "Files.h"
#include "Logger.h"

#include <algorithm>
#include <regex>
#include <sstream>

using namespace std;

string Locale::currentLang = "en";
unordered_map<string, string> Locale::translations;
bool Locale::initialized = false;



void Locale::Init(const string &lang)
{
	currentLang = lang;
	translations.clear();

	// Try to load the language file
	string path = Files::Resources() + "locales/" + lang + ".json";

	if(!Files::Exists(path))
	{
		// Try game data directory as fallback
		path = Files::Data() + "locales/" + lang + ".json";
	}

	if(!Files::Exists(path))
	{
		Logger::Get().Warn("Locale file not found for language:", lang);
		initialized = true;
		return;
	}

	string content = Files::Read(path);

	// Parse JSON manually (avoiding external JSON library dependency)
	// This is a simple parser that handles our flat key-value structure
	ParseJSON(content);

	initialized = true;
	Logger::Get().Info("Loaded", translations.size(), "translations for", lang);
}



const string &Locale::CurrentLanguage()
{
	return currentLang;
}



string_view Locale::Get(string_view key)
{
	string keyStr(key);
	if(translations.contains(keyStr))
		return translations.at(keyStr);

	// Return the key itself as fallback
	return key;
}



bool Locale::Has(string_view key)
{
	return translations.contains(string(key));
}



bool Locale::IsInitialized()
{
	return initialized;
}



string Locale::FormatImpl(string_view text, const vector<string> &args)
{
	string result(text);

	// Replace {0}, {1}, etc. with actual values
	for(size_t i = 0; i < args.size(); ++i)
	{
		string placeholder = "{" + to_string(i) + "}";
		size_t pos = 0;
		while((pos = result.find(placeholder, pos)) != string::npos)
		{
			result.replace(pos, placeholder.length(), args[i]);
			pos += args[i].length();
		}
	}

	return result;
}



// Simple JSON parser for flat key-value structure
void Locale::ParseJSON(const string &content)
{
	// This is a simplified parser for our JSON format:
	// { "category": { "key": "value" } }

	enum class State {
		ROOT,
		CATEGORY,
		KEY,
		VALUE
	};

	State state = State::ROOT;
	string currentCategory;
	string currentKey;
	string currentValue;
	bool inString = false;
	bool escaping = false;

	for(size_t i = 0; i < content.length(); ++i)
	{
		char c = content[i];

		if(escaping)
		{
			switch(c)
			{
				case 'n': currentValue += '\n'; break;
				case 't': currentValue += '\t'; break;
				case '"': currentValue += '"'; break;
				case '\\': currentValue += '\\'; break;
				default: currentValue += c; break;
			}
			escaping = false;
			continue;
		}

		if(c == '\\' && inString)
		{
			escaping = true;
			continue;
		}

		if(c == '"')
		{
			inString = !inString;
			continue;
		}

		if(inString)
		{
			if(state == State::KEY)
				currentKey += c;
			else if(state == State::VALUE)
				currentValue += c;
			continue;
		}

		// Not in string
		switch(c)
		{
			case '{':
				if(state == State::ROOT)
					state = State::CATEGORY;
				else if(state == State::CATEGORY)
					state = State::KEY;
				break;
			case '}':
				if(state == State::VALUE && !currentKey.empty() && !currentValue.empty())
				{
					string fullKey = currentCategory.empty()
						? currentKey
						: currentCategory + "." + currentKey;
					translations[fullKey] = currentValue;
					currentKey.clear();
					currentValue.clear();
				}
				state = State::CATEGORY;
				break;
			case ':':
				if(state == State::KEY)
					state = State::VALUE;
				break;
			case ',':
				if(state == State::VALUE && !currentKey.empty() && !currentValue.empty())
				{
					string fullKey = currentCategory.empty()
						? currentKey
						: currentCategory + "." + currentKey;
					translations[fullKey] = currentValue;
					currentKey.clear();
					currentValue.clear();
				}
				state = State::KEY;
				break;
		}
	}
}
```

**Step 2: Add declaration to header**

Add this private method declaration to Locale.h:

```cpp
private:
	// Parse JSON translation file.
	static void ParseJSON(const std::string &content);
```

**Step 3: Commit**

```bash
git add source/Locale.h source/Locale.cpp
git commit -m "feat(locale): implement Locale class with JSON parsing"
```

---

### Task 2.3: Update CMakeLists.txt

**Files:**
- Modify: `source/CMakeLists.txt`

**Step 1: Add Locale.cpp to source list**

Find the source file list in `source/CMakeLists.txt` and add `Locale.cpp` to it.

```cmake
# Add to the source file list
Locale.cpp
```

**Step 2: Build and verify**

```bash
cd /home/jiang/cc/games/endless-sky
cmake --build build
```

Expected: Build succeeds with Locale.cpp compiled.

**Step 3: Commit**

```bash
git add source/CMakeLists.txt
git commit -m "build: add Locale.cpp to build system"
```

---

## Phase 3: es-translate Skill

### Task 3.1: Create Skill Directory

**Files:**
- Create: `~/.claude/skills/es-translate/SKILL.md`

**Step 1: Create skill directory**

```bash
mkdir -p ~/.claude/skills/es-translate
```

**Step 2: Create SKILL.md**

```markdown
---
name: es-translate
description: Endless Sky translation workflow - extract, translate, and generate Chinese content using Qwen AI
---

# Endless Sky Translation Skill

Automates the translation of Endless Sky game content from English to Simplified Chinese using Qwen 3.5 AI.

## Triggers

- "翻译 endless sky"
- "更新 endless sky 中文翻译"
- "运行 es-translator"
- "/es-translate"

## Environment Requirements

1. **Qwen API Key**: Must be set in `~/cc/.env`
   ```bash
   export DASHSCOPE_API_KEY="your-api-key"
   export DASHSCOPE_BASE_URL="https://dashscope.aliyuncs.com/compatible-mode/v1"
   ```

2. **es-translator tool**: Must be built in `es-translator/`

## Workflow

### Full Translation Pipeline

```bash
# 1. Source environment
source ~/cc/.env

# 2. Navigate to project
cd /home/jiang/cc/games/endless-sky

# 3. Build tool (if needed)
cd es-translator && cargo build --release && cd ..

# 4. Extract UI strings
./es-translator/target/release/es-translator extract --source ui --output pending_ui.json

# 5. Extract data file text
./es-translator/target/release/es-translator extract --source data --output pending_data.json

# 6. Translate UI (uses Qwen API)
./es-translator/target/release/es-translator translate --input pending_ui.json --output translated_ui.json

# 7. Translate data
./es-translator/target/release/es-translator translate --input pending_data.json --output translated_data.json

# 8. Generate locale file
./es-translator/target/release/es-translator generate --type ui --input translated_ui.json

# 9. Generate plugin data
./es-translator/target/release/es-translator generate --type data --input translated_data.json
```

### Incremental Update

```bash
# Only translate new/changed strings
source ~/cc/.env
cd /home/jiang/cc/games/endless-sky

./es-translator/target/release/es-translator extract --source ui --output pending_ui.json
# Compare with previous translation and only translate new items
./es-translator/target/release/es-translator translate --input pending_ui.json --output translated_ui.json
./es-translator/target/release/es-translator generate --type ui --input translated_ui.json
```

## Output Locations

| Type | Output Path |
|------|-------------|
| UI translations | `locales/zh_CN.json` |
| Data translations | `plugins/chinese/data/` |
| Plugin metadata | `plugins/chinese/about.txt` |
| Translation cache | `.es-translator/cache/` |

## Verification

After translation:

```bash
# Build the game
cmake --build build

# Run and verify translations display correctly
./build/EndlessSky
```

In-game, enable the Chinese plugin:
1. Open Settings
2. Enable the "简体中文语言包" plugin
3. Restart game

## Troubleshooting

| Issue | Solution |
|-------|----------|
| API key error | Check `DASHSCOPE_API_KEY` in `~/cc/.env` |
| Translation quality | Edit `es-translator/prompts/system.md` to add context |
| Missing strings | Check extractor patterns in `extractor.rs` |
| Build errors | Run `cargo build` in `es-translator/` |

## Files Modified by This Skill

- `locales/zh_CN.json` - UI translations
- `plugins/chinese/` - Data file translations
- `pending_*.json` - Intermediate extraction files
- `translated_*.json` - Intermediate translation files
```

**Step 3: Verify skill is available**

```bash
ls -la ~/.claude/skills/es-translate/
```

Expected: Shows SKILL.md file.

**Step 4: Commit**

```bash
# Skill is in user directory, not project - skip git commit
echo "Skill created at ~/.claude/skills/es-translate/SKILL.md"
```

---

## Phase 4: Create Initial Translation Files

### Task 4.1: Create Locales Directory Structure

**Files:**
- Create: `locales/en.json` (placeholder)
- Create: `locales/zh_CN.json` (will be generated)

**Step 1: Create directory**

```bash
mkdir -p /home/jiang/cc/games/endless-sky/locales
```

**Step 2: Create placeholder en.json**

```json
{
  "_comment": "Default English locale - keys are returned as-is",
  "_version": "1.0.0"
}
```

**Step 3: Commit**

```bash
git add locales/
git commit -m "feat: add locales directory structure"
```

---

### Task 4.2: Create Chinese Plugin Structure

**Files:**
- Create: `plugins/chinese/about.txt`

**Step 1: Create directory**

```bash
mkdir -p /home/jiang/cc/games/endless-sky/plugins/chinese/data
```

**Step 2: Create about.txt**

```
name: "简体中文语言包"
version: "1.0.0"
description: "Endless Sky 简体中文翻译插件。包含游戏 UI 和内容的完整中文翻译。"
author: "ES Translator (AI-powered)"
homepage: "https://github.com/endless-sky/endless-sky"
```

**Step 3: Commit**

```bash
git add plugins/chinese/
git commit -m "feat: add Chinese localization plugin structure"
```

---

## Summary

### Implementation Order

1. **Phase 1**: es-translator tool (Tasks 1.1-1.7)
2. **Phase 2**: Locale class in game code (Tasks 2.1-2.3)
3. **Phase 3**: Claude Code skill (Task 3.1)
4. **Phase 4**: Initial file structure (Tasks 4.1-4.2)

### Key Files Created

```
es-translator/
├── Cargo.toml
├── src/
│   ├── main.rs
│   ├── types.rs
│   ├── extractor.rs
│   ├── translator.rs
│   └── generator.rs
└── prompts/
    └── system.md

source/
├── Locale.h
└── Locale.cpp

locales/
├── en.json
└── zh_CN.json (generated)

plugins/chinese/
├── about.txt
└── data/ (generated)

~/.claude/skills/es-translate/
└── SKILL.md
```

### Testing Checklist

- [ ] es-translator builds successfully
- [ ] UI text extraction works
- [ ] Data text extraction works
- [ ] Qwen API translation works
- [ ] Locale file generation works
- [ ] Game builds with Locale class
- [ ] Skill is available in Claude Code
