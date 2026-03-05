use crate::types::TranslatedItems;
use anyhow::Result;
use std::collections::HashMap;
use std::fs;

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

    // Create plugin directory structure
    fs::create_dir_all("plugins/chinese/data")?;

    // Write about.txt
    let about = r#"name: "简体中文语言包"
version: "1.0.0"
description: "Endless Sky 简体中文翻译插件，由 AI 自动翻译生成。"
author: "ES Translator""#;
    fs::write("plugins/chinese/about.txt", about)?;

    // Create a simple translation mapping for all items
    let mapping_path = "plugins/chinese/translation_mapping.json";
    let mapping = serde_json::to_string_pretty(&serde_json::json!({
        "note": "This mapping shows what needs to be translated in data files",
        "translations": translated.items.iter().map(|i| serde_json::json!({
            "id": i.id,
            "original": i.original,
            "translated": i.translated
        })).collect::<Vec<_>>()
    }))?;
    fs::write(mapping_path, mapping)?;

    println!("Generated plugins/chinese/ with {} translations", translated.items.len());
    println!("Note: Data file translation requires manual integration or additional tooling");
    Ok(())
}
