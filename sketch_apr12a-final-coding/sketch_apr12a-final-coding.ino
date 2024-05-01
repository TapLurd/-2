#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 8
#define GREEN_LED_PIN A4
#define RED_LED_PIN A2
#define BUZZER_PIN A3

MFRC522 mfrc522(SS_PIN, RST_PIN);
String authorizedRFID = "43c74c25";
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A0, 2, 3, 4};
byte colPins[COLS] = {5, 6, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Servo myServo;

void setup() {
  Serial.begin(9600);
  Serial.println("System Ready.");

  SPI.begin();
  mfrc522.PCD_Init();

  myServo.attach(SERVO_PIN);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {

  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      String rfidTag = getRFIDTag();
      Serial.println("RFID Card Detected: " + rfidTag);
      
      if (rfidTag.equals(authorizedRFID)) {
       
        Serial.println("Authorized RFID Card Detected.");
        digitalWrite(GREEN_LED_PIN, HIGH);
        tone(BUZZER_PIN, 1000);
        delay(1000);
        digitalWrite(GREEN_LED_PIN, LOW);
        noTone(BUZZER_PIN);
  
        String passcode = getPasscode(keypad); 
        Serial.println("Entered Passcode: " + passcode);
  
        if (passcode.equals("3333")) {
        
          Serial.println("Passcode Correct. Access Granted!");
          digitalWrite(GREEN_LED_PIN, HIGH);
          tone(BUZZER_PIN, 1000);
          delay(1000);
          digitalWrite(GREEN_LED_PIN, LOW);
          noTone(BUZZER_PIN);
          rotateServo(180);
          delay(1000);
          rotateServo(0);
          delay(2000);
          digitalWrite(GREEN_LED_PIN, LOW);
        } else {
          
          Serial.println("Incorrect Passcode. Access Denied!");
          
          digitalWrite(RED_LED_PIN, HIGH);
          tone(BUZZER_PIN, 200);
          delay(300);
          digitalWrite(RED_LED_PIN, LOW);
          noTone(BUZZER_PIN);
          delay(300);
          digitalWrite(RED_LED_PIN, HIGH);
          tone(BUZZER_PIN, 200);
          delay(300);
          digitalWrite(RED_LED_PIN, LOW);
          noTone(BUZZER_PIN);
        }
      } else {

        Serial.println("Unauthorized RFID Card Detected. Access Denied!");
        for (int i = 0; i < 4; i++) {
          digitalWrite(RED_LED_PIN, HIGH);
          tone(BUZZER_PIN, 200);
          delay(300);
          digitalWrite(RED_LED_PIN, LOW);
          noTone(BUZZER_PIN);
          delay(300);
        }
      }
      
      delay(1000); 
    }
  }
}

String getRFIDTag() {
  String tag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  mfrc522.PICC_HaltA();
  Serial.println("RFID Tag: " + tag);
  return tag;
}

String getPasscode(Keypad &keypad) {
  String pass = "";
  while (pass.length() < 4) {
    char key = keypad.getKey();
    if (key && isDigit(key)) {
      pass += key;
      Serial.print(key); 
    }
  }
  Serial.println(); 
  return pass;
}

void rotateServo(int angle) {
  myServo.write(angle);
  delay(5000); 
  Serial.println("Servo rotated to " + String(angle) + " degrees.");
}
