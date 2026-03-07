#!/bin/bash
# 并行翻译脚本 - 同时处理多个分块

set -e

CHUNK_DIR="es-translator/chunks"
OUTPUT_DIR="es-translator/translated_chunks"
TRANSLATOR="./es-translator/target/release/es-translator"
LOG_DIR="es-translator/logs"

# 并行度（同时处理的分块数）
PARALLEL=3

# 确保目录存在
mkdir -p "$OUTPUT_DIR" "$LOG_DIR"

# 加载环境变量
source ~/cc/.env

# 获取所有未翻译的分块
get_pending_chunks() {
    for f in "$CHUNK_DIR"/chunk_*.json; do
        local basename=$(basename "$f")
        if [ ! -f "$OUTPUT_DIR/$basename" ]; then
            echo "$f"
        fi
    done
}

# 翻译单个分块
translate_chunk() {
    local input_file=$1
    local basename=$(basename "$input_file")
    local output_file="$OUTPUT_DIR/$basename"
    local log_file="$LOG_DIR/${basename%.json}.log"

    echo "[$(date '+%H:%M:%S')] 开始翻译 $basename"

    if $TRANSLATOR translate --input "$input_file" --output "$output_file" > "$log_file" 2>&1; then
        echo "[$(date '+%H:%M:%S')] ✓ 完成 $basename"
        return 0
    else
        echo "[$(date '+%H:%M:%S')] ✗ 失败 $basename"
        return 1
    fi
}

# 主循环
echo "=== 并行翻译 ==="
echo "并行度: $PARALLEL"
echo ""

PENDING_CHUNKS=($(get_pending_chunks))
TOTAL=${#PENDING_CHUNKS[@]}

echo "待翻译分块: $TOTAL"
echo ""

# 使用 xargs 并行处理
printf '%s\n' "${PENDING_CHUNKS[@]}" | xargs -P $PARALLEL -I {} bash -c '
    input_file="{}"
    basename=$(basename "$input_file")
    output_file="es-translator/translated_chunks/$basename"
    log_file="es-translator/logs/${basename%.json}.log"

    if [ -f "$output_file" ]; then
        echo "跳过 $basename (已存在)"
        exit 0
    fi

    echo "[$(date +%H:%M:%S)] 开始 $basename"

    if ./es-translator/target/release/es-translator translate --input "$input_file" --output "$output_file" > "$log_file" 2>&1; then
        echo "[$(date +%H:%M:%S)] ✓ 完成 $basename"
    else
        echo "[$(date +%H:%M:%S)] ✗ 失败 $basename"
    fi
'

echo ""
echo "=== 并行翻译完成 ==="
COMPLETED=$(ls $OUTPUT_DIR/chunk_*.json 2>/dev/null | wc -l)
echo "已完成: $COMPLETED/$(ls $CHUNK_DIR/chunk_*.json | wc -l)"
