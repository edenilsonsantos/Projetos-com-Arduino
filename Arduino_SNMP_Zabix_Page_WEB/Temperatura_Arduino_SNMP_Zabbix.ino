/************************************************************
 *By Edenilson F. Santos                                    *
 *Date        : 10-JAN-2018                                 *
 *                                                          *
 * *********************************************************/
// Monitora a temperatura ambiente da sala do servidor
// Envia valor do sensor de temperatura para o monitoramento Zabbix
// É possivel consultar o arduino via web pelo IP, e ver o valor da temperatura
// Na pagina web é possivel alterar o IP, pelo endereço IP/setup
// Na pagina web é possivel reiniciar, dar um restart no arduino, pelo endereço IP/setup
// Caso a temperatura esteja acima de 24 graus celsius...
// aciona rele, que liga kit 4 exaustores no teto do rack
// aciona alarme sonoro no local
          

////////////// - DECLARAÇÃO DE BIBLIOTECAS E VARIAVEIS - //////////////////////

#include <Streaming.h>         
#include <Ethernet.h>        
#include <SPI.h>
#include <DHT.h>
#include <MemoryFree.h>
#include <Agentuino.h> 
#include <Flash.h>
#include <TextFinder.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
String readString;
#define tempo 10  //tempo do pico do alarme sonoro
int frequencia = 1;

int pino_alarm = 2;
int pino_rele1 = 3;
int pino_rele2 = 4;
int estadorele1 = 1;
int estadorele2 = 1;

int pino_reset = 9;
boolean ligado = true;

#define DHTPIN A1 // pino que estamos conectado o sensor de temp
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

int tempc=0;                                                 //Temperatura em Graus Celsius
int tempf=0;                                                 //Temperatura em Graus Farenheight 
float ajusteTemp=0;

#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC

////////////// - CONFIGURAÇÃO DE MAC ADDRESS E IP - //////////////////////

byte mac[] = { 0xDE, 0xAD, 0xBE, 0x53, 0xFE, 0x3D};
byte ip[] = {192,168,15,135};
byte subnet[] = {255,255,255,0};
byte gateway[] = {192,168,15,1};
EthernetServer server(80);


////////////////  - CODIGO SETUP ALTERA IP PELA WEB - ////////////////////////
char buffer[100];

//prog_char char htmlx0[] PROGMEM = "<html><title>Arduino Ethernet Setup Page</title><body marginwidth=\"0\" marginheight=\"0\" ";
const char htmlx0[] PROGMEM = "<html><title>Arduino Setup</title><body marginwidth=\"0\" marginheight=\"0\" ";
//const char htmlx0[] PROGMEM = "<html><title>Arduino Ethernet Setup Page</title><body marginwidth=\"0\" marginheight=\"0\" ";
const char htmlx1[] PROGMEM = "leftmargin=\"0\" style=\"margin: 0; padding: 0;\"><table bgcolor=\"#999999\" border";
const char htmlx2[] PROGMEM = "=\"0\" width=\"100%\" cellpadding=\"1\" style=\"font-family:Verdana;color:#fff";
const char htmlx3[] PROGMEM = "fff;font-size:12px;\"><tr><td>&nbsp Arduino Ethernet - Pagina para Alterar o Endereco de IP</td></tr></table><br>";
//const char htmlx3[] PROGMEM = "fff;font-size:12px;\"><tr><td>&nbsp Arduino Ethernet Setup Page</td></tr></table><br>"
//PROGMEM const char *string_table0[] = {htmlx0, htmlx1, htmlx2, htmlx3};
PGM_P const string_table0[] PROGMEM = {htmlx0, htmlx1, htmlx2, htmlx3};

