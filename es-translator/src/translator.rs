use crate::types::{PendingTranslations, QwenRequest, QwenResponse, TranslatedItem, TranslatedItems, TranslateItem, Message};
use anyhow::{Context, Result};
use reqwest::Client;
use serde_json;
use std::fs;
use std::env;
use std::time::Duration;

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
3. **重要：保留所有 <...> 和 {...} 占位符原样不变，包括 <planet>, <cargo>, <npc>, <payment>, {0}, {1} 等，不要添加任何空格**
4. 对话保持角色性格和语气
5. UI 文本简洁明了
6. 不要翻译 JSON 的 key，只翻译 text 字段的值"#;

const BATCH_SIZE: usize = 5;
const MAX_RETRIES: u32 = 3;
const CHECKPOINT_INTERVAL: usize = 50; // Save checkpoint every 50 batches

/// Translate extracted text using Qwen API with retry and checkpoint support
pub async fn translate(input: &str, output: &str) -> Result<()> {
    // Ensure output directory exists
    if let Some(parent) = std::path::Path::new(output).parent() {
        std::fs::create_dir_all(parent)?;
    }

    let api_key = env::var("DASHSCOPE_API_KEY")
        .context("DASHSCOPE_API_KEY not found in environment")?;

    let base_url = env::var("DASHSCOPE_BASE_URL")
        .unwrap_or_else(|_| "https://dashscope.aliyuncs.com/compatible-mode/v1".to_string());

    let client = Client::builder()
        .timeout(Duration::from_secs(120))
        .build()?;

    // Load pending translations
    let content = fs::read_to_string(input)?;
    let pending: PendingTranslations = serde_json::from_str(&content)?;

    println!("Translating {} items...", pending.items.len());

    // Try to load checkpoint
    let checkpoint_file = format!("{}.checkpoint", output);
    let mut translated_items = load_checkpoint(&checkpoint_file).unwrap_or_default();

    let start_batch = translated_items.len() / BATCH_SIZE;
    if start_batch > 0 {
        println!("Resuming from checkpoint: {} items already translated", translated_items.len());
    }

    // Process in batches
    let total_batches = (pending.items.len() + BATCH_SIZE - 1) / BATCH_SIZE;
    for (batch_num, chunk) in pending.items.chunks(BATCH_SIZE).enumerate().skip(start_batch) {
        println!("Processing batch {} of {}...", batch_num + 1, total_batches);

        // Retry logic
        let batch_result = translate_batch_with_retry(&client, &api_key, &base_url, chunk).await?;
        translated_items.extend(batch_result);

        // Save checkpoint periodically
        if (batch_num + 1) % CHECKPOINT_INTERVAL == 0 {
            save_checkpoint(&checkpoint_file, &pending.source, &translated_items)?;
            println!("Checkpoint saved: {} items", translated_items.len());
        }

        // Small delay to avoid rate limiting
        tokio::time::sleep(Duration::from_millis(1000)).await;
    }

    let translated = TranslatedItems {
        source: pending.source,
        items: translated_items,
    };

    let json = serde_json::to_string_pretty(&translated)?;
    fs::write(output, json)?;

    // Remove checkpoint file on success
    let _ = fs::remove_file(&checkpoint_file);

    println!("Translation complete. Saved to {}", output);
    Ok(())
}

/// Translate a batch with retry logic
async fn translate_batch_with_retry(
    client: &Client,
    api_key: &str,
    base_url: &str,
    items: &[TranslateItem],
) -> Result<Vec<TranslatedItem>> {
    let mut last_error = None;

    for attempt in 0..MAX_RETRIES {
        match translate_batch(client, api_key, base_url, items).await {
            Ok(result) => return Ok(result),
            Err(e) => {
                last_error = Some(e);
                if attempt < MAX_RETRIES - 1 {
                    let delay = Duration::from_secs(2u64.pow(attempt));
                    println!("Retry attempt {} after {:?}...", attempt + 1, delay);
                    tokio::time::sleep(delay).await;
                }
            }
        }
    }

    Err(last_error.unwrap())
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

    // Parse the response with better error handling
    let cleaned_content = content
        .trim_matches(|c| c == '`')
        .replace("json\n", "")
        .replace("```json", "")
        .replace("```", "");

    let parsed: Vec<serde_json::Value> = match serde_json::from_str(&cleaned_content) {
        Ok(p) => p,
        Err(e) => {
            eprintln!("  Warning: JSON parse error: {}", e);
            eprintln!("  Response preview: {}", &cleaned_content.chars().take(200).collect::<String>());
            // Return original text for all items in this batch
            return Ok(items.iter().map(|item| TranslatedItem {
                id: item.id.clone(),
                original: item.text.clone(),
                translated: item.text.clone(),
                source: item.source.clone(),
            }).collect());
        }
    };

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
            source: item.source.clone(),
        });
    }

    Ok(results)
}

/// Load checkpoint file
fn load_checkpoint(checkpoint_file: &str) -> Result<Vec<TranslatedItem>> {
    let content = fs::read_to_string(checkpoint_file)?;
    let checkpoint: TranslatedItems = serde_json::from_str(&content)?;
    Ok(checkpoint.items)
}

/// Save checkpoint file
fn save_checkpoint(
    checkpoint_file: &str,
    source: &str,
    items: &[TranslatedItem],
) -> Result<()> {
    let checkpoint = TranslatedItems {
        source: source.to_string(),
        items: items.to_vec(),
    };

    let json = serde_json::to_string_pretty(&checkpoint)?;
    fs::write(checkpoint_file, json)?;

    Ok(())
}
