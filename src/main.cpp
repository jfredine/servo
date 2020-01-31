#include <Arduino.h>
#include <Servo.h>
#include <stdlib.h>

Servo servo;
static bool try_read_line(Stream &stream,
                          char *buffer, unsigned buffer_size, char **end);

void setup() {
    servo.attach(9);
    Serial.begin(9600);
}

#define BUFFER_SIZE 10
void loop() {
    static int pos = 0;
    static char buffer[BUFFER_SIZE];
    int new_pos = pos;
    char *end;

    // get input which may take multiple loops or have buffer overflow errors
    char *old_end = &buffer[strlen(buffer)];
    bool complete = try_read_line(Serial, buffer, BUFFER_SIZE, &end);
    if (end != old_end) {
        Serial.write(old_end);
    }
    if (complete) {
        Serial.println("");
    } else {
        if ((end - buffer) == (BUFFER_SIZE - 1)) {
            // overflow
            Serial.println("");
            Serial.println("Error: Buffer overflow.  Ignoring data");
            buffer[0] = '\0';
            return;
        } else {
            // not enough characters
            delay(100);
            return;
       }
    }

    // complete line of input to process
    char *cp;
    int i = strtoul(buffer, &cp, 0);
    if (cp != end) {
        Serial.println("Error: Input must be an integer in the range [35,150]");
    } else if ((i < 35) || (i > 150)) {
        Serial.println("Error: Input must be an integer in the range [35,150]");
    } else {
        new_pos = i;
    }

    if (pos != new_pos) {
        pos = new_pos;
        servo.write(pos);
    }
    buffer[0] = '\0';
}


//
// try_read_line
//
// Arguments: stream - input stream to read (e.g. Serial)
//            buffer - character buffer for collecting input
//            buffer_size - max number of characters which may be written
//                          to the buffer
//            end - pointer to last character written ('\0') in buffer            
// Returns: true - a complete line was read
//          falsle - a complete line was not read
//
// This routine attempts to read a line of input from the provided stream.
// A line is assumed to be completed by a '\r' or '\n' character.  Detection of
// one of these characters as the first character in a line is ignored.  This
// allows for lines terminated by \n, \r, \r\n to all be recognized.  It also
// means that empty lines are ignored.  A line may not be fully recognized if:
//
//     1) No more characters are available from the input stream and no
//        end of line character was found.
//
//     2) The max buffer size was reached before an end of line character
//        was found.
//
// The user can check the length of the buffer if false is returned
// to detect condition #2.  Otherwise false means there were not enough
// characters in the input stream to complete a line.
//
// The routine appends results to the buffer with each call so the first call
// should be made with an empty buffer (buffer[0] = '\0') and subsequent calls
// can be made with partial lines until true is returned or the buffer is full.
//
// At exit from this routine the buffer will be null terminated and the end
// pointer (if not NULL) will be set to the null character.  This can be used
// to check the size of the buffer.
//

bool try_read_line(Stream &stream,
                   char *buffer, unsigned buffer_size, char **end) {
    if ((buffer == NULL) || (buffer_size == 0)) {
        return false;
    }

    unsigned buffer_index = strlen(buffer);
    while (buffer_index < (buffer_size - 1)) {
        if (stream.available()) {
            char c = stream.read();
            if ((c == '\n') || (c == '\r')) {
                if (buffer_index == 0) {
                    continue;
                } else {
                    buffer[buffer_index] = '\0';
                    if (end) {
                        *end = &buffer[buffer_index];
                    }
                    return true;
                }
            } else {
                buffer[buffer_index] = c;
                buffer_index++;
            }
        } else {
            break;
        }
    }

    buffer[buffer_index] = '\0';
    if (end) {
        *end = &buffer[buffer_index];
    }
    return false;
}
