# Chinese Plugin Generator Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Implement `generate --type data` in es-translator to produce a working `plugins/zh_CN/` directory that makes Endless Sky display Chinese text.

**Architecture:** Replace `generate_data()` in `generator.rs` with a real implementation that reads `translated_data.json`, groups translations by source file, reads each original data file, performs text substitution, inserts `overwrite` keywords before `mission`/`phrase` blocks, and writes the result to `plugins/zh_CN/data/`. Also generates `plugin.txt`.

**Tech Stack:** Rust, regex crate (already in Cargo.toml), std::fs, serde_json

---

### Task 1: Implement `generate_data()` in generator.rs

**Files:**
- Modify: `es-translator/src/generator.rs`

The current `generate_data()` only writes a JSON mapping file. Replace it entirely.

**Step 1: Replace `generate_data()` with the real implementation**

Open `es-translator/src/generator.rs` and replace the entire `generate_data()` function (lines 51–81) with:

```rust
/// Generate plugins/zh_CN/data/ for data translations
fn generate_data(input: &str) -> Result<()> {
    let content = fs::read_to_string(input)?;
    let translated: TranslatedItems = serde_json::from_str(&content)?;

    // Group translations by source file.
    // ID format: "data.<dir>.<file>.txt.<seq>" or "data.<dir>.<file>.txt.desc.<seq>"
    let mut file_map: HashMap<String, Vec<(String, String, bool)>> = HashMap::new();
    for item in &translated.items {
        // Skip items where translation equals original (empty/template vars)
        if item.translated == item.original {
            continue;
        }
        let is_desc = item.id.contains(".desc.");
        // Extract file key: everything between "data." and the last ".txt.<num>"
        // e.g. "data.human.intro missions.txt.127" -> "human/intro missions.txt"
        let without_prefix = item.id.strip_prefix("data.").unwrap_or(&item.id);
        // Find ".txt." separator
        if let Some(txt_pos) = without_prefix.find(".txt.") {
            let dir_file = &without_prefix[..txt_pos];
            // Convert dots-as-separators back to path: last segment is filename,
            // everything before last dot is directory.
            let file_key = if let Some(dot) = dir_file.rfind('.') {
                format!("{}/{}.txt", dir_file[..dot].replace('.', "/"), &dir_file[dot+1..])
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

    // Write plugin.txt
    fs::create_dir_all(&plugin_dir)?;
    fs::write(
        plugin_dir.join("plugin.txt"),
        "name \"Chinese Localization\"\nversion \"1.0.0\"\nabout \"Endless Sky 简体中文本地化插件。\"\n",
    )?;

    let mut files_written = 0;
    let mut total_replaced = 0;

    for (file_key, translations) in &file_map {
        let src_path = std::path::Path::new("data").join(file_key);
        if !src_path.exists() {
            eprintln!("Warning: source file not found: {}", src_path.display());
            continue;
        }

        let mut file_content = fs::read_to_string(&src_path)?;

        // Apply translations
        let mut replaced = 0;
        for (original, translated, is_desc) in translations {
            if *is_desc {
                // description field: plain string replacement
                let orig = original.trim();
                let trans = translated.trim();
                if file_content.contains(orig) {
                    file_content = file_content.replacen(orig, trans, 1);
                    replaced += 1;
                }
            } else {
                // backtick-quoted paragraph text
                let old = format!("`{}`", original);
                let new = format!("`{}`", translated);
                if file_content.contains(&old) {
                    file_content = file_content.replacen(&old, &new, 1);
                    replaced += 1;
                }
            }
        }

        // Insert `overwrite` before every `mission` and `phrase` top-level block
        let re = regex::Regex::new(r"(?m)^(mission|phrase) \"").unwrap();
        let file_content = re.replace_all(&file_content, "overwrite\n$1 \"").to_string();

        // Write to plugin directory, preserving subdirectory structure
        let dest_path = data_dir.join(file_key);
        if let Some(parent) = dest_path.parent() {
            fs::create_dir_all(parent)?;
        }
        fs::write(&dest_path, &file_content)?;

        files_written += 1;
        total_replaced += replaced;
    }

    println!(
        "Generated plugins/zh_CN/ — {} files, {} replacements",
        files_written, total_replaced
    );
    Ok(())
}
```

**Step 2: Add `regex` import at top of file**

The `regex` crate is already in `Cargo.toml`. Add the use statement at the top of `generator.rs` if not present:

```rust
use regex::Regex;
```

(The inline `regex::Regex::new(...)` in the function body also works without a top-level use.)

**Step 3: Build**

```bash
cd /home/jiang/cc/games/endless-sky/es-translator
cargo build --release 2>&1 | tail -20
```

Expected: `Finished release [optimized]` with no errors.

**Step 4: Commit**

```bash
cd /home/jiang/cc/games/endless-sky
git add es-translator/src/generator.rs
git commit -m "feat: implement generate_data to produce zh_CN plugin files"
```

---

### Task 2: Run the generator and verify output

**Files:**
- Creates: `plugins/zh_CN/plugin.txt`
- Creates: `plugins/zh_CN/data/**/*.txt` (154 files)

**Step 1: Run the generator**

