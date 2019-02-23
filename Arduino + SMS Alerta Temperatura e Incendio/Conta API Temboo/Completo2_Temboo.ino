
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
#include <OneWire.h>
#include <DallasTemperature.h>
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 29
// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);
// Armazena temperaturas minima e maxima
float tempMin = 999;
float tempMax = 0;
 
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

// Pinos sensor temp
#define DHTPIN 4 
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
float tempc = 0;
float tempC = 0;

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
int porta_rele4 = 5; // Exaustores



EthernetServer server(80);
static byte mac[] = { 0xFE, 0xAD, 0xBE, 0xEF, 0xFE, 0x3D };  //Mac Address 
static byte ip[] = { 192, 168, 15, 17 };  

EthernetClient client;

int calls = 1;   
int maxCalls = 11;  



void setup(){
Serial.begin(9600);
	dht.begin();
  sensors.begin();
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0))
     Serial.println("Sensores nao encontrados !");
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);
  Serial.println();
  Serial.println();
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
  /// --- Linha temperatura --- ///

    float t = tempc;
   
    u8g.setPrintPos(10, 57);
    u8g.print(t); 
        //u8g.setFont(u8g_font_6x12);
        // char buf[9];
        // sprintf (buf, "%.1f", tempC);
        // u8g.drawStr(10, 57, buf);
        //u8g.drawStr( 20, 57, "23");
    u8g.drawCircle(117,24,3);
        //u8g.drawStr( 92, 57, "C");       
  //moldura
  u8g.drawRFrame(2,18, 126, 46, 4);
}

void draw_temp() 
{

  u8g.setFont(u8g_font_8x13B);
  //Linha superior 
  u8g.drawStr( 20, 15, "ALERTA !!!");
  // Nova Fonte de texto
  u8g.setFont(u8g_font_fub30);

   
   // u8g.setPrintPos(10, 57);
   // u8g.print(t); 
        u8g.setFont(u8g_font_6x12);
        //char buf[20];
        //sprintf (buf, "%s","Temperatura");
        // u8g.drawStr(10, 57, buf);
        u8g.drawStr( 20, 57, "Temperatura");
        u8g.drawStr( 20, 57, "Alta");
    //u8g.drawCircle(117,24,3);
        //u8g.drawStr( 92, 57, "C");       
  //moldura
  u8g.drawRFrame(2,18, 126, 46, 4);
}


////////////////////// - FIM FUNÇÕES DO DISPLAY OLED  - /////////////////////// 


/// --- Sensor Temp 2 --- ////
void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


void loop(){
	
  delay(1000);

  /// --- SENSOR DE TEMPC - DS18B20 --- ///
    // Le a informacao do sensor
    sensors.requestTemperatures();
    float tempC = sensors.getTempC(sensor1);
  // Atualiza temperaturas minima e maxima
    if (tempC < tempMin){
      tempMin = tempC;
    }
    if (tempC > tempMax){
      tempMax = tempC;
    }
    
    
  // Mostra dados no serial monitor
    Serial.print("Temp ds18b20 C: ");
    Serial.print(tempC);
    Serial.print(" Min : ");
    Serial.print(tempMin);
    Serial.print(" Max : ");
    Serial.println(tempMax);
    //tempc=(int)tempC; //converte float para inteiro
    tempc=(float)tempC;
    
  /// --- FINAL SENSOR TEMPC - DS18B20 --- ///
  
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
   // tempc=(int)t;                                       
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
 if(tempc > 32){
	 
	 		digitalWrite(porta_rele8, LOW);  //Liga rele 8 - Exautores
      digitalWrite(porta_rele4, LOW);  //Liga rele 4- Exautores
			delay(100);
      digitalWrite(porta_rele5, LOW);   //Liga rele 5 alarme e giroflex
 
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

		if(tempc <= 32){
			digitalWrite(porta_rele8, HIGH);  //Desliga rele 8 - Exautores
      digitalWrite(porta_rele4, HIGH);  //Desliga rele 4 - Exautores
			delay(100);
     if((d ==1)&&(valor_analogico < nivel_sensor)){
     digitalWrite(porta_rele5, HIGH);   //Liga rele 5 alarme e giroflex
     }
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
























