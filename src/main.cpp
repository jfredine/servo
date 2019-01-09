#include <Arduino.h>
#include <Servo.h>

Servo servo;

void setup() {
  servo.attach(9);
  Serial.begin(9600);
}

#define BUFFER_SIZE 10
void loop() {
  static int pos = 0;
  static char buffer[BUFFER_SIZE];
  static int buffer_index;
  static char last_char = '\0';

  int new_pos;
  new_pos = pos;

  // update desired servo position
  for (int i=Serial.available(); i > 0; i--) {
    char c = Serial.read();
    Serial.write(c);

    if (buffer_index == (BUFFER_SIZE - 1)) {
      Serial.println("Error: Buffer overflow.  Ignoring data");
      buffer_index++;
    }
    else {
      buffer[buffer_index] = c;

      // end of line -- process data
      if ((c == '\n') || (c == '\r')) {
        if ((last_char != '\r') and (last_char != '\n')) {
          buffer[buffer_index] = '\0';
          if (buffer_index > 0) {
            buffer_index = 0;
            int i = atoi(buffer);
            if ((i < 35) || (i > 150)) {
              Serial.println("Error: Value outside limits of [35,150]");
            } else {
              new_pos = i;
            }
          }
        }
      }

      // digit -- add to buffer
      else if (isDigit(c)) {
        buffer[buffer_index] = c;
        buffer_index++;
      }

      // illegal character
      else  {
        Serial.println("Ignoring illegal character: ");
        Serial.println((int)c);
      }
    }
    last_char = c;
  }

  if (pos != new_pos) {
    pos = new_pos;
    servo.write(pos);
  }

  delay(10);
}