#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>


SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo myServo;
int servoPin = 5;
int openTimeDelay = 5000, openingTimeDelay = 7000;
bool doorLocked = true, doorOpening, doorClosing;
unsigned long lastTime = 0;
int turn = 0;

void setup() {
  //myServo.attach(servoPin);
  Serial.begin(57600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop() {
  getFingerprintID();
  if (doorOpening) {
    if (millis() - lastTime >= openingTimeDelay) {
      myServo.detach();
      doorOpening = false;
      lastTime = millis();
      doorLocked = false;
      Serial.println("Door open.");
    }
  }
  if (!doorClosing && !doorLocked) {
    if ( millis() - lastTime >= openTimeDelay ) {
      myServo.attach(servoPin);
      myServo.write(0);
      doorClosing = true;
      lastTime = millis();
      Serial.println("Door closing.");
    }
  }
  if (doorClosing) {
    if (millis() - lastTime >= openingTimeDelay) {
      myServo.detach();
      doorClosing = false;
      lastTime = millis();
      doorLocked = true;
      Serial.println("Door locked.");
    }
  }
  
}





uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_OK) {
    Serial.println("Image taken,proceeding to the next step");
  } else {
    return p;
  }

  p = finger.image2Tz();
  if (p == FINGERPRINT_OK) {
    Serial.println("Image converted, you are almost there");
  } else {
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!,Access granted");
    myServo.attach(servoPin);
    myServo.write(180);
    lastTime = millis();
    doorOpening = true;
  }  else {
    Serial.println("Match not found, Acess Denied");
    return p;
  }
}
