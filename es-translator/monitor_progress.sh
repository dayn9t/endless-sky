#!/bin/bash
# 监控 es-translator 翻译进度

LOG_FILE="es-translator/translation.log"

echo "=== Endless Sky 翻译进度监控 ==="
echo ""

# 检查进程是否运行
if pgrep -f "es-translator translate" > /dev/null; then
    echo "✅ 翻译进程正在运行"
else
    echo "❌ 翻译进程未运行"
    exit 1
fi

# 显示最新进度
echo ""
echo "📊 当前进度:"
tail -5 "$LOG_FILE"

# 计算进度百分比
CURRENT_BATCH=$(tail -1 "$LOG_FILE" | grep -oP 'batch \K\d+' || echo "0")
TOTAL_BATCHES=1001

if [ "$CURRENT_BATCH" -gt 0 ]; then
    PERCENTAGE=$((CURRENT_BATCH * 100 / TOTAL_BATCHES))
    echo ""
    echo "进度: $CURRENT_BATCH / $TOTAL_BATCHES 批次 ($PERCENTAGE%)"

    # 估算剩余时间（假设每批次 1 秒）
    REMAINING=$((TOTAL_BATCHES - CURRENT_BATCH))
    MINUTES=$((REMAINING / 60))
    SECONDS=$((REMAINING % 60))
    echo "预计剩余时间: ${MINUTES}分${SECONDS}秒"
fi

echo ""
echo "💡 提示: 使用 'tail -f $LOG_FILE' 实时查看进度"
