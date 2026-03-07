use crate::types::TranslatedItems;
use anyhow::Result;
use std::collections::HashMap;
use std::fs;

pub fn generate(type_arg: &str, input: &str) -> Result<()> {
    match type_arg {
        "ui" => generate_ui(input),
        "data" => generate_data(input),
        _ => anyhow::bail!("Unknown type: {}", type_arg),
    }
}

fn generate_ui(input: &str) -> Result<()> {
    let content = fs::read_to_string(input)?;
    let translated: TranslatedItems = serde_json::from_str(&content)?;

    let mut locale: HashMap<String, serde_json::Value> = HashMap::new();

    for item in &translated.items {
        let parts: Vec<&str> = item.id.split('.').collect();
        if parts.len() >= 2 {
            let category = parts[0].to_lowercase();
            let key = parts[1..].join(".");
            let entry = locale.entry(category).or_insert_with(|| serde_json::json!({}));
            if let Some(obj) = entry.as_object_mut() {
                obj.insert(key, serde_json::Value::String(item.translated.clone()));
            }
        }
    }

    fs::create_dir_all("locales")?;
    let json = serde_json::to_string_pretty(&locale)?;
    fs::write("locales/zh_CN.json", json)?;
    println!("Generated locales/zh_CN.json with {} translations", translated.items.len());
    Ok(())
}

