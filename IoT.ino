const int trigPin1 = 2;
const int echoPin1 = 3;
const int trigPin2 = 6;
const int echoPin2 = 7;
const int trigPin3 = 4;
const int echoPin3 = 5;
#define irTransmitterPin 13

long pulseDuration;
int distLeft, distRight, distMiddle;

#include <IRremote.h>
IRsend irSender;

void setup() {
  Serial.begin(9600);
  irSender.begin(irTransmitterPin);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
}

void calculateDistance(int trigger, int echo) {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  pulseDuration = pulseIn(echo, HIGH);
  dist = pulseDuration * 0.034 / 2;
  if (dist > 60)
    dist = 60;
}

void loop() {
  calculateDistance(trigPin1, echoPin1);
  distLeft = dist;
  calculateDistance(trigPin2, echoPin2);
  distRight = dist;
  calculateDistance(trigPin3, echoPin3);
  distMiddle = dist;

  if ((distLeft > 22 && distRight > 22) && (distLeft < 30 && distRight < 30)) {
    Serial.println("Play/Pause");
    irSender.sendNEC(0x80, 0x1, true);
    delay(500);
  }

  if (distLeft >= 12 && distLeft <= 20) {
    delay(1000);
    calculateDistance(trigPin1, echoPin1);
    distLeft = dist;
    if (distLeft >= 12 && distLeft <= 20) {
      Serial.println("Left Locked");
      bool prevSong = false;
      while (!prevSong) {
        Serial.println("prev song");
        prevSong = true;
        irSender.sendNEC(0x80, 0x2, true);
      }
    }
  }

  if (distRight >= 12 && distRight <= 20) {
    delay(1000);
    calculateDistance(trigPin2, echoPin2);
    distRight = dist;
    if (distRight >= 13 && distRight <= 17) {
      Serial.println("Right Locked");
      bool nextSong = false;
      while (!nextSong) {
        Serial.println("next song");
        nextSong = true;
        irSender.sendNEC(0x80, 0x3, true);
      }
    }
  }

  if (distMiddle >= 12 && distMiddle <= 25) {
    delay(200);
    calculateDistance(trigPin3, echoPin3);
    distMiddle = dist;
    if (distMiddle >= 12 && distMiddle <= 25) {
      Serial.println("Mid Locked");
      int t = millis();
      while (distMiddle <= 50) {
        calculateDistance(trigPin3, echoPin3);
        distMiddle = dist;
        bool volDown = false;
        bool volUp = false;
        if (distMiddle < 15) {
          Serial.println("Vdown");
          delay(300);
          volDown = false;
          while (!volDown) {
            Serial.println("Volume down 5 times");
            irSender.sendNEC(0x80, 0x5, true);
            volDown = true;
            delay(200);
            break;
          }
        }
        if (distMiddle > 22) {
          Serial.println("Vup");
          delay(300);
          volUp = false;
          while (!volUp) {
            Serial.println("Volume up 5 times");
            irSender.sendNEC(0x80, 0x6, true);
            volUp = true;
            delay(200);
            break;
          }
        }
        if (volUp) {
          delay(200);
          break;
        }
        if (volDown) {
          delay(200);
          break;
        }
        if (millis() - t > 2000) {
          break;
        }
      }
      delay(400);
    }
  }
}
