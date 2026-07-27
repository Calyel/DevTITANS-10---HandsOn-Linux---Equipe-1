#!/bin/bash
echo "Carregando driver SmartLamp..."
sudo insmod smartlamp-kernel-module/probe.ko
echo "Verificando dmesg..."
dmesg | tail -20