const char htmla0[] PROGMEM = "<script>function hex2num (s_hex) {eval(\"var n_num=0X\" + s_hex);return n_num;}";
const char htmla1[] PROGMEM = "</script><table><form><input type=\"hidden\" name=\"SBM\" value=\"1\"><tr><td>MAC:";
const char htmla2[] PROGMEM = "<input id=\"T1\" type=\"text\" size=\"2\" maxlength=\"2\" name=\"DT1\" value=\"";
const char htmla3[] PROGMEM = "\">.<input id=\"T3\" type=\"text\" size=\"2\" maxlength=\"2\" name=\"DT2\" value=\"";
const char htmla4[] PROGMEM = "\">.<input id=\"T5\" type=\"text\" size=\"2\" maxlength=\"2\" name=\"DT3\" value=\"";
const char htmla5[] PROGMEM = "\">.<input id=\"T7\" type=\"text\" size=\"2\" maxlength=\"2\" name=\"DT4\" value=\"";
const char htmla6[] PROGMEM = "\">.<input id=\"T9\" type=\"text\" size=\"2\" maxlength=\"2\" name=\"DT5\" value=\"";
const char htmla7[] PROGMEM = "\">.<input id=\"T11\" type=\"text\" size=\"2\" maxlength=\"2\" name=\"DT6\" value=\"";
PGM_P const string_table1[] PROGMEM = {htmla0, htmla1, htmla2, htmla3, htmla4, htmla5, htmla6, htmla7};

