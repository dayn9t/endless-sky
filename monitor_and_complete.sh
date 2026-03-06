#!/bin/bash
# 监控翻译进度并在完成后合并

LOG_FILE="es-translator/completion_monitor.log"
TOTAL=41

echo "=== 翻译完成监控器 ===" | tee -a "$LOG_FILE"
echo "启动时间: $(date)" | tee -a "$LOG_FILE"
echo "总任务: $TOTAL 个分块" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

# 等待所有分块完成
while true; do
    COMPLETED=$(ls es-translator/translated_chunks/chunk_*.json 2>/dev/null | wc -l)
    PROGRESS=$(tail -1 es-translator/bulk_translation.log 2>/dev/null | grep -o '\[.*/41\]' || echo "[?/41]")

    echo "[$(date '+%H:%M:%S')] 进度: $COMPLETED/$TOTAL $PROGRESS" | tee -a "$LOG_FILE"

    if [ "$COMPLETED" -eq "$TOTAL" ]; then
        echo "" | tee -a "$LOG_FILE"
        echo "🎉 所有分块翻译完成！" | tee -a "$LOG_FILE"
        echo "时间: $(date)" | tee -a "$LOG_FILE"
        break
    fi

    # 每 5 分钟检查一次
    sleep 300
done

# 合并翻译结果
echo "" | tee -a "$LOG_FILE"
echo "开始合并翻译结果..." | tee -a "$LOG_FILE"

./es-translator/target/release/es-translator merge \
    --input-dir es-translator/translated_chunks \
    --output es-translator/translated_data.json \
    --source "data" 2>&1 | tee -a "$LOG_FILE"

if [ -f "es-translator/translated_data.json" ]; then
    echo "✓ 合并完成: es-translator/translated_data.json" | tee -a "$LOG_FILE"

    # 生成中文插件
    echo "" | tee -a "$LOG_FILE"
    echo "生成中文插件..." | tee -a "$LOG_FILE"

    ./es-translator/target/release/es-translator generate \
        --type data \
        --input es-translator/translated_data.json 2>&1 | tee -a "$LOG_FILE"

    echo "" | tee -a "$LOG_FILE"
    echo "🎉 所有任务完成！" | tee -a "$LOG_FILE"
    echo "时间: $(date)" | tee -a "$LOG_FILE"
else
    echo "✗ 合并失败" | tee -a "$LOG_FILE"
fi
