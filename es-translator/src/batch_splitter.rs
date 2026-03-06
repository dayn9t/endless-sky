use crate::types::{PendingTranslations, TranslatedItem, TranslateItem};
use anyhow::{Context, Result};
use std::fs;
use std::path::Path;

const ITEMS_PER_FILE: usize = 500; // 每个文件 500 条

/// Split large pending file into smaller chunks
pub fn split_pending_file(input: &str, output_dir: &str) -> Result<Vec<String>> {
    fs::create_dir_all(output_dir)?;

    let content = fs::read_to_string(input)?;
    let pending: PendingTranslations = serde_json::from_str(&content)?;

    let total_items = pending.items.len();
    let total_files = (total_items + ITEMS_PER_FILE - 1) / ITEMS_PER_FILE;

    println!("Splitting {} items into {} files...", total_items, total_files);

    let mut chunk_files = Vec::new();

    for (file_idx, chunk) in pending.items.chunks(ITEMS_PER_FILE).enumerate() {
        let chunk_pending = PendingTranslations {
            source: pending.source.clone(),
            items: chunk.to_vec(),
        };

        let output_file = format!("{}/chunk_{:04}.json", output_dir, file_idx + 1);
        let json = serde_json::to_string_pretty(&chunk_pending)?;
        fs::write(&output_file, json)?;

        println!("  Created: {} ({} items)", output_file, chunk.len());
        chunk_files.push(output_file);
    }

    println!("Split complete: {} files created", chunk_files.len());
    Ok(chunk_files)
}

/// Merge translated chunk files into single output
pub fn merge_translated_chunks(chunk_files: &[String], output: &str, source: &str) -> Result<()> {
    println!("Merging {} translated chunks...", chunk_files.len());

    let mut all_items: Vec<TranslatedItem> = Vec::new();

    for chunk_file in chunk_files {
        if !Path::new(chunk_file).exists() {
            println!("  Warning: {} not found, skipping", chunk_file);
            continue;
        }

        let content = fs::read_to_string(chunk_file)?;
        let translated: crate::types::TranslatedItems = serde_json::from_str(&content)?;
        let item_count = translated.items.len();
        all_items.extend(translated.items);

        println!("  Merged: {} ({} items)", chunk_file, item_count);
    }

    let merged = crate::types::TranslatedItems {
        source: source.to_string(),
        items: all_items,
    };

    let total_count = merged.items.len();
    let json = serde_json::to_string_pretty(&merged)?;
    fs::write(output, json)?;

    println!("Merge complete: {} total items saved to {}", total_count, output);
    Ok(())
}

/// Get list of unprocessed chunks
pub fn get_unprocessed_chunks(chunk_dir: &str, translated_dir: &str) -> Result<Vec<String>> {
    let mut unprocessed = Vec::new();

    for entry in fs::read_dir(chunk_dir)? {
        let entry = entry?;
        let path = entry.path();

        if path.extension().and_then(|s| s.to_str()) == Some("json") {
            let filename = path.file_name().unwrap().to_str().unwrap();
            let translated_path = format!("{}/{}", translated_dir, filename);

            if !Path::new(&translated_path).exists() {
                unprocessed.push(path.to_str().unwrap().to_string());
            }
        }
    }

    // Sort to maintain order
    unprocessed.sort();
    Ok(unprocessed)
}

/// Count completed translations
pub fn count_completed(translated_dir: &str) -> Result<usize> {
    let mut total = 0usize;

    if Path::new(translated_dir).exists() {
        for entry in fs::read_dir(translated_dir)? {
            let entry = entry?;
            let path = entry.path();

            if path.extension().and_then(|s| s.to_str()) == Some("json") {
                let content = fs::read_to_string(&path)?;
                let translated: crate::types::TranslatedItems = serde_json::from_str(&content)?;
                total += translated.items.len();
            }
        }
    }

    Ok(total)
}
