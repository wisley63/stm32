#include <mcp_can.h>
#include <SPI.h>

// Define CAN constants
#define CAN_CS_PIN 10 
#define LED_PIN 13 

MCP_CAN CAN(CAN_CS_PIN);


void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize CAN module for both sender and receiver
  if (CAN.begin(MCP_STDEXT, CAN_1000KBPS, MCP_8MHz) == CAN_OK) {
    Serial.println("CAN module init ok!");
    pinMode(LED_PIN, OUTPUT);

    // Set up a filter to accept frames with ID 0x1800EEF0
    CAN.init_Mask(0, 0, 0x1FFFFFFF); // Mask 0 matches all bits in the ID
    CAN.init_Filt(0, 0, 0x1800EEF0); // Filter 0 matches the desired ID
  } else {
    Serial.println("Error initializing CAN module!");
    while (1);
  }
}


void loop() {
  // Check if the wire is disconnected
  if (CAN_OK == CAN.checkError()) {
    // Send the CAN frame with ID 0x1800EEF0 containing the number 0
    uint8_t data[] = {0};
    CAN.sendMsgBuf(0x1800EEF0, 0, 1, data);
    delay(1000); // Wait for 1 second
  } else {
    // If the wire is disconnected, send a CAN frame with ID 0x1800EEF0 containing the number 1
    uint8_t data[] = {1};
    CAN.sendMsgBuf(0x1800EEF0, 0, 1, data);
    delay(100); // Wait for a short time before sending the error frame again
  }

  // Check if there is a CAN frame available
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    // Read the CAN frame
    uint8_t len = 0;
    uint8_t buf[8];
    if (CAN.readMsgBuf(&len, buf) == CAN_OK) {
      if (len == 1) {
        if (buf[0] == 0) {
          // Data frame contains 0, leave the LED lit indefinitely
          digitalWrite(LED_PIN, HIGH);
        } else if (buf[0] == 1) {
          // Data frame contains 1, toggle the LED
          digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
      }
    }
  }
}
