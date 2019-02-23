#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
#include <CayenneMQTTEthernet.h>

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "5c717cd0-2ee5-11e8-9eaa-434cc97e08c2";
char password[] = "a6552916ef91cb1f7039167ee321395e8dca7f77";
char clientID[] = "e6f96b10-34e9-11e8-aa1d-e5677d2125ce";

#define VIRTUAL_CHANNEL1 1 //sensor temp
#define VIRTUAL_CHANNEL2 2 //sensor fogo
#define VIRTUAL_CHANNEL3 3 //sensor fumaça
#define VIRTUAL_CHANNEL4 4 //Exaustores fase2
#define VIRTUAL_CHANNEL5 5 //Alarme e Giroflex
#define VIRTUAL_CHANNEL6 6 //Disparador de CO2
#define VIRTUAL_CHANNEL7 7 //Switch de rede
#define VIRTUAL_CHANNEL8 8 //Exaustores fase1

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
int porta_rele8 = 9; // Exaustores fase1
int porta_rele7 = 8; // Switch de rede
int porta_rele6 = 7; // Disparador de CO2
int porta_rele5 = 6; // Alarme e Giroflex
int porta_rele4 = 5; // Exaustores fase2

void setup(){
  Serial.begin(9600);
  Cayenne.begin(username, password, clientID);
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
  delay(2000);
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
    
      u8g.setFont(u8g_font_8x13B);
  //Linha superior 
      u8g.drawStr( 20, 15, "Temperatura");
      u8g.setFont(u8g_font_fub30);
  /// --- Linha temperatura --- ///
      float t = tempc;
      u8g.setPrintPos(10, 57);
      u8g.print(t); 
      u8g.drawCircle(117,24,3);
  ///moldura
  u8g.drawRFrame(2,18, 126, 46, 4);
}


////////////////////// - FIM FUNÇÕES DO DISPLAY OLED  - /////////////////////// 


/// --- Inicio Sensor Temp 2 --- ////

void mostra_endereco_sensor(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
/// --- Fim Sensor Temp 2 --- ////




void loop(){
	Cayenne.loop();
  delay(1000);

  /// --- SENSOR DE TEMPC - DS18B20 --- ///
    
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
		      delay(500);
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
			      delay(500);
		      }else{
			      digitalWrite (led, LOW);
			      Serial.println("Ambiente Normal");
			      delay(2000);
		      }
	          delay(500);
           
  ///--- FINAL SENSOR DE FOGO ---///



  ///--- INICIO SENSOR DE TEMPERATURA  DHT ---///

        float h = dht.readHumidity();
        float t = dht.readTemperature();
        // testa se retorno é valido, caso contrário algo está errado.
        if (isnan(t) || isnan(h)) 
        {
        tempc=00;                                       
        //Serial.println("Failed to read from DHT");
        } else {
        // tempc=(int)t;                                       
        //Serial.print("Temperatura: ");
        //Serial.print(t);
        }
        
  ///--- FINAL SENSOR DE TEMPERATURA DHT ---///


  
  /// --- INICIO MOSTRAR DISPLAY OLED --- ///

	         u8g.firstPage();  
		       do 
		       {
		       draw();
		       } while( u8g.nextPage() );
		       delay(50);
  
  /// --- FINAL MOSTRAR DISPLAY OLED --- ///



  /// --- Inicio Envia SMS --- ///

        function notify_pushbullet(){
        http.post('https://api.pushbullet.com/v2/pushes',
              'Content-Type: application/jsonrnAccess-Token: TOKEN_DO_PUSHBULLETrn',
              '{"body":"O sensor detectou fogo, verifique.","title":"Fogo detectado!!!","type":"note"}',
              function (code, data){
              print(code);
              }
        }
  /// --- Fim Envia SMS --- ///

  
} // Fim do loop




  /// --- INICIO COMUNICAÇÃO COM NUVEM API CAYENNE --- ///

        CAYENNE_OUT(VIRTUAL_CHANNEL1) //Monitora temp
        {
        Cayenne.virtualWrite(VIRTUAL_CHANNEL1, tempc, TYPE_TEMPERATURE, UNIT_CELSIUS);
        }


        CAYENNE_OUT(VIRTUAL_CHANNEL2) //Monitora fogo
        {
        Cayenne.virtualWrite(VIRTUAL_CHANNEL2, d, TYPE_TEMPERATURE, UNIT_CELSIUS);
        }


        CAYENNE_OUT(VIRTUAL_CHANNEL3) //Monitora fumaça
        {
        int valor_analogico = analogRead(pin_a0);
        Cayenne.virtualWrite(VIRTUAL_CHANNEL3, valor_analogico, "co", "ppm");
        }


        CAYENNE_IN(VIRTUAL_CHANNEL4) // Rele Exaustor Fase 2
        {
              if (getValue.asInt() == 0) {
              digitalWrite(porta_rele4, HIGH);
              }else{
              digitalWrite(porta_rele4, LOW);
              }
        }


        CAYENNE_IN(VIRTUAL_CHANNEL5) // Alarme Giroflex
        {
            if (getValue.asInt() == 0) {
            digitalWrite(porta_rele5, HIGH);
            }else{
            digitalWrite(porta_rele5, LOW);
            }
        }


        CAYENNE_IN(VIRTUAL_CHANNEL6) // Disparador de CO2
        {
            if (getValue.asInt() == 0) {
            digitalWrite(porta_rele6, HIGH);
            delay(200);
            digitalWrite(porta_rele6, LOW);
            }else{
            digitalWrite(porta_rele6, LOW);
            }
        }


        CAYENNE_IN(VIRTUAL_CHANNEL7) // Switch de Rede
        {
            if (getValue.asInt() == 0) {
            digitalWrite(porta_rele7, HIGH);
            }else{
            digitalWrite(porta_rele7, LOW);
            }
        }


        CAYENNE_IN(VIRTUAL_CHANNEL8) // Rele Exaustor Fase 1
        {
            if (getValue.asInt() == 0) {
            digitalWrite(porta_rele8, HIGH);
            }else{
            digitalWrite(porta_rele8, LOW);
            }
        }