const char htmlb0[] PROGMEM = "\"><input id=\"T2\" type=\"hidden\" name=\"DT1\"><input id=\"T4\" type=\"hidden\" name=\"DT2";
const char htmlb1[] PROGMEM = "\"><input id=\"T6\" type=\"hidden\" name=\"DT3\"><input id=\"T8\" type=\"hidden\" name=\"DT4";
const char htmlb2[] PROGMEM = "\"><input id=\"T10\" type=\"hidden\" name=\"DT5\"><input id=\"T12\" type=\"hidden\" name=\"D";
const char htmlb3[] PROGMEM = "T6\"></td></tr><tr><td>IP: <input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT7\" value=\"";
const char htmlb4[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT8\" value=\"";
const char htmlb5[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT9\" value=\"";
const char htmlb6[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT10\" value=\"";
PGM_P const string_table2[] PROGMEM = {htmlb0, htmlb1, htmlb2, htmlb3, htmlb4, htmlb5, htmlb6};

const char htmlc0[] PROGMEM = "\"></td></tr><tr><td>MASK: <input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT11\" value=\"";
const char htmlc1[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT12\" value=\"";
const char htmlc2[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT13\" value=\"";
const char htmlc3[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT14\" value=\"";
PGM_P const string_table3[] PROGMEM = {htmlc0, htmlc1, htmlc2, htmlc3};

const char htmld0[] PROGMEM = "\"></td></tr><tr><td>GW: <input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT15\" value=\"";
const char htmld1[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT16\" value=\"";
const char htmld2[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT17\" value=\"";
const char htmld3[] PROGMEM = "\">.<input type=\"text\" size=\"3\" maxlength=\"3\" name=\"DT18\" value=\"";
const char htmld4[] PROGMEM = "\"></td></tr><tr><td><br></td></tr><tr><td><input id=\"button1\"type=\"submit\" value=\"SALVAR E REINICIAR O ARDUINO\" ";
const char htmld5[] PROGMEM = "></td></tr></form></table></body></html>";
PGM_P const string_table4[] PROGMEM = {htmld0, htmld1, htmld2, htmld3, htmld4, htmld5};

const char htmle0[] PROGMEM = "Onclick=\"document.getElementById('T2').value ";
const char htmle1[] PROGMEM = "= hex2num(document.getElementById('T1').value);";
const char htmle2[] PROGMEM = "document.getElementById('T4').value = hex2num(document.getElementById('T3').value);";
const char htmle3[] PROGMEM = "document.getElementById('T6').value = hex2num(document.getElementById('T5').value);";
const char htmle4[] PROGMEM = "document.getElementById('T8').value = hex2num(document.getElementById('T7').value);";
const char htmle5[] PROGMEM = "document.getElementById('T10').value = hex2num(document.getElementById('T9').value);";
const char htmle6[] PROGMEM = "document.getElementById('T12').value = hex2num(document.getElementById('T11').value);\"";
PGM_P const string_table5[] PROGMEM = {htmle0, htmle1, htmle2, htmle3, htmle4, htmle5, htmle6};

const byte ID = 0x92; //used to identify if valid data in EEPROM the "know" bit, 

////////////// - DECLARAÇÃO DOS CODIGOS OIDs E MIB para o PROTOCOLO SNMP - //////////////////////

// RFC1213-MIB OIDs
const char sysDescr[] PROGMEM      = "1.3.6.1.2.1.1.1.0";  // System Description
const char sysContact[] PROGMEM    = "1.3.6.1.2.1.1.4.0";  // System Contact
const char sysName[] PROGMEM       = "1.3.6.1.2.1.1.5.0";  // System Name
const char sysLocation[] PROGMEM   = "1.3.6.1.2.1.1.6.0";  // System Location
const char sysServices[] PROGMEM   = "1.3.6.1.2.1.1.7.0";  // System Services

//My Custom OID's
const char temperatureC[]       PROGMEM     = "1.3.6.1.3.2016.5.1.0";     //Temperature in Celsius
const char temperatureF[]       PROGMEM     = "1.3.6.1.3.2016.5.1.1";     //Temperature is Fahrenheit

// RFC1213 local values
static char locDescr[]              = "Sensor Temperatura";                 
static char locContact[50]          = "Sicredi";             
static char locName[20]             = "Casa Branca";                              
static char locLocation[20]         = "Sao Paulo";                          
static int32_t locServices          = 2; 
                                            
uint32_t prevMillis = millis();
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;

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

void pduReceived()
{
  SNMP_PDU pdu;
  api_status = Agentuino.requestPdu(&pdu);
  //
  if ((pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET)
    && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {
    //
    pdu.OID.toString(oid);
    
    if ( strcmp_P(oid, sysDescr ) == 0 ) {
      // handle sysDescr (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locDescr
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } 
    } else if ( strcmp_P(oid, sysName ) == 0 ) {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locName, strlen(locName)); 
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locName
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysContact ) == 0 ) {
      // handle sysContact (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locContact, strlen(locContact)); 
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locContact
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysLocation ) == 0 ) {
      // handle sysLocation (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locLocation, strlen(locLocation)); 
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locLocation
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysServices) == 0 ) {
      // handle sysServices (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locServices
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locServices);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    }
    else if ( strcmp_P(oid, temperatureC ) == 0 ) /////////////////////////////////////////////////////////////Temperature in Centigrade
    {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) 
      {      
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } 
      else 
      {
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tempc);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
    } 
        else if ( strcmp_P(oid, temperatureF ) == 0 ) ///////////////////////////////////////////////////////////Temperature in Farenheight
    {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) 
      {      
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } 
      else 
      {
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tempf);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
    }   
    else {
      // oid does not exist
      // response packet - object not found
      pdu.type = SNMP_PDU_RESPONSE;
      pdu.error = SNMP_ERR_NO_SUCH_NAME;
    }
    Agentuino.responsePdu(&pdu);
  }

  Agentuino.freePdu(&pdu);
 
}

////////////////////////////////////////////////////- SETUP - //////////////////////////////////////////////


void setup()
{
  ShieldSetup (); //Setup the Ethernet shield
  Ethernet.begin(mac, ip, gateway, subnet);                     //Initialize Ethernet Shield
  server.begin();
  api_status = Agentuino.begin();              //Begin Snmp agent on Ethernet shield
  
  Serial.begin(9600);
  pinMode(pino_rele1, OUTPUT);
  pinMode(pino_rele2, OUTPUT);
  pinMode(pino_alarm, OUTPUT);

  digitalWrite(pino_rele1, HIGH);
  digitalWrite(pino_rele2, HIGH);

  // Display Oled Setup
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
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

  //SNMP Setup
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    Agentuino.onPduReceive(pduReceived);
    delay(10);
    return;
  }
  delay(10);

  //Serial.println("DHT11 test!");
  dht.begin();
  
}

////////////////////// - VOID SHIELD SETUP - //////////////////////////

