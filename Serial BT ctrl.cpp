#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// Pin Definitions
const int LED_PIN = 2;  // D2 pin for LED
const int segPins[] = {26, 27, 13, 12, 14, 25, 33};  // GPIO pins for 7-segment display a->g

// Variables
bool onState = false;                  // Tracks if the LED is on or off
bool blinking = false;                 // Tracks if the LED is in blinking mode
unsigned long previousMillis = 0;      // the last time LED was updated
int blinkTimes = 0;                    // Number of times to blink
int blinkDelay = 0;                    // Delay between blinks
int currentBlinkCount = 0;             // Counter for blinking

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
  onState = true;
  blinking = false;
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
  onState = false;
  blinking = false;
}

void startBlinking(int count, int delayTime) {
  blinking = true;
  blinkTimes = count;
  blinkDelay = delayTime;
  currentBlinkCount = 0;
  previousMillis = millis();
}

void displayNumber(int number) {
  const byte digitPatterns[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
  };
  
  if (number >= 0 && number <= 9) {
    byte segments = digitPatterns[number];
    for (int i = 0; i < 7; i++) {
      digitalWrite(segPins[i], (segments >> i) & 0x01);
    }
  }
}

void shutDown() {
  ledOff();
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], LOW);
  }
}

String toLowercase(String str) {
  str.toLowerCase();
  str.trim();
  return str;
}

bool containsIgnoreCase(String command, String keyword) {
  return toLowercase(command).indexOf(toLowercase(keyword)) != -1;
}

// Helper function to detect and extract numbers
int extractNumber(String command) {
  const String numberWords[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
  for (int i = 0; i < 10; i++) {
    if (containsIgnoreCase(command, numberWords[i])) {
      return i;
    }
  }
  for (char digit = '0'; digit <= '9'; digit++) {
    if (command.indexOf(digit) != -1) {
      return digit - '0';
    }
  }
  return -1; // No number found
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }

  Serial.begin(115200);
  SerialBT.begin("ESP32_LED_Control");
  Serial.println("Bluetooth Device is Ready to Pair");
}

void loop() {
  if (SerialBT.available()) {
    String command = SerialBT.readString();
    command.trim();
    Serial.println("Received command: '" + command + "'");

    // Use lowercase to make parsing case-insensitive
    command = toLowercase(command);

    // Check for LED ON command
    if (containsIgnoreCase(command, "on") && containsIgnoreCase(command, "led")) {
      ledOn();
      SerialBT.println("LED turned ON");

    // Check for LED OFF command
    } else if (containsIgnoreCase(command, "off") && containsIgnoreCase(command, "led")) {
      shutDown();
      SerialBT.println("LED turned OFF");

    // Check for BLINK command
    } else if (containsIgnoreCase(command, "blink") && containsIgnoreCase(command, "led")) {
      int count = 0;
      int delayTime = 0;
      sscanf(command.c_str(), "blink led %d %d", &count, &delayTime);
      SerialBT.printf("Blinking LED %d times with %d ms delay\n", count, delayTime);
      startBlinking(count, delayTime);

    // Check for SHOW NUMBER command
    } else if (containsIgnoreCase(command, "show") && containsIgnoreCase(command, "number")) {
      int number = extractNumber(command);
      if (number != -1) {
        SerialBT.printf("Displaying number %d on 7-segment display\n", number);
        displayNumber(number);
      } else {
        SerialBT.println("No valid number found. Only numbers 0-9 are supported.");
      }

    // Check for SHUTDOWN command
    } else if (containsIgnoreCase(command, "shut") && containsIgnoreCase(command, "down")) {
      shutDown();
      SerialBT.println("System shut down");

    } else {
      SerialBT.println("Unknown command.");
    }
  }

  // Non-blocking LED blinking logic
  if (blinking) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= blinkDelay) {
      previousMillis = currentMillis;
      if (onState) {
        digitalWrite(LED_PIN, LOW);
        onState = false;
        currentBlinkCount++;
      } else {
        digitalWrite(LED_PIN, HIGH);
        onState = true;
      }

      if (currentBlinkCount >= blinkTimes) {
        blinking = false;
        ledOff();
      }
    }
  }
}

