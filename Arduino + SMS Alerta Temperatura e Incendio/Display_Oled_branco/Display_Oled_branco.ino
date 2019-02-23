//biblioteca
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC

int tempc = 25;


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



void setup()
{
   
  Serial.begin(9600);
  
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

  delay(10);
  
}



void loop()
{ 
  
////// - MOSTRAR DISPLAY OLED - ////////////////////////////////////

u8g.firstPage();  
  do 
  {
    draw();
  } while( u8g.nextPage() );
  delay(50);
  
  }
