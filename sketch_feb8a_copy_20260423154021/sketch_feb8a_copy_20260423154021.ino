#define MOISTURE_WET_VALUE 400
#define MOISTURE_DRY_VALUE 3800
#include <SimpleDHT.h>

const int dataPin = 32;
const int NUM_SAMPLES = 10;
const int tempPin= 34;
int pinDHT11 = ;
SimpleDHT11 dht11(pinDHT11);

int readings[NUM_SAMPLES];
int readIndex = 0;
long total = 0;

int moistureValue = 0;
int moisturePercentage = 0;

int smoothedRead();

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // 12-bit = 0 to 4095
  
  // Pre-fill buffer
  for (int i = 0; i < NUM_SAMPLES; i++) {
    readings[i] = analogRead(dataPin);
    total += readings[i];
    delay(10);
  }
}

void loop() {
  moistureValue = smoothedRead();

  Serial.print("Moisture Raw Value: ");
  Serial.println(moistureValue);

  moisturePercentage = map(moistureValue, MOISTURE_WET_VALUE, MOISTURE_DRY_VALUE, 100, 0);
  moisturePercentage = constrain(moisturePercentage, 0, 100);

  Serial.print("Moisture Percentage: ");
  Serial.print(moisturePercentage);
  Serial.println("%");


  Serial.println("=================================");
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  delay(3000);
}

int smoothedRead() {
  total -= readings[readIndex];
  readings[readIndex] = analogRead(dataPin);
  total += readings[readIndex];
  readIndex = (readIndex + 1) % NUM_SAMPLES;
  return total / NUM_SAMPLES;
}