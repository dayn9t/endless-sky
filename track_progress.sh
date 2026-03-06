#!/bin/bash
# 进度跟踪脚本

LOG="es-translator/progress_tracker.log"
TOTAL=41

echo "=== 进度跟踪器启动 ===" | tee -a "$LOG"
echo "时间: $(date)" | tee -a "$LOG"
echo "" | tee -a "$LOG"

while true; do
    COMPLETED=$(ls es-translator/translated_chunks/ 2>/dev/null | wc -l)
    PERCENT=$((COMPLETED * 100 / TOTAL))

    # 计算预计剩余时间
    if [ "$COMPLETED" -gt 0 ]; then
        # 假设每个分块约 25 分钟
        REMAINING_MINUTES=$(( (TOTAL - COMPLETED) * 25 ))
        REMAINING_HOURS=$((REMAINING_MINUTES / 60))
        REMAINING_MINS=$((REMAINING_MINUTES % 60))
        ETA="约 ${REMAINING_HOURS}小时 ${REMAINING_MINS}分钟"
    else
        ETA="计算中..."
    fi

    echo "[$(date '+%H:%M:%S')] 进度: $COMPLETED/$TOTAL (${PERCENT}%) - 预计剩余: $ETA" | tee -a "$LOG"

    if [ "$COMPLETED" -eq "$TOTAL" ]; then
        echo "" | tee -a "$LOG"
        echo "🎉 翻译完成！" | tee -a "$LOG"
        echo "时间: $(date)" | tee -a "$LOG"
        break
    fi

    # 每 10 分钟报告一次
    sleep 600
done
