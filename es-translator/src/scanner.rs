use crate::types::{PendingTranslations, TranslateItem};
use anyhow::Result;
use regex::Regex;
use std::collections::HashMap;
use std::fs;
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

pub struct ScanOptions {
    pub data_dir: PathBuf,
    pub plugin_dir: PathBuf,
    pub output: String,
    pub report: bool,
}

struct ScanResult {
    missing_files: Vec<(PathBuf, usize)>,   // (file, string_count)
    untranslated: Vec<TranslateItem>,
}

pub fn scan(opts: ScanOptions) -> Result<()> {
    let result = do_scan(&opts)?;

    if opts.report {
        print_report(&result);
    }

    let pending = PendingTranslations {
        source: "scan".to_string(),
        items: result.untranslated,
    };

    let json = serde_json::to_string_pretty(&pending)?;
    fs::write(&opts.output, &json)?;
    println!("Wrote {} untranslated items to {}", pending.items.len(), opts.output);
    Ok(())
}

fn do_scan(opts: &ScanOptions) -> Result<ScanResult> {
    let mut missing_files: Vec<(PathBuf, usize)> = Vec::new();
    let mut untranslated: Vec<TranslateItem> = Vec::new();

    // Walk all .txt files in data_dir
    for entry in WalkDir::new(&opts.data_dir)
        .into_iter()
        .filter_map(|e| e.ok())
        .filter(|e| e.path().extension().map_or(false, |x| x == "txt"))
    {
        let orig_path = entry.path();
        let rel = orig_path.strip_prefix(&opts.data_dir)?;
        let plugin_path = opts.plugin_dir.join(rel);

        // Extract translatable strings from original file
        let orig_strings = extract_translatable(orig_path)?;
        if orig_strings.is_empty() {
            continue;
        }

        if !plugin_path.exists() {
            // Entire file missing from plugin
            let count = orig_strings.len();
            missing_files.push((rel.to_path_buf(), count));
            for (line_num, text) in orig_strings {
                untranslated.push(TranslateItem {
                    id: format!("scan.{}.{}", rel.to_string_lossy().replace(['/', '\\', ' '], "."), line_num),
                    text,
                    context: Some(format!("line {} in {}", line_num, rel.display())),
                    source: Some(rel.to_string_lossy().into_owned()),
                });
            }
        } else {
            // File exists — check each string
            let plugin_content = fs::read_to_string(&plugin_path)?;

            for (line_num, text) in orig_strings {
                // Skip if original text is not English (already translated in source?)
                if !is_english(&text) {
                    continue;
                }

                // Check if plugin has translated this specific string
                let is_translated = check_if_translated(&plugin_content, &text);

                if !is_translated {
                    untranslated.push(TranslateItem {
                        id: format!("scan.{}.{}", rel.to_string_lossy().replace(['/', '\\', ' '], "."), line_num),
                        text,
                        context: Some(format!("line {} in {}", line_num, rel.display())),
                        source: Some(rel.to_string_lossy().into_owned()),
                    });
                }
            }
        }
    }

    // Deduplicate by text
    untranslated.sort_by(|a, b| a.text.cmp(&b.text));
    untranslated.dedup_by(|a, b| a.text == b.text);

    Ok(ScanResult { missing_files, untranslated })
}

/// Extract translatable strings from an ES data file.
/// Returns Vec<(line_number, text)> for precise matching.
fn extract_translatable(path: &Path) -> Result<Vec<(usize, String)>> {
    let content = match fs::read_to_string(path) {
        Ok(c) => c,
        Err(_) => return Ok(vec![]),
    };

    let mut results = Vec::new();

    // Backtick strings: `some text`
    // Only match backticks that contain narrative text (longer, with sentences)
    let backtick_re = Regex::new(r"`([^`]{20,})`").unwrap();

    // Quoted description/name fields
    let quoted_re = Regex::new(
        r#"(?m)^\s*(?:description|name|label|"wrapped label")\s+"([^"]{10,})""#
    ).unwrap();

    for (line_num, line) in content.lines().enumerate() {
        // Skip comments
        if line.trim().starts_with('#') {
            continue;
        }

        // Check for backtick strings on this line
        for cap in backtick_re.captures_iter(line) {
            let text = cap[1].trim().to_string();
            if should_translate(&text) {
                results.push((line_num + 1, text));
            }
        }

        // Check for quoted fields on this line
        for cap in quoted_re.captures_iter(line) {
            let text = cap[1].trim().to_string();
            if should_translate(&text) {
                results.push((line_num + 1, text));
            }
        }
    }

    Ok(results)
}

