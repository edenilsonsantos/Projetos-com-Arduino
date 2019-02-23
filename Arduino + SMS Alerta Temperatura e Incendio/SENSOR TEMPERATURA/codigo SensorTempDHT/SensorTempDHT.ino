

#include <SPI.h>
//#include <WiFi101.h>
//#include <WiFiSSLClient.h>
#include <TembooSSL.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>

#include "DHT.h"

#define DHTPIN 4     // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE);


EthernetServer server(80);
static byte mac[] = { 0xFE, 0xAD, 0xBE, 0xEF, 0xFE, 0x3D };  //Mac Address for Arduino Ethernet Shield
static byte ip[] = { 192, 168, 15, 125 };  

EthernetClient client;

int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 10;   // Maximum number of times the Choreo should be executed

void setup() {

  
  Serial.println("DHTxx test!");

  dht.begin();
  Serial.begin(9600);
      Ethernet.begin(mac, ip); 
    server.begin();
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  Serial.println("OK");

}

void loop() {


  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //Serial.print("Humidity: ");
  //Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  //Serial.print(f);
 // Serial.print(" *F\t");
 // Serial.print("Heat index: ");
  //Serial.print(hic);
  //Serial.print(" *C ");
  //Serial.print(hif);
  //Serial.println(" *F");


 if(t > 30){

  
  if (calls <= maxCalls) {
    Serial.println("Running SendSMS - Run #" + String(calls++));
    TembooChoreo SendSMSChoreo(client);
    // Invoke the Temboo client
    SendSMSChoreo.begin();

    // Set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);
    SendSMSChoreo.setDeviceType(TEMBOO_DEVICE_TYPE);

    // Set Choreo inputs
    String AuthTokenValue = "c4075641efd3970125eb7710badac424";
    SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
    String FromValue = "+13317012585";
    SendSMSChoreo.addInput("From", FromValue);
    String ToValue = "+5519983504100";
    SendSMSChoreo.addInput("To", ToValue);
    String BodyValue = "Alarme! Temperatura Alta no Rack 1 na fatec Mogi Mirim";
    SendSMSChoreo.addInput("Body", BodyValue);
    String AccountSIDValue = "ACe996cdad688c23ceb5d88055a9f3aba5";
    SendSMSChoreo.addInput("AccountSID", AccountSIDValue);

    // Identify the Choreo to run
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // Run the Choreo; when results are available, print them to serial
    SendSMSChoreo.run();

    while(SendSMSChoreo.available()) {
      char c = SendSMSChoreo.read();
      Serial.print(c);
    }
    SendSMSChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(100000); // wait 30 seconds between SendSMS calls
 }
}
























