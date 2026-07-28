

# Tarefa 2.1.1 — GET_LDR no setup()

## Objetivo
Fazer o ESP32 enviar automaticamente o valor do LDR ao iniciar, para que o driver possa ler sem comando manual.

## Implementação
No `setup()` do `smartlamp.ino`, foi adicionado:
```cpp
Serial.print("RES GET_LDR ");
Serial.println(ldrGetValue());

## Validação no Wokwi
## Saida Monitor
SmartLamp Initialized.
RES GET_LDR 25





