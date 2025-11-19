/***********************************************
 *  Bi-Directional People Counter with ESP8266
 *  Hardware:
 *  - NodeMCU ESP8266
 *  - IR Sensor 1 (Entry)
 *  - IR Sensor 2 (Exit)
 *  - SG90 Servo Motor
 *  - Blynk IoT Dashboard
 ***********************************************/

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Visitor Counter"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// -------------------- Pin Configuration --------------------
#define IR_ENTRY_PIN  D5      // Entry sensor
#define IR_EXIT_PIN   D6      // Exit sensor
#define SERVO_PIN     D4      // Servo motor

// -------------------- System Variables --------------------
int peopleIn = 0;
int peopleOut = 0;
int totalPeople = 0;

// User-set maximum occupancy (controlled from Blynk)
int maxPeople = 10;

Servo doorServo;

// -------------------- WiFi Credentials --------------------
char ssid[] = "WIFI_NAME";
char pass[] = "WIFI_PASSWORD";

// -------------------- Blynk Control Slider ---------------
BLYNK_WRITE(V3)
{
  maxPeople = param.asInt();
  Serial.print("Updated Max People Limit: ");
  Serial.println(maxPeople);
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(9600);

  pinMode(IR_ENTRY_PIN, INPUT);
  pinMode(IR_EXIT_PIN, INPUT);

  doorServo.attach(SERVO_PIN);
  doorServo.write(0);   // 0 = Door open

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("System Started...");
}

// -------------------- Main Loop --------------------
void loop() {
  Blynk.run();
  countEntry();
  countExit();
  updateDoorState();
  sendValuesToBlynk();
}

// -------------------- Detect Entry --------------------
void countEntry() {
  if (digitalRead(IR_ENTRY_PIN) == LOW) {
    peopleIn++;
    totalPeople = peopleIn - peopleOut;
    Serial.print("Person Entered. Total = ");
    Serial.println(totalPeople);
    delay(600); // Debounce for sensor
  }
}

// -------------------- Detect Exit --------------------
void countExit() {
  if (digitalRead(IR_EXIT_PIN) == LOW) {
    peopleOut++;
    totalPeople = peopleIn - peopleOut;
    Serial.print("Person Exited. Total = ");
    Serial.println(totalPeople);
    delay(600); // Debounce
  }
}

// -------------------- Servo Door Logic --------------------
void updateDoorState() {
  if (totalPeople >= maxPeople) {
    doorServo.write(90);   // Door closed
  } else {
    doorServo.write(0);    // Door open
  }
}

// -------------------- Send Live Data to Blynk --------------
void sendValuesToBlynk() {
  Blynk.virtualWrite(V0, peopleIn);
  Blynk.virtualWrite(V1, peopleOut);
  Blynk.virtualWrite(V2, totalPeople);
}