/// Check if the plugin file has been translated.
/// Returns true if the plugin contains significant Chinese content (>
/// threshold).
fn check_if_translated(plugin_content: &str, _original: &str) -> bool {
    // Count CJK characters in the plugin file
    let cjk_count = plugin_content.chars().filter(|&c| is_cjk(c)).count();
    let total_chars = plugin_content.chars().filter(|c| c.is_alphabetic()).count();

    if total_chars == 0 {
        return false;
    }

    // If > 30% of alphabetic characters are CJK, consider it translated
    let ratio = cjk_count as f64 / total_chars as f64;
    ratio > 0.3
}

/// True if the string should be translated (English narrative text)
fn should_translate(s: &str) -> bool {
    let trimmed = s.trim();

    // Minimum length for narrative text
    if trimmed.len() < 20 {
        return false;
    }

    // Skip pure placeholders
    if trimmed.starts_with('<') && trimmed.ends_with('>') && !trimmed.contains(' ') {
        return false;
    }

    // Skip code/keywords (starts with lowercase keyword)
    let first_word = trimmed.split_whitespace().next().unwrap_or("");
    let code_keywords = ["label", "goto", "branch", "apply", "set", "clear",
                         "has", "not", "and", "or", "mission", "event", "ship"];
    if code_keywords.contains(&first_word) {
        return false;
    }

    // Must be primarily English to need translation
    is_english(trimmed)
}

/// True if the string is primarily English (no CJK, has ASCII letters)
fn is_english(s: &str) -> bool {
    // If it has any CJK character, it's already translated
    if s.chars().any(|c| is_cjk(c)) {
        return false;
    }
    // Must have at least one ASCII letter
    s.chars().any(|c| c.is_ascii_alphabetic())
}

fn is_cjk(c: char) -> bool {
    matches!(c,
        '\u{4E00}'..='\u{9FFF}'   // CJK Unified Ideographs
        | '\u{3400}'..='\u{4DBF}' // CJK Extension A
        | '\u{20000}'..='\u{2A6DF}' // CJK Extension B
        | '\u{F900}'..='\u{FAFF}' // CJK Compatibility Ideographs
        | '\u{2E80}'..='\u{2EFF}' // CJK Radicals Supplement
        | '\u{3000}'..='\u{303F}' // CJK Symbols and Punctuation
    )
}

fn print_report(result: &ScanResult) {
    let total: usize = result.untranslated.len();
    let missing_count: usize = result.missing_files.iter().map(|(_, n)| n).sum();

    println!("\nScan complete: {} untranslated strings found\n", total);

    if !result.missing_files.is_empty() {
        let file_count = result.missing_files.len();
        println!("Missing plugin files ({}) — {} strings:", file_count, missing_count);
        let mut sorted = result.missing_files.clone();
        sorted.sort_by(|a, b| b.1.cmp(&a.1));
        for (path, count) in &sorted {
            println!("  {:60} ({} strings)", path.display(), count);
        }
        println!();
    }

    // Group remaining by source file
    let mut by_file: HashMap<String, usize> = HashMap::new();
    for item in &result.untranslated {
        if let Some(src) = &item.source {
            *by_file.entry(src.clone()).or_insert(0) += 1;
        }
    }

    if !by_file.is_empty() {
        println!("Untranslated strings by file:");
        let mut sorted: Vec<_> = by_file.iter().collect();
        sorted.sort_by(|a, b| b.1.cmp(a.1));
        for (file, count) in sorted {
            println!("  {:60} {} strings", file, count);
        }
        println!();
    }
}
