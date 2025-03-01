/*
    WARNING: Needs library RFID_MFRC522v2 2.0.6
*/

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <WiFi.h>
#include "SPIFFS.h"

const char* ssid     = "SSID";
const char* password = "PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

#define IDENTIFIERS_FILE "/identifiers.bin"

unsigned long* ids_array = NULL;
int num_ids = 0;

MFRC522DriverPinSimple ss_pin(10); // Configurable, see typical pin layout above.
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
MFRC522 mfrc522{driver};  // Create MFRC522 instance.


void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

int Show_RFID_ids() {

  byte buffer[4];

  File file = SPIFFS.open(IDENTIFIERS_FILE, FILE_READ);
  if (!file) {
    Serial.println("No stored IDs");
    return -1;
  }

  int num_bytes = file.size();
  // Serial.println("File size: " + String(num_bytes) + " bytes");

  int num_ids = num_bytes / 4;

  Serial.println("List of IDs:");

  for (int i=0; (i<num_ids); i++) {

    int bytesRead = file.read(buffer, 4);
    if (bytesRead < 4) {
      Serial.println("Error reading file or file is shorter than expected.");
      return -1;
    }

    printHex(buffer, 4);
    Serial.println();
  }

  file.close();

  return num_ids;
}

int Show_RFID_ids_json(WiFiClient client) {

  byte buffer[4];

  client.print("[");

  File file = SPIFFS.open(IDENTIFIERS_FILE, FILE_READ);
  if (!file) {
    client.println("No stored IDs");
    num_ids = 0;
  }
  else {
    int num_bytes = file.size();

    int num_ids = num_bytes / 4;

    for (int i=0; (i<num_ids); i++) {

      int bytesRead = file.read(buffer, 4);
      if (bytesRead < 4) {
        Serial.println("Error reading file or file is shorter than expected.");
        return -1;
      }

      if (i>0) client.print(",");

      client.print("\"");

      for (byte j = 0; j < 4; j++) {
        //client.print(buffer[j] < 0x10 ? " 0" : " ");
        client.print(buffer[j], HEX);
      }

      client.print("\"");
    }

    file.close();
  }

  client.print("]");

  return num_ids;
}

int Store_RFID_Id(unsigned long id) {

  unsigned long buffer;

  File file = SPIFFS.open(IDENTIFIERS_FILE, FILE_READ);
  if (!file) {
    file = SPIFFS.open(IDENTIFIERS_FILE, FILE_WRITE);
    file.close();
    Serial.println("File created");
  }
  else {
    int num_bytes = file.size();
    Serial.println("File size: " + String(num_bytes) + " bytes");

    int num_ids = num_bytes / 4;

    for (int i=0; (i<num_ids); i++) {
      file.read((byte *)&buffer, 4);
      if (id==buffer) {
        Serial.println("Found ");
        printHex((byte *)&id, 4);
        Serial.println();
        return 0;
      }
    }

    file.close();
  }

  Serial.println("Appending");
  printHex((byte *)&id, 4);
  Serial.println();

  file = SPIFFS.open(IDENTIFIERS_FILE, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return -1;
  }

  file.write((byte *)&id, 4);

  file.close();

  return 1;
}

void deleteFile(const char* filename) {
  if (SPIFFS.exists(filename)) {
    if (SPIFFS.remove(filename)) {
      Serial.println("File deleted successfully.");
    } else {
      Serial.println("Failed to delete the file.");
    }
  } else {
    Serial.println("File does not exist.");
  }
}

String parseURLPath(const String& request) {
  // Find the beginning of the URL path (after "GET " and before " HTTP/")
  int start = request.indexOf(' ') + 1;
  int end = request.indexOf(' ', start);
  if (start != -1 && end != -1) {
    return request.substring(start, end);
  }
  return "/";
}

void setup() { 

  delay(1000);
  Serial.begin(115200);
  Serial.println();

  SPI.begin(); // Init SPI bus

  mfrc522.PCD_Init();  // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) { // Format if necessary
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }
  Serial.println("SPIFFS mounted successfully");

  Show_RFID_ids();

  // Get SPIFFS info
  size_t totalBytes = SPIFFS.totalBytes(); // Total SPIFFS partition size
  size_t usedBytes = SPIFFS.usedBytes();   // Bytes currently used

  // Calculate free space
  size_t freeBytes = totalBytes - usedBytes;

  // Print information
  Serial.printf("SPIFFS Total Space: %d bytes\n", totalBytes);
  Serial.printf("SPIFFS Used Space: %d bytes\n", usedBytes);
  Serial.printf("SPIFFS Free Space: %d bytes\n", freeBytes);

  WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(1000);
  //  Serial.print(".");
  //}

  server.begin();
}

void loop() {

  WiFiClient client = server.available();

  if (client) {

    Serial.println("New Client.");
    String request = "";

    unsigned long startTime = millis();
    while (client.connected() && (millis() - startTime < 500)) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && request.endsWith("\r\n\r\n")) { // End of the HTTP request
          break;
        }
      }
    }

    Serial.println("Request:");
    Serial.println(request);

    String path = parseURLPath(request);
    Serial.println("Path: " + path);

    // Send HTTP headers
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/json");
    client.println("Connection: close");
    client.println();

    if (path == "/") {
      Show_RFID_ids_json(client);
    } else if (path == "/delete") {
      deleteFile(IDENTIFIERS_FILE);
    } else {
      client.println("{\"error\": \"Path not found\"}");
    }

    client.println();

    client.stop();

    Serial.println("Client disconnected.");
    Serial.println("");
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( !mfrc522.PICC_IsNewCardPresent())
		return;

  // Verify if the NUID has been readed
  if ( !mfrc522.PICC_ReadCardSerial())
    return;

  //MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(MFRC522Debug::PICC_GetTypeName(piccType));

  Serial.println(F("The NUID tag is:"));
  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();

  unsigned long uid;
  memcpy(&uid, mfrc522.uid.uidByte, 4);
  int ret = Store_RFID_Id(uid);
  switch (ret) {
    case 0:  neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); break; // Blue if stored already
    case 1:  neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); break; // Green if it is new
    default: neopixelWrite(RGB_BUILTIN,0,0,0); break; // Red if error
  }
  delay(500);
  neopixelWrite(RGB_BUILTIN,0,0,0); // Off (black) after 0.5s

  Show_RFID_ids();

  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
