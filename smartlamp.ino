// Pinos utilizados
const int ledPin = 23;
const int ldrPin = 4;

// Valor atual do LED entre 0 e 100
int ledValue = 10;

// Valor máximo medido pelo LDR
int ldrMax = 4000;

void setup() {
    Serial.begin(9600);

    pinMode(ledPin, OUTPUT);
    pinMode(ldrPin, INPUT);

    // Inicia o LED com intensidade 10
    ledUpdate();

    Serial.println("SmartLamp Initialized.");
}

// Executada continuamente pelo ESP32
void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');

        // Remove espaços, \n e \r
        command.trim();

        if (command.length() > 0) {
            processCommand(command);
        }
    }
}

// Processa os comandos recebidos
void processCommand(String command) {

    if (command == "GET_LED") {
        Serial.print("RES GET_LED ");
        Serial.println(ledValue);
        return;
    }

    if (command == "GET_LDR") {
        int ldrValue = ldrGetValue();

        Serial.print("RES GET_LDR ");
        Serial.println(ldrValue);
        return;
    }

    if (command == "SET_LED" || command.startsWith("SET_LED ")) {
        String valueText = command.substring(7);
        valueText.trim();

        bool validValue = valueText.length() > 0;

        // Confere se o valor contém apenas números
        for (unsigned int i = 0; i < valueText.length(); i++) {
            if (!isDigit(valueText[i])) {
                validValue = false;
                break;
            }
        }

        if (validValue) {
            int newLedValue = valueText.toInt();

            if (newLedValue >= 0 && newLedValue <= 100) {
                ledValue = newLedValue;
                ledUpdate();

                Serial.println("RES SET_LED 1");
                return;
            }
        }

        Serial.println("RES SET_LED -1");
        return;
    }

    Serial.println("ERR Unknown command.");
}

// Atualiza a intensidade do LED
void ledUpdate() {
    // Converte de 0–100 para 0–255
    int pwmValue = map(ledValue, 0, 100, 0, 255);

    pwmValue = constrain(pwmValue, 0, 255);

    analogWrite(ledPin, pwmValue);
}

// Lê o LDR e normaliza entre 0 e 100
int ldrGetValue() {
    int rawLdrValue = analogRead(ldrPin);

    rawLdrValue = constrain(rawLdrValue, 0, ldrMax);

    int normalizedLdrValue = map(
        rawLdrValue,
        0,
        ldrMax,
        0,
        100
    );

    return constrain(normalizedLdrValue, 0, 100);
}