void ShieldSetup()
{

  int idcheck = EEPROM.read(0);
  if (idcheck != ID){

  }

  // COMENTE O TRECHO ABAIXO PARA CARREGAR O CODIGO E ZERAR O IP, SEM CARREGAR DA EEPROM.
  // INICIE AQUI O COMENTARIO COM ( /* )
  
  if (idcheck == ID){

    for (int i = 0; i < 6; i++){
      mac[i] = EEPROM.read(i+1);
    }
    for (int i = 0; i < 4; i++){
      ip[i] = EEPROM.read(i+7);
    }
    for (int i = 0; i < 4; i++){
      subnet[i] = EEPROM.read(i+11);
    }
    for (int i = 0; i < 4; i++){
      gateway[i] = EEPROM.read(i+15);
    }
  }
  Ethernet.begin(mac, ip, gateway, subnet);

// TERMINE AQUI O COMENTARIO COM ( */ )
  
}

///////////////////////////////////////////- LOOP - //////////////////////////////////////////////////////////

void loop()
{ 
  
 Agentuino.listen();                                      //Listen/Handle for incoming SNMP requests
 
  
 //////////// - LOOP CAPTURA DA TEMPERATURA SENSOR DHT - //////////////////////
  
  // A leitura da temperatura e umidade pode levar 250ms!
  // O atraso do sensor pode chegar a 2 segundos.
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    tempc=00;                                       //Update Centigrade temperature variable
    //Serial.println("Failed to read from DHT");
  } 
  else 
  {

    tempc=(int)t;                                       //Update Centigrade temperature variable
    /*Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print(" %t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C"); */
  }

////// - MOSTRAR DISPLAY OLED - ////////////////////////////////////

u8g.firstPage();  
  do 
  {
    draw();
  } while( u8g.nextPage() );
  delay(50);

 
/////// - CONDICAO LIGAR RELE - ACIONA EXAUSTORES - //////////////

if (tempc > 24){
          //ligar rele 1
            digitalWrite(pino_rele1, LOW);
            estadorele1 = 0;
                }
                else
                {
 if (tempc <= 24){
          //desligar o rele 1
            digitalWrite(pino_rele1, HIGH);
            estadorele1 = 1;
        }
   }

///////////////- CONDIÇÃO PARA ACIONAR O ALARME SONORO - /////////////////////
 if (tempc > 24){
          //ligar alarm
          // tone(pino, frequencia, duração)
  for (frequencia = 150; frequencia < 1000; frequencia += 1) 
  {
    tone(2, frequencia, tempo); 
      delay(1);
        }
          for (frequencia = 1000; frequencia > 150; frequencia -= 1) 
           {
            tone(2, frequencia, tempo); 
             delay(1);
              }
               for (frequencia = 150; frequencia < 1000; frequencia += 1) 
                {
                  tone(2, frequencia, tempo); 
                    delay(1);
                      }
                        noTone(2); 
                          delay(2000);

  for (frequencia = 150; frequencia < 1000; frequencia += 1) 
  {
    tone(2, frequencia, tempo); 
      delay(1);
        }
          for (frequencia = 1000; frequencia > 150; frequencia -= 1) 
            {
              tone(2, frequencia, tempo); 
                delay(1);
                 }
                  for (frequencia = 150; frequencia < 1000; frequencia += 1) 
                    {
                      tone(2, frequencia, tempo); 
                        delay(1);
                         }
                          noTone(2); 
                            delay(2000);


  for (frequencia = 150; frequencia < 1000; frequencia += 1) 
    {
      tone(2, frequencia, tempo); 
        delay(1);
          }
            for (frequencia = 1000; frequencia > 150; frequencia -= 1) 
              {
                tone(2, frequencia, tempo); 
                  delay(1);
                    }
                      for (frequencia = 150; frequencia < 1000; frequencia += 1) 
                        {
                          tone(2, frequencia, tempo); 
                            delay(1);
                              }
                                noTone(2); 
                                  delay(2000);
  }else{
  if (tempc <= 24){
      //desligar o alarm
        noTone(2);
                  }
   }
