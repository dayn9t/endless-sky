# Translation Model Configuration

## Current Model

**Model**: `qwen-plus`
**Provider**: Alibaba Cloud DashScope (通义千问)
**API Endpoint**: `https://dashscope.aliyuncs.com/compatible-mode/v1`
**Format**: OpenAI-compatible API

---

## Model Details

### Qwen-Plus Specifications

| Attribute | Value |
|-----------|-------|
| Model Family | Qwen (通义千问) |
| Tier | Plus (中等规模) |
| Context Window | ~32K tokens |
| Languages | 中文、English (优化) |
| Use Case | 平衡性能和成本 |
| API Format | OpenAI-compatible |

### Pricing (Approximate)

- Input: ~0.004 元/1K tokens
- Output: ~0.012 元/1K tokens

**Estimated cost for this project**:
- UI translation (100 items): ~0.5 元
- Data translation (20,005 items): ~50-100 元
- **Total**: ~50-100 元

---

## Alternative Models

### Available Qwen Models

| Model | Speed | Quality | Cost | Best For |
|-------|-------|---------|------|----------|
| `qwen-turbo` | ⚡⚡⚡ | ⭐⭐ | 💰 | 快速翻译、草稿 |
| `qwen-plus` | ⚡⚡ | ⭐⭐⭐ | 💰💰 | **当前使用**，平衡选择 |
| `qwen-max` | ⚡ | ⭐⭐⭐⭐ | 💰💰💰 | 高质量翻译 |
| `qwen-long` | ⚡ | ⭐⭐⭐ | 💰💰 | 长文本翻译 |

### How to Change Model

Edit `es-translator/src/translator.rs` line 109:

```rust
let request = QwenRequest {
    model: "qwen-max".to_string(),  // Change here
    messages: vec![...],
};
```

Then rebuild:
```bash
cd es-translator
cargo build --release
```

---

## API Configuration

### Environment Variables

Set in `~/cc/.env`:

```bash
# Required
export DASHSCOPE_API_KEY="sk-xxxxx"

# Optional (defaults shown)
export DASHSCOPE_BASE_URL="https://dashscope.aliyuncs.com/compatible-mode/v1"
```

### API Key Management

Get your API key from:
- **Console**: https://bailian.console.aliyun.com/
- **Path**: API-KEY 管理 → 创建新的 API-KEY

---

## Translation Parameters

### Current Settings

```rust
const BATCH_SIZE: usize = 20;  // Items per API call
const DELAY_MS: u64 = 500;     // Delay between batches
```

### System Prompt

The translator uses a specialized system prompt with:
- Game background (Endless Sky lore)
- Terminology glossary (ship→飞船, outfit→装备, etc.)
- Translation rules (preserve placeholders, sci-fi style)

See `es-translator/src/translator.rs` lines 8-39 for full prompt.

---

## Performance Tuning

### Increase Speed

```rust
// Option 1: Use faster model
model: "qwen-turbo".to_string(),

// Option 2: Increase batch size (may hit rate limits)
const BATCH_SIZE: usize = 30;

// Option 3: Reduce delay (may hit rate limits)
const DELAY_MS: u64 = 200;
```

### Improve Quality

```rust
// Option 1: Use better model
model: "qwen-max".to_string(),

// Option 2: Reduce batch size (more context per call)
const BATCH_SIZE: usize = 10;

// Option 3: Add more examples to system prompt
```

---

## Error Handling

### Current Implementation

- **Retry**: Not implemented (manual retry required)
- **Checkpoint**: Not implemented (restarts from beginning)
- **Rate Limiting**: 500ms delay between batches

### Recommended Improvements

1. **Add retry logic**:
```rust
for attempt in 0..3 {
    match translate_batch(...).await {
        Ok(result) => return Ok(result),
        Err(e) if attempt < 2 => {
            tokio::time::sleep(Duration::from_secs(2u64.pow(attempt))).await;
        }
        Err(e) => return Err(e),
    }
}
```

2. **Add checkpoint system**:
```rust
// Save progress every 100 batches
if batch_num % 100 == 0 {
    save_checkpoint("checkpoint.json", &translated_items)?;
}
```

3. **Resume from checkpoint**:
```rust
let mut translated_items = load_checkpoint("checkpoint.json")
    .unwrap_or_default();
```

---

## Monitoring

### Check Translation Progress

```bash
# Real-time monitoring
tail -f es-translator/translation.log

# Progress script
./es-translator/monitor_progress.sh
```

### Check API Usage

Visit DashScope console:
- **URL**: https://bailian.console.aliyun.com/
- **Path**: 费用中心 → 用量明细

---

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| `DASHSCOPE_API_KEY not found` | Missing env var | Set in `~/cc/.env` |
| `connection closed` | Network error | Retry translation |
| `rate limit exceeded` | Too many requests | Increase `DELAY_MS` |
| `invalid model` | Wrong model name | Check available models |
| `context too long` | Batch too large | Reduce `BATCH_SIZE` |

### Debug Mode

Add debug logging:
```rust
// In translator.rs
println!("Request: {:?}", request);
println!("Response: {:?}", qwen_response);
```

---

## Model Comparison

### Translation Quality Test

Sample text: "You don't have enough money to buy this outfit."

| Model | Translation | Quality |
|-------|-------------|---------|
| qwen-turbo | 你没有足够的钱购买这个装备。 | ⭐⭐ (literal) |
| qwen-plus | 你没有足够的积分购买此装备。 | ⭐⭐⭐ (uses "积分") |
| qwen-max | 你没有足够的积分来购买这件装备。 | ⭐⭐⭐⭐ (natural) |

### Recommendation

- **Development/Testing**: `qwen-turbo` (fast, cheap)
- **Production**: `qwen-plus` (current, balanced)
- **Final Polish**: `qwen-max` (best quality)

---

## Future Enhancements

### Planned Features

- [ ] Automatic retry with exponential backoff
- [ ] Checkpoint/resume system
- [ ] Progress bar with ETA
- [ ] Translation quality validation
- [ ] Parallel batch processing
- [ ] Custom model selection via CLI flag

### CLI Enhancement

```bash
# Future usage
es-translator translate \
  --input pending.json \
  --output translated.json \
  --model qwen-max \
  --batch-size 30 \
  --checkpoint checkpoint.json \
  --resume
```

---

## References

- [Qwen API Documentation](https://help.aliyun.com/document_detail/2712195.html)
- [DashScope Console](https://bailian.console.aliyun.com/)
- [Model Pricing](https://help.aliyun.com/document_detail/2712195.html#section-pricing)
