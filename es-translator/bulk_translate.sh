#!/bin/bash
# 批量翻译 - 顺序处理所有分块，带自动恢复

set -e

CHUNK_DIR="es-translator/chunks"
OUTPUT_DIR="es-translator/translated_chunks"
TRANSLATOR="./es-translator/target/release/es-translator"
LOG_FILE="es-translator/bulk_translation.log"

source ~/cc/.env

# 获取所有分块
TOTAL=$(ls $CHUNK_DIR/chunk_*.json | wc -l)
COMPLETED=$(ls $OUTPUT_DIR/chunk_*.json 2>/dev/null | wc -l)

echo "=== 批量翻译 ===" | tee -a "$LOG_FILE"
echo "时间: $(date)" | tee -a "$LOG_FILE"
echo "总分块: $TOTAL, 已完成: $COMPLETED" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

for i in $(seq -f "%04g" 1 $TOTAL); do
    INPUT_FILE="$CHUNK_DIR/chunk_$i.json"
    OUTPUT_FILE="$OUTPUT_DIR/chunk_$i.json"

    if [ -f "$OUTPUT_FILE" ]; then
        echo "[$i/$TOTAL] ✓ chunk_$i.json 已完成" | tee -a "$LOG_FILE"
        continue
    fi

    echo "[$i/$TOTAL] 翻译 chunk_$i.json..." | tee -a "$LOG_FILE"

    if timeout 1800 $TRANSLATOR translate --input "$INPUT_FILE" --output "$OUTPUT_FILE" 2>&1 | tee -a "$LOG_FILE"; then
        echo "  ✓ 完成 chunk_$i.json" | tee -a "$LOG_FILE"
    else
        echo "  ✗ 失败 chunk_$i.json，继续下一个" | tee -a "$LOG_FILE"
    fi

    # 每个分块后休息
    sleep 1
done

echo "" | tee -a "$LOG_FILE"
echo "=== 批量翻译完成 ===" | tee -a "$LOG_FILE"
FINAL=$(ls $OUTPUT_DIR/chunk_*.json 2>/dev/null | wc -l)
echo "已完成: $FINAL/$TOTAL" | tee -a "$LOG_FILE"
echo "时间: $(date)" | tee -a "$LOG_FILE"

# 如果全部完成，合并
if [ "$FINAL" -eq "$TOTAL" ]; then
    echo "所有分块完成，开始合并..." | tee -a "$LOG_FILE"
    ./es-translator/target/release/es-translator merge \
        --input-dir "$OUTPUT_DIR" \
        --output es-translator/translated_data.json \
        --source "data"
fi
