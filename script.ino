#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>

// Definição dos pinos dos sensores
#define ONE_WIRE_BUS 2
#define TRIG_PIN 8
#define ECHO_PIN 9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;

unsigned long previousMillis = 0; // Variável para armazenar o tempo da última leitura
const long interval = 14400000; // Intervalo de 4 horas em milissegundos

void setup() {
  Serial.begin(9600);

  // Inicializa o sensor DS18B20
  sensors.begin();
  
  // Inicializa os pinos do sensor ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializa a comunicação I2C e o RTC
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("Não foi possível encontrar o RTC");
    while (1); // Para a execução se o RTC não for encontrado
  }

  // Verifica se o RTC está funcionando
  if (!rtc.isrunning()) {
    Serial.println("O RTC NÃO está funcionando, ajustando o tempo...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajusta o RTC para a hora de compilação
  }
}

void loop() {
  unsigned long currentMillis = millis();
  DateTime now = rtc.now();

  // Verifica se passou 4 horas desde a última leitura
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Realiza a leitura da temperatura e do nível do mar
    float temperatura = lerTemperatura();
    float nivelMar = lerNivelMar();

    // Imprime as informações
    Serial.print("Horário: ");
    imprimirDataHora(now);
    Serial.print(", Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C, Nível do mar: ");
    Serial.print(nivelMar);
    Serial.println(" cm");
  }
}

float lerTemperatura() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

float lerNivelMar() {
  // Dispara o sensor ultrassônico
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Lê o tempo de eco
  long duracao = pulseIn(ECHO_PIN, HIGH);

  // Calcula a distância
  return (duracao * 0.0343) / 2;
}

// Função para imprimir a data e hora
void imprimirDataHora(DateTime dt) {
  Serial.print(dt.year(), DEC);
  Serial.print('/');
  Serial.print(dt.month(), DEC);
  Serial.print('/');
  Serial.print(dt.day(), DEC);
  Serial.print(" ");
  Serial.print(dt.hour(), DEC);
  Serial.print(':');
  Serial.print(dt.minute(), DEC);
  Serial.print(':');
  Serial.print(dt.second(), DEC);
}