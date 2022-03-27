/*
 * Tutorials and code extracted from: 
 *    Detect RFID car removal: 
 *       https://github.com/Martin-Laclaustra/MFRC522-examples/blob/main/UIDRemovalDetection/UIDRemovalDetection.ino
 *    Security Access using MFRC522 RFID Reader with Arduino:
 *       https://randomnerdtutorials.com/security-access-using-mfrc522-rfid-reader-with-arduino/
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino          ESP32 
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------     
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST              (D)22
 * SPI SS      SDA(SS)      10            53        D10        10               10               (D)5
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16               (D)23
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14               (D)19
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15               (D)18
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <PubSubClient.h>
#include <WiFi.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "";
const char* password = "";
// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
//// MQTT broker////



#define RST_PIN         22          // Configurable, see typical pin layout above
#define SS_PIN          5         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

bool locked = false;
bool Jar_1 = false;
bool Jar_2 = false;
 
void setup() 
{
  Serial.begin(115200);   // Initiate a serial communication
  /// wifi///
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  /// wifi ////
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  ////MQTT////
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ////MQTT////
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      //digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      //digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




void loop() 
{
if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

  
// Wake up all cards present within the sensor/reader range.
  bool cardPresent = PICC_IsAnyCardPresent();
  
  // Reset the loop if no card was locked an no card is present.
  // This saves the select process when no card is found.
  if (! locked && ! cardPresent)
    return;

  // When a card is present (locked) the rest ahead is intensive (constantly checking if still present).
  // Consider including code for checking only at time intervals.

  // Ask for the locked card (if mfrc522.uid.size > 0) or for any card if none was locked.
  // (Even if there was some error in the wake up procedure, attempt to contact the locked card.
  // This serves as a double-check to confirm removals.)
  // If a card was locked and now is removed, other cards will not be selected until next loop,
  // after mfrc522.uid.size has been set to 0.
  MFRC522::StatusCode result = mfrc522.PICC_Select(&mfrc522.uid,8*mfrc522.uid.size);

  if(!locked && result == MFRC522::STATUS_OK)
  {
    //Show UID on serial monitor
    Serial.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    
    if (content.substring(1) == "04 10 31 EA F4 6B 81"){ 
    Serial.println("Jar 1 present");
    Jar_1=true;
    locked=true;
    }

    if (content.substring(1) == "04 0B 31 EA F4 6B 81"){ 
    Serial.println("Jar 2 present");
    Jar_2=true;
    locked=true;
    }
    
  } else if(Jar_1 && result != MFRC522::STATUS_OK)
  {
    Jar_1=false;
    locked=false;
    mfrc522.uid.size = 0;
    // Action on card removal.
    /*
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);
    */
    Serial.print(F("Jar 1 removed! Reason for unlocking: "));
    Serial.println(mfrc522.GetStatusCodeName(result));  
    /// mqtt message to appear at mqtt broker///
    client.publish("esp32/Jar1","Jar 1 removed!");
    /// mqtt message to appear at mqtt broker///
  } 
  else if(Jar_2 && result != MFRC522::STATUS_OK)
  {
    Jar_2=false;
    locked=false;
    mfrc522.uid.size = 0;
    // Action on card removal.
    /*
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);
    */
    Serial.print(F("Jar 2 removed! Reason for unlocking: "));
    Serial.println(mfrc522.GetStatusCodeName(result));
    /// mqtt message to appear at mqtt broker///
    client.publish("esp32/Jar2","Jar 2 removed!");
    /// mqtt message to appear at mqtt broker///
  }
  else if(!locked && result != MFRC522::STATUS_OK)
  {
    // Clear locked card data just in case some data was retrieved in the select procedure
    // but an error prevented locking.
    mfrc522.uid.size = 0;
  }

  mfrc522.PICC_HaltA();
}
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(((buffer[i])>>4)&0x0F,  HEX);
    Serial.print(buffer[i]&0x0F, HEX);
    Serial.print(" ");
  }
}

// This convenience function could be added to the library in the future

/**
 * Returns true if a PICC responds to PICC_CMD_WUPA.
 * All cards in state IDLE or HALT are invited.
 * 
 * @return bool
 */
bool PICC_IsAnyCardPresent() {
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  
  // Reset baud rates
  mfrc522.PCD_WriteRegister(mfrc522.TxModeReg, 0x00);
  mfrc522.PCD_WriteRegister(mfrc522.RxModeReg, 0x00);
  // Reset ModWidthReg
  mfrc522.PCD_WriteRegister(mfrc522.ModWidthReg, 0x26);
  
  MFRC522::StatusCode result = mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
  return (result == MFRC522::STATUS_OK || result == MFRC522::STATUS_COLLISION);
} // End PICC_IsAnyCardPresent()
