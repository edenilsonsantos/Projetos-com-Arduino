
int d = 0; 
int led = 13; 
int digital = 2;  

 
void setup()
{
  Serial.begin(9600);
  pinMode(digital, INPUT);
  pinMode(led, OUTPUT);  // Declara o LED como saida
  digitalWrite(led,LOW); // Inicia o LED em estado baixo (desligado)
}
 
void loop()
{

  int d = digitalRead(digital);  // E a quantificação através da leitura analógica.
 
  //Serial.print(" Porta digital: ");
  //Serial.println(d);
 
  if (d != 1) //Se há detecção de fogo for positiva, e acima de um limiar, LED acende.
  {
    digitalWrite(led, HIGH);
    Serial.println("OPs, Fogo Detectado");
    delay(2000);
    
  }
  else{
    digitalWrite (led, LOW);
    Serial.println("Sem Fogo");
    delay(2000);
  }
  delay(500);
}