/////////////// - FIM DO ALARME SONORO - /////////////////////


/////////////// - INICIO DO CODIGO DA PAGINA WEB - ////////////////////

EthernetClient client = server.available();
  if (client) {

    TextFinder  finder(client );
    while (client.connected()) {      
      if (client.available()) {

        
        //This part does all the text searching
        if( finder.find("GET /") ) {
          
                 
          if (finder.findUntil("setup", "\n\r")){
        
          if (finder.findUntil("SBM", "\n\r")){
            byte SET = finder.getValue();
            
              while(finder.findUntil("DT", "\n\r")){
                int val = finder.getValue();
                // if val from "DT" is between 1 and 6 the according value must be a MAC value.
                if(val >= 1 && val <= 6) {
                  mac[val - 1] = finder.getValue();
                }
                // if val from "DT" is between 7 and 10 the according value must be a IP value.
                if(val >= 7 && val <= 10) {
                  ip[val - 7] = finder.getValue();
                }
                // if val from "DT" is between 11 and 14 the according value must be a MASK value.
                if(val >= 11 && val <= 14) {
                  subnet[val - 11] = finder.getValue();
                }
                // if val from "DT" is between 15 and 18 the according value must be a GW value.
                if(val >= 15 && val <= 18) {
                  gateway[val - 15] = finder.getValue();
                }
              }
            // Now that we got all the data, we can save it to EEPROM
            for (int i = 0 ; i < 6; i++){
              EEPROM.write(i + 1,mac[i]);
            }
            for (int i = 0 ; i < 4; i++){
              EEPROM.write(i + 7, ip[i]);
            }
            for (int i = 0 ; i < 4; i++){
              EEPROM.write(i + 11, subnet[i]);
            }
            for (int i = 0 ; i < 4; i++){
              EEPROM.write(i + 15, gateway[i]);
            }
            // configure ID para o bit conhecido, então, quando você redefinir o Arduino, usará os valores de EEPROM
            EEPROM.write(0, 0x92); 
            // Se todos os dados foram gravados em EEPROM, devemos reiniciar o arduino
            // por enquanto você terá que usar o botão de reinicialização do hardware

            pino_reset = 8;
            pinMode(pino_reset, OUTPUT);
            digitalWrite(pino_reset, HIGH);
            delay(1000);
            digitalWrite(pino_reset, LOW);
            Serial.println("Reset Iniciado");
            ligado = false;
            delay(1000);
            pino_reset = 9;
            
          }
          // e a partir deste ponto, podemos começar a construir nossa página de configuração
          // e mostre no navegador do cliente.
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          for (int i = 0; i < 4; i++)
            {
            strcpy_P(buffer, (char*)pgm_read_word(&(string_table0[i])));
            client.print( buffer );
            }
          for (int i = 0; i < 3; i++)
            {
            strcpy_P(buffer, (char*)pgm_read_word(&(string_table1[i])));
            client.print( buffer );
            }
          client.print(mac[0],HEX);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table1[3])));
          client.print( buffer );
          client.print(mac[1],HEX);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table1[4])));
          client.print( buffer );
          client.print(mac[2],HEX);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table1[5])));
          client.print( buffer );
          client.print(mac[3],HEX);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table1[6])));
          client.print( buffer );
          client.print(mac[4],HEX);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table1[7])));
          client.print( buffer );
          client.print(mac[5],HEX);
          for (int i = 0; i < 4; i++)
            {
            strcpy_P(buffer, (char*)pgm_read_word(&(string_table2[i])));
            client.print( buffer );
            }
          client.print(ip[0],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table2[4])));
          client.print( buffer );
          client.print(ip[1],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table2[5])));
          client.print( buffer );
          client.print(ip[2],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table2[6])));
          client.print( buffer );
          client.print(ip[3],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table3[0])));
          client.print( buffer );
          client.print(subnet[0],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table3[1])));
          client.print( buffer );
          client.print(subnet[1],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table3[2])));
          client.print( buffer );
          client.print(subnet[2],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table3[3])));
          client.print( buffer );
          client.print(subnet[3],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table4[0])));
          client.print( buffer );
          client.print(gateway[0],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table4[1])));
          client.print( buffer );
          client.print(gateway[1],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table4[2])));
          client.print( buffer );
          client.print(gateway[2],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table4[3])));
          client.print( buffer );
          client.print(gateway[3],DEC);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table4[4])));
          client.print( buffer );
          for (int i = 0; i < 7; i++)
            {
            strcpy_P(buffer, (char*)pgm_read_word(&(string_table5[i])));
            client.print( buffer );
            }
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table4[5])));
          client.print( buffer );
         
          break;
       
      }
         } 
 
        // put your own html until here 

        ////////////////////// - PAGINA WEB INTEFACE DO CLIENTE - ///////////////////////

                readString = "";

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino</title>");
          
