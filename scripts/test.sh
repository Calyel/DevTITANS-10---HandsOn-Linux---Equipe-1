#!/bin/bash
echo "=== TESTE SMARTLAMP ==="
echo "Leitura do LDR:"
cat /sys/kernel/smartlamp/ldr
echo ""
echo "Escrevendo LED (50%):"
echo 50 > /sys/kernel/smartlamp/led
echo "Leitura do LED:"
cat /sys/kernel/smartlamp/led