fn generate_data(input: &str) -> Result<()> {
    let content = fs::read_to_string(input)?;
    let translated: TranslatedItems = serde_json::from_str(&content)?;

    let mut file_map: HashMap<String, Vec<(String, String, bool)>> = HashMap::new();
    for item in &translated.items {
        if item.translated == item.original {
            continue;
        }
        let is_desc = item.id.contains(".desc.");
        let without_prefix = item.id.strip_prefix("data.").unwrap_or(&item.id);
        if let Some(txt_pos) = without_prefix.find(".txt.") {
            let dir_file = &without_prefix[..txt_pos];
            let file_key = if let Some(dot) = dir_file.rfind('.') {
                format!("{}/{}.txt", dir_file[..dot].replace('.', "/"), &dir_file[dot + 1..])
            } else {
                format!("{}.txt", dir_file)
            };
            file_map
                .entry(file_key)
                .or_default()
                .push((item.original.clone(), item.translated.clone(), is_desc));
        }
    }

    let plugin_dir = std::path::Path::new("plugins/zh_CN");
    let data_dir = plugin_dir.join("data");
    fs::create_dir_all(plugin_dir)?;

    let plugin_txt = concat!(
        "name ", '"', "Chinese Localization", '"', '\n',
        "version ", '"', "1.0.0", '"', '\n',
        "about ", '"', "Endless Sky Chinese Localization Plugin.", '"', '\n',
    );
    fs::write(plugin_dir.join("plugin.txt"), plugin_txt)?;

    let re = regex::Regex::new(r#"(?m)^(mission|phrase) ""#).unwrap();

    let mut files_written = 0;
    let mut total_replaced = 0;

    for (file_key, translations) in &file_map {
        let src_path = std::path::Path::new("data").join(file_key);
        if !src_path.exists() {
            eprintln!("Warning: source file not found: {}", src_path.display());
            continue;
        }

        let mut file_content = fs::read_to_string(&src_path)?;

        let mut replaced = 0;
        // Game syntax keywords that must never be translated.
        // Any item whose original text (trimmed) starts with one of these words,
        // or contains a line starting with one of these words, is skipped.
        let game_keywords: &[&str] = &[
            // Top-level block types
            "mission", "phrase", "conversation", "planet", "outfit", "ship",
            "fleet", "government", "event", "system", "help", "news", "shipyard",
            "outfitter", "wormhole", "tribute", "dialog",
            // Mission attributes
            "name", "description", "source", "destination", "waypoint", "cargo",
            "passengers", "deadline", "clearance", "infiltrating", "priority",
            "job", "landing", "repeat", "blocked", "invisible", "minor",
            "npc", "on", "to", "log", "payment", "fine", "illegal", "stealth",
            "autosave", "fail", "defer", "apply", "mark", "replace", "set",
            "depart", "launch", "port", "bribe", "security", "save",
            "clear", "spaceport", "has", "not", "and", "or", "never",
            "assisting", "boarding", "entering", "disabled", "flagship",
            // Conversation nodes
            "goto", "label", "choice", "branch", "action", "scene",
            "accept", "decline", "never", "die", "flee",
            // NPC attributes
            "government", "personality", "system", "planet", "ship",
            "conversation", "dialog",
            // Phrase attributes
            "word",
        ];
        for (original, translated, is_desc) in translations {
            // Skip any item whose trimmed content starts with a game keyword
            // (with or without a trailing colon, e.g. "outfit:" or "outfit").
            let first_word = original.trim().split_whitespace().next().unwrap_or("");
            let first_word_bare = first_word.trim_end_matches(':');
            if game_keywords.contains(&first_word) || game_keywords.contains(&first_word_bare) {
                continue;
            }
            // Also skip multi-line items that contain any line starting with a keyword.
            let has_keyword_line = original.lines().any(|line| {
                let w = line.trim().split_whitespace().next().unwrap_or("");
                let w_bare = w.trim_end_matches(':');
                game_keywords.contains(&w) || game_keywords.contains(&w_bare)
            });
            if has_keyword_line {
                continue;
            }
            // Skip if translation introduces more newlines than original
            // (would break backtick text spanning multiple lines).
            if translated.matches('\n').count() > original.matches('\n').count() {
                continue;
            }
            // Skip items whose trimmed content is purely numeric (phrase weights like " 4\n\t\t").
            let trimmed_orig = original.trim();
            if trimmed_orig.chars().all(|c| c.is_ascii_digit() || c.is_whitespace()) && !trimmed_orig.is_empty() {
                continue;
            }

            if *is_desc {
                let orig = original.trim();
                let trans = translated.trim();
                if file_content.contains(orig) {
                    file_content = file_content.replacen(orig, trans, 1);
                    replaced += 1;
                }
            } else {
                let bt = char::from(0x60).to_string();
                let old = format!("{}{}{}", bt, original, bt);
                let new_val = format!("{}{}{}", bt, translated, bt);
                if file_content.contains(&old) {
                    // Skip if this backtick text is used as a name reference.
                    // Two cases:
                    // 1. Preceded by a reference keyword (ammo, outfit, ship, etc.)
                    // 2. Appears as a standalone line item (only tabs before it on the line)
                    //    which means it's an outfitter/shipyard stock list entry.
                    let ref_keywords = [
                        "ammo", "stock", "installed", "add", "remove", "outfit", "ship",
                        "flagship", "escort", "carry", "require", "provides", "thumbnail",
                        "sprite", "hardpoint", "gun", "turret", "fighter", "drone",
                        "phrase", "conversation", "event", "fleet", "government",
                    ];
                    let idx = file_content.find(&old).unwrap_or(0);
                    let before = &file_content[..idx];
                    let last_word = before.split_whitespace().next_back().unwrap_or("")
                        .trim_end_matches(':').to_lowercase();
                    let last_word_str: &str = &last_word;
                    if ref_keywords.contains(&last_word_str) {
                        continue;
                    }
                    // Check if this is a standalone line (only tabs/spaces before backtick on this line)
                    let line_start = before.rfind('\n').map(|i| i + 1).unwrap_or(0);
                    let line_prefix = &before[line_start..];
                    if line_prefix.chars().all(|c| c == '\t' || c == ' ') {
                        // It's a standalone backtick item on its own line.
                        // Check if it's inside a block that uses outfit/ship name references.
                        let block_keywords = ["outfitter", "shipyard", "fleet", "npc", "ship", "phrase"];
                        let nearest_block = block_keywords.iter()
                            .filter_map(|kw| {
                                let search = format!("\n{} ", kw);
                                before.rfind(&search).map(|pos| (pos, *kw))
                            })
                            .max_by_key(|(pos, _)| *pos);
                        let nearest_mission = before.rfind("\nmission ").unwrap_or(0);
                        let nearest_conversation = before.rfind("\nconversation ").unwrap_or(0);
                        // Only mission and conversation blocks contain narrative backtick text.
                        // phrase/outfitter/ship/fleet blocks use backtick for name references.
                        let nearest_text_block = nearest_mission.max(nearest_conversation);
                        if let Some((block_pos, _)) = nearest_block {
                            if block_pos >= nearest_text_block {
                                continue;
                            }
                        }
                    }
                    file_content = file_content.replacen(&old, &new_val, 1);
                    replaced += 1;
                }
            }
        }

        let replacement = concat!("overwrite", '\n', "$1 ", '"');
        let file_content = re.replace_all(&file_content, replacement).to_string();

        let dest_path = data_dir.join(file_key);
        if let Some(parent) = dest_path.parent() {
            fs::create_dir_all(parent)?;
        }
        fs::write(&dest_path, &file_content)?;

        files_written += 1;
        total_replaced += replaced;
    }

    println!("Generated plugins/zh_CN/ -- {} files, {} replacements", files_written, total_replaced);
    Ok(())
}