```bash
cd /home/jiang/cc/games/endless-sky
./es-translator/target/release/es-translator generate \
    --type data \
    --input es-translator/translated_data.json
```

Expected output:
```
Generated plugins/zh_CN/ — 154 files, ~19993 replacements
```

**Step 2: Spot-check output files**

```bash
# Check plugin.txt exists
cat plugins/zh_CN/plugin.txt

# Check a mission file has Chinese text and overwrite keywords
grep -c "overwrite" plugins/zh_CN/data/coalition/lunarium\ intro.txt
grep "description" plugins/zh_CN/data/coalition/lunarium\ intro.txt | head -5

# Check a planet file
grep "description" plugins/zh_CN/data/map\ planets.txt | head -3

# Check an outfit file
grep "description" plugins/zh_CN/data/human/weapons.txt | head -3
```

Expected: mission files contain `overwrite` before each `mission`/`phrase` block; description fields show Chinese text.

**Step 3: Count files and replacements**

```bash
find plugins/zh_CN/data -name "*.txt" | wc -l
# Expected: 154

grep -r "overwrite" plugins/zh_CN/data | wc -l
# Expected: ~2748 (2245 missions + 503 phrases)

# Verify Chinese characters present
python3 -c "
import os, glob
total_zh = 0
for f in glob.glob('plugins/zh_CN/data/**/*.txt', recursive=True):
    content = open(f).read()
    total_zh += sum(1 for c in content if '\u4e00' <= c <= '\u9fff')
print(f'Total Chinese characters: {total_zh}')
"
```

Expected: several hundred thousand Chinese characters.

**Step 4: Commit**

```bash
cd /home/jiang/cc/games/endless-sky
git add plugins/zh_CN/
git commit -m "feat: generate zh_CN plugin with 154 translated data files"
```

---

### Task 3: Install plugin and test in-game

**Step 1: Copy plugin to user plugin directory**

```bash
# Linux (SDL_GetPrefPath → ~/.local/share/endless-sky/)
cp -r plugins/zh_CN ~/.local/share/endless-sky/plugins/
```

**Step 2: Launch the game**

```bash
./build/endless-sky
```

**Step 3: Verify in-game**

- Start a new game → intro conversation should show Chinese text
- Open the outfitter → outfit descriptions should be Chinese
- Open the map → planet descriptions should be Chinese
- Accept a mission → mission description should be Chinese

**Step 4: Check for errors**

```bash
cat ~/.local/share/endless-sky/errors.txt | tail -30
```

Look for any `Duplicate definition of mission:` warnings — these indicate a `mission` block is missing its `overwrite` keyword.

**Step 5: If duplicate mission warnings appear**

The `overwrite` regex may have missed some edge cases. Check:

```bash
grep "Duplicate definition" ~/.local/share/endless-sky/errors.txt | head -10
```

Then inspect the corresponding plugin file to see if `overwrite` is present before that mission.

---

### Task 4: Handle font rendering (if Chinese characters show as boxes)

The game uses bitmap fonts (`images/font/ubuntu14r.png`, `ubuntu18r.png`). If Chinese characters render as empty boxes, the font does not include CJK glyphs.

**Step 1: Check current font**

```bash
ls -la /home/jiang/cc/games/endless-sky/images/font/
```

**Step 2: If font lacks CJK — check if FreeType is available**

```bash
grep -r "FreeType\|freetype\|TTF\|ttf" /home/jiang/cc/games/endless-sky/source/ | head -10
```

**Step 3: If bitmap font only — replace with CJK-capable font**

The game's `Font.cpp` loads PNG sprite sheets. A CJK-capable bitmap font sheet would need to be generated. This is a separate task outside this plan's scope.

**Workaround:** Verify the game was already rendering Chinese UI text (from `locales/zh_CN.json`) before this plugin. If UI text was already Chinese, the font supports CJK and the plugin will work.

---

### Task 5: Wire up `generate --type data` in the full pipeline (optional)

**Files:**
- Modify: `es-translator/src/main.rs` (Run command)

If you want `es-translator run --all` to also generate the plugin:

**Step 1: Find the Run command handler in main.rs**

```bash
grep -n "Run\|run\|generate" /home/jiang/cc/games/endless-sky/es-translator/src/main.rs
```

**Step 2: Add generate_data call after translation completes**

In the `Commands::Run` branch, after `generator::generate("ui", ...)`, add:

```rust
generator::generate("data", "es-translator/translated_data.json")?;
```

**Step 3: Build and test**

```bash
cargo build --release
```

**Step 4: Commit**

```bash
git add es-translator/src/main.rs
git commit -m "feat: add data plugin generation to full pipeline"
```

---

## Quick Reference

```bash
# Rebuild translator
cd /home/jiang/cc/games/endless-sky/es-translator && cargo build --release

# Generate plugin
cd /home/jiang/cc/games/endless-sky
./es-translator/target/release/es-translator generate \
    --type data --input es-translator/translated_data.json

# Install
cp -r plugins/zh_CN ~/.local/share/endless-sky/plugins/

# Run game
./build/endless-sky

# Check errors
cat ~/.local/share/endless-sky/errors.txt | tail -20
```
