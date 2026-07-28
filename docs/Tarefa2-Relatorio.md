# Tarefa 2 — Compilar e rodar o driver mínimo no notebook

## Objetivo
Compilar o driver mínimo (`probe.c`) e carregá-lo no kernel Linux, fazendo com que o ESP32 seja reconhecido.

## Comandos utilizados

### 1. Identificar os IDs do ESP32
```bash
lsusb
Bus 001 Device 003: ID 10c4:ea60 Silicon Labs CP210x UART Bridge

VendorID: 10c4

ProductID: ea60


### 2- Descarregar o divber padr'ao (cp210x)
sudo rmmod cp210x

### 3-  Compilar o drive
cd smartlamp-kernel-module
make clean
make

## Saída:
LD [M]  probe.ko

## 4 - Carregar o Driver
sudo rmmod probe


## Dificuldades e soluções

Driver padrão bloqueando	sudo rmmod cp210x
Erro de permissão no dmesg	Usar sudo dmesg
IDs USB incorretos	        Confirmar com lsusb e ajustar no probe.c
