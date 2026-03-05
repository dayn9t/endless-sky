use crate::types::{PendingTranslations, TranslateItem};
use anyhow::Result;
use regex::Regex;
use std::fs;
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
