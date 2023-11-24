

// ======================================================================================================
// --- Bibliotecas ---
//#include <U8glib.h>

// ======================================================================================================
// --- Objetos ---
//U8GLIB_ST7920_128X64_1X u8g( 6,  //E
                           //  5,  //R/W
                         //    4,  //RS
                         //    7); //RST

// ======================================================================================================
// --- Protótipo das Funções ---
//void draw();                        //função para desenho de string
//void disp_graph_init();             //função de inicialização do display


// ======================================================================================================

#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// --- Mapeamento de Hardware ---
#define   start           9                    //Botão Start
#define   stop1           8                    //Botão Stop
#define   but1           10                    //botão 1 
#define   but2           11                    //botão 2 
#define   but3           12                    //botão 2 
#define   principal       0                    //SSR Principal
#define   triangulo       1                    //SSR Triangulo
#define   estrela         2                    //SSR Estrela


//--- Variaveis ---
char counter = 0x00;
int counter1 = 0;
int estado=0;

// --- Rotina de Interrupção ---
ISR(TIMER2_OVF_vect)
{
    TCNT2=100;          // Reinicializa o registrador do Timer2
    counter++;
    if (counter == 0x0A){

    // aqui coloca a função do timer - 

    // -- Varredura dos Botões --

       if(!digitalRead(start))
        estado = 1;
       if(!digitalRead(stop1))
        estado = 0;

   }

        
      
}//timer2


// --- Configurações Iniciais ---
void setup() 
{

 // disp_graph_init();
   pinMode(8, INPUT);  //Button Stop
   pinMode(9, INPUT);  //Button Start
   pinMode(10, INPUT); //Button 1
   pinMode(11, INPUT); //Button 2
   pinMode(12, INPUT); //Button 3
   pinMode(0, OUTPUT); //SSR Principal
   pinMode(1, OUTPUT); //SSR Triangulo
   pinMode(2, OUTPUT); //SSR Estrela

   digitalWrite(principal, LOW);
   digitalWrite(triangulo, LOW);
   digitalWrite(estrela, LOW);


   // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);


//---Parametros do TIMER2---
    TCCR2A = 0x00;   //Timer operando em modo normal
    TCCR2B = 0x07;   //Prescaler 1:1024
    TCNT2  = 100;    //10 ms overflow again
    TIMSK2 = 0x01;   //Habilita interrupção do Timer2
   

// Configuração do timer1 
//  TCCR1A = 0;                        //confira timer para operação normal pinos OC1A e OC1B desconectados
 // TCCR1B = 0;                        //limpa registrador
//  TCCR1B |= (1<<CS10)|(1 << CS12);   // configura prescaler para 1024: CS12 = 1 e CS10 = 1 
 // TCNT1 = 0xC2F7;                    // incia timer com valor para que estouro ocorra em 1 segundo
//  TIMSK1 |= (1 << TOIE1);           // habilita a interrupção do TIMER1
  
} //end setup


// ======================================================================================================
// --- Loop Infinito ---
void loop() 
{
   lcd.print("hello, world!");
   if (estado==1){
       digitalWrite(principal, HIGH); //Start pressionado? Sim, liga led1
       digitalWrite(estrela, HIGH);
       digitalWrite(triangulo, LOW);
       counter1++;
                }
   if (counter1 == 1000){
           digitalWrite(principal, HIGH);
           digitalWrite(estrela, LOW);
           digitalWrite(triangulo, HIGH);
           estado=2;
           counter1=0;
                       }

   if (estado==0){
        digitalWrite(principal, LOW); //Start pressionado? Sim, liga led1
        digitalWrite(estrela, LOW);
        digitalWrite(triangulo, LOW);
        counter1=0;
                }
              
        delay(100);  
} //end loop


// ======================================================================================================
// --- Desenvolvimento das Funções ---


// ======================================================================================================
// --- Função para desenho de strings ---