/////////// - INICIO DO STYLE CSS - ///////////////////////////////////

client.println("<style type='text/css'>");
client.println("root {"); 
client.println("display: block;");
client.println("}");

client.println("body {");
client.println("margin: 0;");
client.println("padding: 0;");
client.println("}");

client.println("#wrapper {");
client.println("padding-top: 40px;");
client.println("position: relative;");
client.println("width: 530px;");
client.println("height: 600px;");
client.println("margin-right: auto;");
client.println("margin-top: 0;");
client.println("margin-left: auto;");
client.println("font-size: 30px;");
client.println("color: black;");
client.println("text-align: center;");
client.println("font-family: arial;");
client.println("}");

client.println("#div1 {");
client.println("position: absolute;");
client.println("right: 85px;");
client.println("top: 100px;");
client.println("border-radius: 25px;");
client.println("border: 2px solid #c2c3c9;");
client.println("width: 350px;");
client.println("height: 154px; ");
client.println("}");

client.println("#div2 {");
client.println("position: absolute;");
client.println("right: 85px;");
client.println("top: 340px;");
client.println("border-radius: 25px;");
client.println("border: 2px solid #c2c3c9;");
client.println("width: 350px;");
client.println("height: 38px; ");
client.println("}");
client.println("</style>");

/////////// - FIM DO STYLE CSS - ///////////////////////////////////


          client.println("</head>");
          client.println("<body>");

        client.println();
        // put your own html from here on
        client.print("Alterar o IP ou Reiniciar: Acesse ");
        client.print(ip[0],DEC);
        for (int i= 1; i < 4; i++){
          client.print(".");
          client.print(ip[i],DEC);
          }
        client.print("/setup");
        client.println();
          
          client.println("<div id='wrapper'><img src='http://www.sicredi.com.br/assets/site/img/logo.png'/>");
          
          client.println("<div id='div1'>");
          client.println("<font size='1'>");
          client.println("<br></font>");
             client.println("Temperatura Ambiente: "); 
             //client.println("<br>");              
              if (tempc > 24){
                  client.println("<font size='20' color=\"red\">");
                  client.println(tempc);
                  client.println("&#186;"); 
                  client.println("C</font>");
                  }else{
                  client.println("<font size='20' color=\"blue\">");
                  client.println(tempc);
                  client.println("&#186;"); 
                  client.println("C</font>");
                } 
               client.println("<br>");
               client.println(locName);
         client.println("</div>");
                        
              //client.println(locName);

          client.println("<div id='div2'>");
            client.println("Exaustor Status: ");
                if (estadorele1 == 0){
                    client.println("<font color=\"green\">");
                    client.println("ON");
                    client.println("</font>");
                    }else{
                    client.println("<font color=\"red\">");
                    client.println("OFF");
                    client.println("</font>");
                  }     
            client.println("<br>");
          client.println("</div>");
          
          
          client.println("</div>");
          client.println("</body>");
          client.println("</html>");

          delay(2000);
          //client.stop();
          //delay(50);
        
      
    
  
  
        break;  
        }
      }
    delay(1);
    client.stop();
  }
/////////////// - FIM DO CODIGO DA PAGINA WEB - ////////////////////


 
}
/////////////// - FIM DO LOOP - ////////////////////
