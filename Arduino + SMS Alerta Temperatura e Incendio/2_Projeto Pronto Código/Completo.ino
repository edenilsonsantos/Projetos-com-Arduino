

#include <SPI.h>
//#include <WiFi101.h>
//#include <WiFiSSLClient.h>
#include <TembooSSL.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contem Informações + token da conta Temboo
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include "DHT.h"
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

// Pinos sensor temp
#define DHTPIN 4 
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
int tempc = 0;

// Pinos sensor de chama
int d = 0; 
int led = 13; 
int digital = 2;


// Pinos sensor de fumaça
int pin_d0 = 3;
int pin_a0 = A2;
int nivel_sensor = 180;

// Pinos Relay
int porta_rele8 = 9; // Exaustores
int porta_rele7 = 8; // Switch de rede
int porta_rele6 = 7; // Disparador de CO2
int porta_rele5 = 6; // Alarme e Giroflex
int porta_rele4 = 5;



EthernetServer server(80);
static byte mac[] = { 0xFE, 0xAD, 0xBE, 0xEF, 0xFE, 0x3D };  //Mac Address 
static byte ip[] = { 192, 168, 43, 230 };  

EthernetClient client;

int calls = 1;   
int maxCalls = 11;  



void setup(){
Serial.begin(9600);
	dht.begin();
  delay(100);
	Serial.println("DHTxx test!");
		Ethernet.begin(mac,ip); 
		server.begin();
			pinMode(digital, INPUT);
			pinMode(led, OUTPUT);  
			digitalWrite(led,LOW);
			pinMode(pin_d0, INPUT);
			pinMode(pin_a0, INPUT);	

  pinMode(porta_rele8, OUTPUT);
  pinMode(porta_rele7, OUTPUT);
  pinMode(porta_rele6, OUTPUT);
  pinMode(porta_rele5, OUTPUT);
  pinMode(porta_rele4, OUTPUT);
  digitalWrite(porta_rele8, HIGH);  //Desliga rele 8
  digitalWrite(porta_rele7, LOW);   //Liga rele 7
  digitalWrite(porta_rele6, HIGH);  //Desliga rele 6
  digitalWrite(porta_rele5, HIGH);  //Desliga rele 5
  digitalWrite(porta_rele4, HIGH);  //Desliga rele 4

  // Para debugging, Aguarde serial conectada
  delay(4000);
  while(!Serial);
  Serial.println("OK");
  
  // Display Oled Setup
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ){
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  delay(10);
}


////////////////////// - INICIO FUNÇÕES DO DISPLAY OLED  - ///////////////////////

void draw() 
{
  //Comandos graficos para o display devem ser colocados aqui
  //u8g.drawStr( quantd caracters a esquerda, numeros de linha, "texto");
  
  //Seleciona a fonte de texto
  u8g.setFont(u8g_font_8x13B);
  //Linha superior 
  u8g.drawStr( 20, 15, "Temperatura");
  // Nova Fonte de texto
  u8g.setFont(u8g_font_fub30);
  //Linha temperatura 
  char buf[9];
  sprintf (buf, "%d", tempc);
  u8g.drawStr(20, 57, buf);
  //u8g.drawStr( 20, 57, "23");
  u8g.drawCircle(67,24,3);
  u8g.drawStr( 79, 57, "C");
  //moldura
  u8g.drawRFrame(2,18, 126, 46, 4);
}


////////////////////// - FIM FUNÇÕES DO DISPLAY OLED  - /////////////////////// 




