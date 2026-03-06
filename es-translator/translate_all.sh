#!/bin/bash
# 完整翻译脚本 - 带状态记录

set -e

CHUNK_DIR="es-translator/chunks"
OUTPUT_DIR="es-translator/translated_chunks"
TRANSLATOR="./es-translator/target/release/es-translator"
LOG_FILE="es-translator/translation_final.log"
STATUS_FILE="es-translator/translation_status.txt"

# 确保目录存在
mkdir -p "$OUTPUT_DIR"

# 加载环境变量
source ~/cc/.env

# 获取总分块数
TOTAL=$(ls $CHUNK_DIR/chunk_*.json 2>/dev/null | wc -l)
COMPLETED=$(ls $OUTPUT_DIR/chunk_*.json 2>/dev/null | wc -l)

echo "=== Endless Sky 批量翻译 ===" | tee -a "$LOG_FILE"
echo "时间: $(date)" | tee -a "$LOG_FILE"
echo "总分块: $TOTAL" | tee -a "$LOG_FILE"
echo "已完成: $COMPLETED" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

# 更新状态
save_status() {
    local current=$1
    echo "$current/$TOTAL" > "$STATUS_FILE"
    echo "$(date '+%H:%M:%S') - 进度: $current/$TOTAL" | tee -a "$LOG_FILE"
}

for i in $(seq -f "%04g" 1 $TOTAL); do
    INPUT_FILE="$CHUNK_DIR/chunk_$i.json"
    OUTPUT_FILE="$OUTPUT_DIR/chunk_$i.json"

    if [ -f "$OUTPUT_FILE" ]; then
        echo "[$i/$TOTAL] chunk_$i.json 已翻译，跳过" | tee -a "$LOG_FILE"
        continue
    fi

    echo "[$i/$TOTAL] 正在翻译 chunk_$i.json..." | tee -a "$LOG_FILE"

    if $TRANSLATOR translate --input "$INPUT_FILE" --output "$OUTPUT_FILE" 2>&1 | tee -a "$LOG_FILE"; then
        echo "  ✓ 完成: chunk_$i.json" | tee -a "$LOG_FILE"
        save_status "$i"
    else
        echo "  ✗ 失败: chunk_$i.json，记录错误" | tee -a "$LOG_FILE"
        echo "$i" >> "es-translator/failed_chunks.txt"
    fi

    # 每个分块后等待，避免限流
    sleep 2
done

echo "" | tee -a "$LOG_FILE"
echo "=== 批量翻译完成 ===" | tee -a "$LOG_FILE"
FINAL_COMPLETED=$(ls $OUTPUT_DIR/chunk_*.json 2>/dev/null | wc -l)
echo "已完成: $FINAL_COMPLETED/$TOTAL" | tee -a "$LOG_FILE"
echo "时间: $(date)" | tee -a "$LOG_FILE"

# 如果全部完成，合并结果
if [ "$FINAL_COMPLETED" -eq "$TOTAL" ]; then
    echo "所有分块完成，开始合并..." | tee -a "$LOG_FILE"
    $TRANSLATOR merge \
        --input-dir "$OUTPUT_DIR" \
        --output "es-translator/translated_data.json" \
        --source "data"
    echo "合并完成: es-translator/translated_data.json" | tee -a "$LOG_FILE"
fi
