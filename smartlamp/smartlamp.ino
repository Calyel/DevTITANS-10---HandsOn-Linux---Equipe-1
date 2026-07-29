// Definição dos pinos
const int ledPin = 4;
const int ldrPin = 34;

// Intensidade atual do LED, entre 0 e 100
int ledValue = 10;

// Valor máximo obtido pelo LDR
int ldrMax = 4000;

// Protótipo para tarefa serial_read: envia LDR periodicamente
unsigned long lastLdrMillis = 0;
const unsigned long ldrIntervalMs = 2000;

void setup() {
    Serial.begin(9600);

    pinMode(ledPin, OUTPUT);
    pinMode(ldrPin, INPUT);

    ledUpdate();

    Serial.printf("SmartLamp Initialized.\n");

    // Envia a primeira leitura imediatamente; depois a cada 2 segundos no loop
    Serial.printf("RES GET_LDR %d\n", ldrGetValue());
    lastLdrMillis = millis();
}

// Executada continuamente pelo ESP32
void loop() {
    if (Serial.available() > 0) {
        // Lê o comando até encontrar uma quebra de linha
        String command = Serial.readStringUntil('\n');

        // Remove espaços, \n e \r das extremidades
        command.trim();

        if (command.length() > 0) {
            processCommand(command);
        }
    }

    // Envia RES GET_LDR X automaticamente a cada 2 segundos
    unsigned long now = millis();
    if (now - lastLdrMillis >= ldrIntervalMs) {
        lastLdrMillis = now;
        Serial.printf("RES GET_LDR %d\n", ldrGetValue());
    }
}

// Processa os comandos recebidos pela porta serial
void processCommand(String command) {
    if (command == "GET_LED") {
        Serial.printf("RES GET_LED %d\n", ledValue);
        return;
    }

    if (command == "GET_LDR") {
        int ldrValue = ldrGetValue();
        Serial.printf("RES GET_LDR %d\n", ldrValue);
        return;
    }

    // Verifica se é uma tentativa de executar SET_LED
    if (command == "SET_LED" || command.startsWith("SET_LED ")) {
        // SET_LED possui 7 caracteres.
        // A intensidade começa depois do espaço, na posição 8.
        String valueText = command.substring(7);
        valueText.trim();

        bool validValue = valueText.length() > 0;

        // Confere se todos os caracteres são números
        for (unsigned int i = 0; i < valueText.length(); i++) {
            if (valueText[i] < '0' || valueText[i] > '9') {
                validValue = false;
                break;
            }
        }

        if (validValue) {
            int newLedValue = valueText.toInt();

            if (newLedValue >= 0 && newLedValue <= 100) {
                ledValue = newLedValue;
                ledUpdate();

                Serial.printf("RES SET_LED 1\n");
                return;
            }
        }

        // Valor ausente, não numérico ou fora de 0 a 100
        Serial.printf("RES SET_LED -1\n");
        return;
    }

    // Qualquer outro comando
    Serial.printf("ERR Unknown command.\n");
}

// Atualiza a intensidade do LED
void ledUpdate() {
    // Converte a intensidade de 0–100 para PWM de 0–255
    int normalizedLedValue = map(ledValue, 0, 100, 0, 255);

    normalizedLedValue = constrain(normalizedLedValue, 0, 255);

    analogWrite(ledPin, normalizedLedValue);
}

// Lê e normaliza o valor do LDR
int ldrGetValue() {
    int rawLdrValue = analogRead(ldrPin);

    // Evita resultados acima do máximo configurado
    rawLdrValue = constrain(rawLdrValue, 0, ldrMax);

    // Converte a leitura do LDR para o intervalo de 0 a 100
    int normalizedLdrValue = map(
        rawLdrValue,
        0,
        ldrMax,
        0,
        100
    );

    return constrain(normalizedLdrValue, 0, 100);
}
