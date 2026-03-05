use crate::types::{PendingTranslations, TranslatedItems, TranslatedItem};
use anyhow::Result;
use std::fs;
use std::path::Path;

/// Resume translation from a checkpoint
pub fn resume_translation(
    pending_file: &str,
    checkpoint_file: &str,
    output_file: &str,
) -> Result<PendingTranslations> {
    // Load pending translations
    let content = fs::read_to_string(pending_file)?;
    let mut pending: PendingTranslations = serde_json::from_str(&content)?;

    // Try to load checkpoint
    if Path::new(checkpoint_file).exists() {
        let checkpoint_content = fs::read_to_string(checkpoint_file)?;
        let translated: TranslatedItems = serde_json::from_str(&checkpoint_content)?;

        println!("Found checkpoint with {} translated items", translated.items.len());

        // Remove already translated items
        let translated_ids: std::collections::HashSet<_> =
            translated.items.iter().map(|i| i.id.clone()).collect();

        pending.items.retain(|item| !translated_ids.contains(&item.id));

        println!("Remaining items to translate: {}", pending.items.len());
    }

    Ok(pending)
}

/// Save checkpoint during translation
pub fn save_checkpoint(
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