void loop(){
	
  delay(1000);
  
  ///--- INICIO SENSOR DE FUMAÇA ---///
	int valor_digital = digitalRead(pin_d0);
	int valor_analogico = analogRead(pin_a0);
 
	Serial.print("Pino D0 : ");
	Serial.print(valor_digital);
	Serial.println(" Pino A0 : ");
	Serial.print(valor_analogico);
	Serial.println();
	// Verifica o nivel de gas/fumaca detectado
	if (valor_analogico > nivel_sensor){
		Serial.println("Fumaca Detectada");
		delay(1000);
		digitalWrite(porta_rele7, HIGH);  //Desliga rele 7 switch
		digitalWrite(porta_rele8, HIGH);  //Desliga rele 8 exaustores
		digitalWrite(porta_rele6, LOW);   //Liga rele 6 motor disparador de CO2
		delay(100);
		digitalWrite(porta_rele6, HIGH);   //Desliga rele 6 motor disparador de CO2
		digitalWrite(porta_rele5, LOW);   //Liga rele 5 alarme e giroflex
		delay(100);
	}else{
		Serial.println("Tudo Ok sem fumaça");
		delay(1000);
	}
  ///--- FINAL SENSOR DE FUMAÇA ---///
  
  ///--- INICIO SENSOR DE FOGO ---///
	int d = digitalRead(digital);  
		if(d != 1){
			digitalWrite(led, HIGH);
			Serial.println("Alerta! Fogo Detectado");
			delay(1000);
			digitalWrite(porta_rele7, HIGH);  //Desliga rele 7 switch
			digitalWrite(porta_rele8, HIGH);  //Desliga rele 8 exaustores
			digitalWrite(porta_rele6, LOW);   //Liga rele 6 disparador de CO2
			delay(100);
			digitalWrite(porta_rele6, HIGH);   //Desliga rele 6 motor disparador de CO2
			digitalWrite(porta_rele5, LOW);   //Liga rele 5 alarme e giroflex
			delay(100);			
		}else{
			digitalWrite (led, LOW);
			Serial.println("Ambiente Normal");
			delay(2000);
		}
	delay(500);
  ///--- FINAL SENSOR DE FOGO ---///

  ///--- INICIO SENSOR DE TEMPERATURA ---///

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    tempc=00;                                       
    //Serial.println("Failed to read from DHT");
  } 
  else 
  {
    tempc=(int)t;                                       
    //Serial.print("Temperatura: ");
    //Serial.print(t);
    
  }
  ///--- FINAL SENSOR DE TEMPERATURA ---///


  
  /// --- INICIO MOSTRAR DISPLAY OLED --- ///

	u8g.firstPage();  
		do 
		{
		draw();
		} while( u8g.nextPage() );
		delay(50);
  
  /// --- FINAL MOSTRAR DISPLAY OLED --- ///



  ///--- INICIO IF-TEMPERATURA-ALTA SEND SMS ---///
 if(t > 32){
	 
	 		digitalWrite(porta_rele8, LOW);  //Liga rele 8 - Exautores
			delay(100);
 
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
			String AuthTokenValue = "c4075111efd3970125eb7710baaac111";
			SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
			String FromValue = "+1355554343";
			SendSMSChoreo.addInput("From", FromValue);
			String ToValue = "+5519988554433";
			SendSMSChoreo.addInput("To", ToValue);
			String BodyValue = "Alarme! TEMPERATURA Alta no Rack 1 na fatec Mogi Mirim";
			SendSMSChoreo.addInput("Body", BodyValue);
			String AccountSIDValue = "ACe999cdad688c23ceb5d11155a9aaaba5";
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
		Serial.println("\nAguardando...\n");
		delay(180000); // Espera 3 Min para envio de novo SMS
	}

		if(t <= 32){
			digitalWrite(porta_rele8, HIGH);  //Desliga rele 8 - Exautores
			delay(100);
		}
 
	///--- FINAL IF-TEMPERATURA-ALTA SEND SMS ---///

	
	
	///--- INICIO IF-FOGO-DETECT SEND SMS ---///
 if(d != 1){
 
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
			String AuthTokenValue = "c4075111efd3970125eb7710baaac111";
			SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
			String FromValue = "+1355554343";
			SendSMSChoreo.addInput("From", FromValue);
			String ToValue = "+5519988554433";
			SendSMSChoreo.addInput("To", ToValue);
			String BodyValue = "Alarme! TEMPERATURA Alta no Rack 1 na fatec Mogi Mirim";
			SendSMSChoreo.addInput("Body", BodyValue);
			String AccountSIDValue = "ACe999cdad688c23ceb5d11155a9aaaba5";
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
		Serial.println("\nAguardando...\n");
		delay(180000); // Espera 3 Min para envio de novo SMS
	}
	///--- FINAL IF-FOGO-DETECT SEND SMS ---///
	
	///--- INICIO IF-FUMAÇA-DETECT SEND SMS ---///
  
 if (valor_analogico > nivel_sensor){
 
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
			String AuthTokenValue = "c4075111efd3970125eb7710baaac111";
			SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
			String FromValue = "+1355554343";
			SendSMSChoreo.addInput("From", FromValue);
			String ToValue = "+5519988554433";
			SendSMSChoreo.addInput("To", ToValue);
			String BodyValue = "Alarme! TEMPERATURA Alta no Rack 1 na fatec Mogi Mirim";
			SendSMSChoreo.addInput("Body", BodyValue);
			String AccountSIDValue = "ACe999cdad688c23ceb5d11155a9aaaba5";
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
		Serial.println("\nAguardando...\n");
		delay(180000); // Espera 3 Min para envio de novo SMS
  }

	///--- FINAL IF-FUMAÇA-DETECT SEND SMS ---///
	
	
	
}
























