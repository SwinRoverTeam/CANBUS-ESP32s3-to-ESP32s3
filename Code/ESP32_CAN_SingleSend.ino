#include "driver/twai.h"

#define CAN_TX GPIO_NUM_43
#define CAN_RX GPIO_NUM_44
#define RECIVE GPIO_NUM_1

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("CAN Echo + Serial Sender Starting...");

  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX, CAN_RX, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK &&
      twai_start() == ESP_OK) {
    Serial.println("CAN Initialized.");
  } else {
    Serial.println("Failed to initialize CAN.");
    while (1);
  }
}

void loop() {
  // --- Check for incoming CAN message ---
  twai_message_t rx_msg;
  if (twai_receive(&rx_msg, pdMS_TO_TICKS(10)) == ESP_OK) {
    Serial.println("Message received");

    Serial.print("ID: 0x");
    Serial.print(rx_msg.identifier, HEX);
    Serial.print(" | Length: ");
    Serial.print(rx_msg.data_length_code);
    Serial.print(" | Data: ");
    for (int i = 0; i < rx_msg.data_length_code; i++) {
      Serial.print((char)rx_msg.data[i]);
    }
    Serial.println();


    
        
  }

  // --- Check for Serial input and send CAN message ---

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // read until Enter pressed
    input.trim();
    if (input.length() > 0) {
      twai_message_t tx_msg = {};
      tx_msg.identifier = 0x123;
      tx_msg.extd = 0;   // standard frame
      tx_msg.rtr = 0;    // not remote request
      tx_msg.data_length_code = min((int)input.length(), 8);

      for (int i = 0; i < tx_msg.data_length_code; i++) {
        tx_msg.data[i] = input[i];
      }

      if (twai_transmit(&tx_msg, pdMS_TO_TICKS(1000)) == ESP_OK) {
        Serial.println("Sent CAN message from Serial input");
      } else {
        Serial.println("Failed to send CAN message from Serial input");
        
      }
    }

  }
}