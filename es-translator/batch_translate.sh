#!/bin/bash
# 批量翻译脚本

set -e

CHUNK_DIR="es-translator/chunks"
OUTPUT_DIR="es-translator/translated_chunks"
TRANSLATOR="./es-translator/target/release/es-translator"
TOTAL=$(ls $CHUNK_DIR/chunk_*.json 2>/dev/null | wc -l)

echo "=== 批量翻译 ==="
echo "总共 $TOTAL 个分块"
echo ""

# 加载环境变量
source ~/cc/.env

for i in $(seq -f "%04g" 1 $TOTAL); do
    INPUT_FILE="$CHUNK_DIR/chunk_$i.json"
    OUTPUT_FILE="$OUTPUT_DIR/chunk_$i.json"

    if [ -f "$OUTPUT_FILE" ]; then
        echo "[$i/$TOTAL] chunk_$i.json 已翻译，跳过"
        continue
    fi

    echo "[$i/$TOTAL] 正在翻译 chunk_$i.json..."

    if $TRANSLATOR translate --input "$INPUT_FILE" --output "$OUTPUT_FILE" 2>&1; then
        echo "  ✓ 完成: chunk_$i.json"
    else
        echo "  ✗ 失败: chunk_$i.json，继续下一个"
    fi

    # 每个分块后等待，避免限流
    sleep 2
done

echo ""
echo "=== 批量翻译完成 ==="
COMPLETED=$(ls $OUTPUT_DIR/chunk_*.json 2>/dev/null | wc -l)
echo "已完成: $COMPLETED/$TOTAL"
