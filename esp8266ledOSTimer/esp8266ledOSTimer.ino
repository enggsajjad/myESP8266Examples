#include <ESP8266WiFi.h>
#include <user_interface.h>;//Biblioteca necessaria para acessar os Timer`s.
 


#define BLUE_PIN 5
#define RED_PIN 12
#define GREEN_PIN 13
#define BLUE_PIN 16


os_timer_t tmr0;//Cria o Timer. Maximo de 7 Timer's.
 
volatile byte status;//Variavel armazenada na RAM para Status do LED.
long x;//Variavel para mostrar o funcionamento do Codigo.
 
void setup()
{
   os_timer_setfn(&tmr0, led, NULL); //Indica ao Timer qual sera sua Sub rotina.
   os_timer_arm(&tmr0, 1000, true);  //Inidica ao Timer seu Tempo em mS e se sera repetido ou apenas uma vez (loop = true)
                                     //Neste caso, queremos que o processo seja repetido, entao usaremos TRUE.
 
   pinMode(BLUE_PIN, OUTPUT);//Define BLUE_PIN como Saida.
   pinMode(RED_PIN, OUTPUT);//Define RED_PIN como Saida.
   pinMode(GREEN_PIN, OUTPUT);//Define GREEN_PIN como Saida.
 
   Serial.begin(9600);//Inicia a comunicaçao Serial.
}
 
void loop()
{
   leitura();//Sub rotina para processar os LED`s.
 
   //O codigo irá funcionar normalmente sem que haja delays e acendendo os LED`s no intervalo definido.
   //Para demonstraçao, abra o Serial monitor e voce vera que o codigo continua normalmente.
 
   Serial.println(x++);//Print para provar que o codigo nao trava. Abra o Serial monitor e veja
                       //que o codigo continua rodando enquanto os LED`s piscam a cada 1 Segundo.
}
 
void leitura()//Sub rotina para processar os LED`s.
{
   switch (status)
   {
   case(1)://Caso status seja 1, acenda o led 1.
      digitalWrite(BLUE_PIN, 1);
      digitalWrite(RED_PIN, 0);
      digitalWrite(GREEN_PIN, 0);
   break;
 
   case(2)://Caso status seja 2, acenda o led 2.
      digitalWrite(BLUE_PIN, 0);
      digitalWrite(RED_PIN, 1);
      digitalWrite(GREEN_PIN, 0);
   break;
 
   case(3)://Caso status seja 3, acenda o led 3.
      digitalWrite(BLUE_PIN, 0);
      digitalWrite(RED_PIN, 0);
      digitalWrite(GREEN_PIN, 1);
   break;
 
   case(4)://Caso status seja 4, Apague os 3 LED`s.
      digitalWrite(BLUE_PIN, 1);
      digitalWrite(RED_PIN, 1);
      digitalWrite(GREEN_PIN, 1);
   break;
   }
}
 
void led(void*z)//Sub rotina ISR do Timer sera acessada a cada 1 Segundo e mudara o status do LED.
{
   if (status == 4)//Verifica se o maximo foi alcançado para resetar a contagem.
   {
      status = 0;
   }
 
   status++;
}
