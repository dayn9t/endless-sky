# Chinese Plugin Template

This directory contains the template structure for the Chinese translation plugin.

## Structure

When you run `es-translator generate data`, it will create:

```
plugins/chinese/
├── about.txt              # Plugin metadata
├── data/                  # Translated game data files
└── translation_mapping.json  # Translation reference
```

## about.txt Template

```
name: "简体中文语言包"
version: "1.0.0"
description: "Endless Sky 简体中文翻译插件，由 AI 自动翻译生成。"
author: "ES Translator"
```

## Usage

1. Generate the plugin:
   ```bash
   cd es-translator
   cargo run -- generate data translated_data.json
   ```

2. The plugin will be created in `plugins/chinese/`

3. Launch Endless Sky to load the translations

## Notes

- The `plugins/` directory is gitignored (user-installed plugins)
- Generated files are not committed to the repository
- This template documents the expected structure
- Data file translations require manual integration or additional tooling

## Translation Mapping

The `translation_mapping.json` file contains:
- Original text from data files
- Translated Chinese text
- Item IDs for reference

This mapping can be used to:
- Manually update data files
- Create automated patching tools
- Review translation quality
- Track translation progress
