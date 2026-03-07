#!/bin/bash
# 自动重试翻译缺失的分块

source ~/cc/.env

translate_chunk() {
    local chunk=$1
    local output="es-translator/translated_chunks/$chunk"
    local input="es-translator/chunks/$chunk"
    local retries=0
    local max_retries=50

    if [ -f "$output" ]; then
        echo "✓ $chunk 已存在"
        return 0
    fi

    echo "开始翻译 $chunk..."

    while [ $retries -lt $max_retries ]; do
        if ./es-translator/target/release/es-translator translate \
            --input "$input" \
            --output "$output" 2>&1; then
            if [ -f "$output" ]; then
                echo "✓ $chunk 完成"
                return 0
            fi
        fi

        retries=$((retries + 1))
        echo "重试 $retries/$max_retries..."
        sleep 5
    done

    echo "✗ $chunk 达到最大重试次数"
    return 1
}

echo "=== 翻译缺失分块 ==="
echo "时间: $(date)"

translate_chunk "chunk_0027.json"
translate_chunk "chunk_0028.json"

echo ""
echo "=== 完成 ==="
echo "时间: $(date)"
echo "已完成分块: $(ls es-translator/translated_chunks/*.json 2>/dev/null | grep -v checkpoint | wc -l)/41"

# 如果全部完成，合并
COMPLETED=$(ls es-translator/translated_chunks/*.json 2>/dev/null | grep -v checkpoint | wc -l)
if [ "$COMPLETED" -eq 41 ]; then
    echo ""
    echo "所有分块完成，开始合并..."
    ./es-translator/target/release/es-translator merge \
        --input-dir es-translator/translated_chunks \
        --output es-translator/translated_data.json \
        --source "data"
fi