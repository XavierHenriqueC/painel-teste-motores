//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Painel de testes de motores AC com sensores de corrente e acionamento por SSR. Programado por: Henrique Xavier                                                                        //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <18F452.H>
#device ADC=10
#use delay (clock=16000000)
#fuses HS, NOWDT, PUT, NOBROWNOUT, NOLVP, PROTECT
#use rs232(baud=9600,parity=N,xmit=PIN_C2,rcv=PIN_C7,bits=8)// Pino RC2 é Tx e o Pino Rx é o Pino RC7

#define mode_4_bit       //GLCD Operando em 4 bits
#include <st7920.c>      //Biblioteca do GLCD 128x64
#include <graphics.c>    //Biblioteca grafica
#include <internal_eeprom.c>
//...................................................................................................   
  
 //Variaveis Globais
   int16 tempo_on=0;          //incremento para controle de tempo do estrela triangulo
   int   start_on=0;          //estado do modo estrela triangulo
   int   filtro=0;            // filtro de tecla C0 e C1
   int  Mod_Start=0;
   int16  tempo_on_pulse=0;
   int16  tempo_off_pulse=0;
   int16  tempo_on_pulse_set=3;
   int16  tempo_off_pulse_set=3;
   int   pulse_qnd=0;
   int   pulse_qnd_set=5;
     
   float L1=0;                //Valor da string L1    
   float Offset_L1=0;         //offset sensor 1
   float ref_L1=0;            //Referencia sensor 1
   float L1_eq=0;
   float L1_real=0;
   int rep1=0;
   
   float L2=0;                //Valor da string L2                    
   float Offset_L2=0;         //offset sensor 2
   float ref_L2=0;            //Referencia sensor 2
   float L2_eq=0;
   float L2_real=0;
   int rep2=0;
   
   float L3=0;                 //Valor da string L3                    
   float Offset_L3=0;         //offset sensor 3
   float ref_L3=0;            //Referencia sensor 3 
   float L3_eq=0;
   float L3_real=0;
   int rep3=0;
   
   
   float tensao_L1=0;
   float Ajuste_L1;
   
   float tensao_L2=0;
   float Ajuste_L2;
   
   float tensao_L3=0;
   float Ajuste_L3;
 
   int16 mediaAD_L1=0;         //valor do ADC 1
   int16 mediaAD_L2=0;         //valor do ADC 2
   int16 mediaAD_L3=0;         //valor do ADC 3
   float tensao=0;             //Auxiliar para valores de tensão dos sensores
   int16 valor=0;              //Auxiliar para valores dos Conversores AD
   int16 valor_adc=0;              //Auxiliar para valores dos Conversores AD
   int   rep=0;                //Auxiliar para repetiçoes (FOR)
 
   int menu=0;                 //Telas de menu restrito
   int menu_lib=0;             //Telas de menu liberado
   int filtro_E0=0;            //Filtra a tecla de descida dos menus
   int filtro_c3=0;            //Filtra a tecla de subida dos menus
   int TempoET;                //Tempo do estrela triangulo
   float TempoETreal;          //conversão do tempo de estrela triangulo para sugundos
   
   int16 Pot_real=0;           // Valor da potencia real (Watts)
   int   Pot_HP=0;             // Valor da potencia convertido em (HP)
   int16 tensao_rede=220;      //tensão da rede
   float corrente_media=0;     //valor da media das tres correntes (L1+L2+L3/3)

   int16 leituraEeprom=0;      //Lê os valores salvos na eemprom para poder salvar se for mudado
   int def_eeprom=0;
   
   char string[50];
   
 void leitura_sensores()
 {  
   //------   LEITURA DO SENSOR DE string L1  --------
   
   set_adc_channel(0);
   delay_us(20);
   mediaAD_L1=0; valor=0; tensao=0;
   
   for(rep=1; rep<=8; rep++)
   { 
     valor_adc = read_adc();
     if(valor_adc > 500)
     {
      valor = valor_adc;
     }
     delay_ms(1);
     mediaAD_L1= mediaAD_L1 + valor;
   }
   mediaAD_L1= mediaAD_L1/8;
   tensao=(float)mediaAD_L1*4.8828;
   tensao_L1 = tensao;
   L1_real=(float)(((tensao - (2500 + Ajuste_L1))*100)/1650);
   
   //CALCULA CORRENTE MEDIA EM AMPERES
   rep1++;
   L1_eq=L1_eq+L1_real;
 
   if(rep1>=20)
   {
    L1_eq = L1_eq/20; 
    L1 = L1_eq;
    rep1=0;
    L1_eq=0;
   }
   if (L1<0)
   {
     L1=0;
   }
      
     
   //------LEITURA DO SENSOR DE string L2  -------
     
   set_adc_channel(1);
   delay_us(20);
   mediaAD_L2=0; valor=0; tensao=0;
   
   for(rep=1; rep<=8; rep++)
   { 
     valor_adc = read_adc();
     if(valor_adc > 500)
     {
      valor = valor_adc;
     }
     delay_ms(1);
     mediaAD_L2 = mediaAD_L2 + valor;
   }
   mediaAD_L2= mediaAD_L2/8;
   tensao=(float)mediaAD_L2*4.8828;
   tensao_L2 = tensao;
   L2_real=(float)(((tensao - (2500 + Ajuste_L2))*100)/1650);
   
      //CALCULA CORRENTE MEDIA EM AMPERES
   rep2++;
   L2_eq=L2_eq+L2_real;
 
   if(rep2>=20)
   {
    L2_eq = L2_eq/20; 
    L2 = L2_eq;
    rep2=0;
    L2_eq=0;
   }      
   if (L2<0)
   {
     L2=0;
   }
    
  //------LEITURA DO SENSOR DE string L3  --------
    
   set_adc_channel(2);
   delay_us(20);
   mediaAD_L3=0; valor=0; tensao=0;
   
   for(rep=1; rep<=8; rep++)
   {
     valor_adc = read_adc();
     if(valor_adc > 500)
     {
      valor = valor_adc;
     }
     delay_ms(1);
     mediaAD_L3= mediaAD_L3 + valor;
   }
   mediaAD_L3= mediaAD_L3/8;
   tensao=(float)mediaAD_L3*4.8828;
   tensao_L3 = tensao;
   L3_real=(float)(((tensao - (2500 + Ajuste_L3))*100)/1650);
     
   rep3++;
   L3_eq=L3_eq+L3_real;
 
   if(rep3>=20)
   {
    L3_eq = L3_eq/20; 
    L3 = L3_eq;
    rep3=0;
    L3_eq=0;
   }
   
   if (L3<0)
   {
      L3=0;
   } 
   
 }//leitura sensores
   
 void controle_saidas()
{
 
 if(Mod_Start==0&&menu_lib==0||menu>=1)
 {
  if(!input (pin_D0))
  {
    start_on=0;
  }
  if(!input(pin_D1)&&input(pin_D0)&&filtro==0)
  {
    start_on=1;
    filtro++;
  }
  if(start_on==1)
  {
    output_high(pin_C4);
    output_high(pin_D3);
    output_low(pin_D2);
    tempo_on++;                                    
  }
  if(tempo_on==TempoET&&start_on==1)       
  { 
    output_high(pin_C4);
    output_low(pin_D3);
    output_high(pin_D2);
    start_on=2;
    tempo_on=0;
  } 
  if(start_on==2)
  {
    output_high(pin_C4);
    output_low(pin_D3);
    output_high(pin_D2);
  }
       
  if(start_on==0)
  {
    output_low(pin_C4);
    output_low(pin_D3);
    output_low(pin_D2);
    tempo_on=0;  
    filtro=0;
    Ajuste_L1 = 2500 - tensao_L1;
    Ajuste_L2 = 2500 - tensao_L2;
    Ajuste_L3 = 2500 - tensao_L3;
    tensao_L1 = tensao_L1*(-1);
    tensao_L2 = tensao_L2*(-1);
    tensao_L3 = tensao_L3*(-1);
  }
 } //if controletring
 
 
 
  if(Mod_Start==1&&menu_lib==0)
 {
  if(!input (pin_D0))
  {
    start_on=0;
  }
  if(!input(pin_D1)&&input(pin_D0)&&filtro==0)
  {
    start_on=1;
    filtro++;
  }
    if(pulse_qnd==pulse_qnd_set)
  {
    output_high(pin_C4);
  }
  if(start_on==1&&tempo_off_pulse==0&&pulse_qnd!=pulse_qnd_set)
  {
    output_high(pin_C4);
    tempo_on_pulse++; 
  }
  if(tempo_on_pulse==tempo_on_pulse_set&&start_on==1&&pulse_qnd!=pulse_qnd_set)       
  { 
    output_low(pin_C4);
    tempo_off_pulse++;
  } 
   if(tempo_off_pulse==tempo_off_pulse_set&&start_on==1&&pulse_qnd!=pulse_qnd_set)       
  { 
    tempo_on_pulse=0;
    tempo_off_pulse=0;
    pulse_qnd++;
  } 
       
  if(start_on==0)
  {
    output_low(pin_C4);
    output_low(pin_D3);
    output_low(pin_D2);  
    filtro=0;
    tempo_on_pulse=0;
    tempo_off_pulse=0; 
    pulse_qnd=0;
    Ajuste_L1 = 2500 - tensao_L1;
    Ajuste_L2 = 2500 - tensao_L2;
    Ajuste_L3 = 2500 - tensao_L3;
    tensao_L1 = tensao_L1*(-1);
    tensao_L2 = tensao_L2*(-1);
    tensao_L3 = tensao_L3*(-1);
  }
 } //if controletring
 
} //controle_saidas

 void controle_menu()
 {
  glcd_fillScreen(OFF);//clear glcd
  if(!input(pin_E0)&&menu>0&&menu<6&&filtro_E0==0) // Condição de menu depois de menu=1
  {
   menu++;
   filtro_E0++;
  }
  if(!input(pin_E0)&&menu_lib<5&&filtro_E0==0&&menu==0) // 
  {
   menu_lib++;
   filtro_E0++;
  }
  
  if(!input(pin_E0)&&menu_lib==1&&menu==0) // Condição de menu depois de menu=1
  {
   filtro_E0++;
  }
  
  if(filtro_E0>20) //Condição para entrar no primeiro menu de calibrações
  {
   menu=1;
  }
 
  if(input(pin_E0)) //Filtra tecla de descida 
  {
   filtro_E0=0;
  }
  
  if(!input(pin_C3)&&menu>0&&filtro_c3==0) // Sobe Menu
  {
   menu--;
   filtro_c3++;
   start_on=0;
  }
   if(!input(pin_C3)&&menu_lib>0&&filtro_c3==0) // Sobe Menu
  {
   menu_lib--;
   filtro_c3++;
   start_on=0;
  }
  
  if(input(pin_C3)) // Filtra tecla de subida
  {
   filtro_c3=0;
  }
    
}//controle_menu

 void menus()
{
  if (menu==0&&menu_lib==0)
  {
   glcd_fillScreen(OFF);//clear glcd  
   
   sprintf(string, "%.1f", (float)L1); // Converte adc em texto

   if(L1>=100)
   {
     string[6] = '\0';
     string[5] = 'A';
   }
   if(L1<100)
   {
     string[5] = '\0';
     string[4] = 'A';
   }
   if(L1<10)
   {
     string[4] = '\0';
     string[3] = 'A';
   }
   
   glcd_text57(18, 1, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
     
   sprintf(string, "L1:"); // Converte char em texto
   string[3] = '\0';
   glcd_text57(1, 1, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
   
   sprintf(string, "%.1f", (float)L2); // Converte adc em texto
     
   if(L2>=100)
   {
     string[6] = '\0';
     string[5] = 'A';
   }
   if(L2<100)
   {
     string[5] = '\0';
     string[4] = 'A';
   }
   if(L2<10)
   {
     string[4] = '\0';
     string[3] = 'A';
   }
     
   glcd_text57(18, 10, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
   
   sprintf(string, "L2:"); // converte char em texto
   string[3] = '\0';
   glcd_text57(1, 10, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
   
   sprintf(string, "%.1f", (float)L3); // Converts adc em texto
   if(L3>=100)
   {
     string[6] = '\0';
     string[5] = 'A';
   }
   if(L3<100)
   {
     string[5] = '\0';
     string[4] = 'A';
   }
   if(L3<10)
   {
     string[4] = '\0';
     string[3] = 'A';
   }
     
   glcd_text57(18, 19, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
     
   sprintf(string, "L3:"); // converte char em texto
   string[3] = '\0';
   glcd_text57(1, 19, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
    
  } //if menu=0
   
  if(menu_lib==1&&menu==0)  //Menu de ajuste do tempo de estrela / triangulo
  {
   glcd_fillScreen(OFF);//clear glcd
   
   sprintf(string, "Defina o tempo do    Estrela / Triangulo"); // converte char em texto
   string[40] = '\0';
   glcd_text57(1, 1, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "Tempo:"); // converte char em texto
   string[6] = '\0';
   glcd_text57(1, 25, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "%.1f",TempoETreal); // Converte adc em texto
   string[4] = '\0';
   string[3] = 's';
   

   glcd_text57(50, 25, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   if(!input(pin_E1)&&TempoET<45) //incrementa && valor maximo
   {
    TempoET++;
   }
   if(!input(pin_C2)&&TempoET>5) //decrementa && valor minimo
   {
    TempoET--;
   }
   TempoETreal = TempoET/4.54; //converte em segungos
   
   glcd_update();//update glcd
  
  } //if menu_lib=1
  
  
  if(start_on==0&&menu_lib==2||menu_lib==3||menu_lib==4||menu_lib==5)  //Menu de ajuste do modo de start
  {
   glcd_fillScreen(OFF);//clear glcd
  
   sprintf(string, "Modo de ligamento"); // converte char em texto
   string[40] = '\0';
   glcd_text57(1, 1, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "Tipo:"); // converte char em texto
   string[5] = '\0';
   glcd_text57(1, 15, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   IF(Mod_Start==1)
   {
   sprintf(string, "Pulso"); // converte char em texto
   string[7] = '\0';
   glcd_text57(70, 15, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   }
   IF(Mod_Start==0)
   {
   sprintf(string, "Direto"); // converte char em texto
   string[7] = '\0';
   glcd_text57(70, 15, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   }
   
   sprintf(string, "Pulsos ON:"); // converte char em texto
   glcd_text57(1, 35, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "%ld",tempo_on_pulse_set); // Converte adc em texto
   glcd_text57(70, 35, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "Pulsos OFF:"); // converte char em texto
   glcd_text57(1, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "%ld",tempo_off_pulse_set); // Converte adc em texto
   glcd_text57(70, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "Quantidade:"); // converte char em texto
   glcd_text57(1, 55, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "%d",pulse_qnd_set); // Converte adc em texto
   glcd_text57(70, 55, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_update();//update glcd
   if(menu_lib==2)
   {
   
     glcd_rect(0, 14, 120, 22, yes, on);
     if(!input(pin_E1)&&Mod_Start<1) //incrementa && valor maximo
     {
       Mod_Start++;
     }
     if(!input(pin_C2)&&Mod_Start>0) //decrementa && valor minimo
     {
       Mod_Start--;
     }
   }
   if(menu_lib==3)  //Menu de calibração do sensor L1 (offset)
   {
       // glcd_fillScreen(OFF);//clear glcd
      glcd_rect(0, 34, 120, 42, yes, on);
   
      if(!input(pin_E1)) //incrementa && valor maximo
      {
       tempo_on_pulse_set++;
      }
      if(!input(pin_C2)) //decrementa && valor minimo
      {
       tempo_on_pulse_set--;
      }
   }
   if(menu_lib==4)  //Menu de calibração do sensor L1 (Ref)
  {
     glcd_rect(0, 44, 120, 52, yes, on);
   
     if(!input(pin_E1)) //incrementa && valor maximo
     {
       tempo_off_pulse_set++;
     }
     if(!input(pin_C2)) //decrementa && valor minimo
     {
       tempo_off_pulse_set--;
     }
  }
   if(menu_lib==5)  //Menu de calibração do sensor L1 (Ref)
  {
     //glcd_fillScreen(OFF);//clear glcd
     glcd_rect(0, 54, 120, 62, yes, on);
   
     if(!input(pin_E1)) //incrementa && valor maximo
     {
       pulse_qnd_set++;
     }
     if(!input(pin_C2)) //decrementa && valor minimo
     {
       pulse_qnd_set--;
     }
  }
  
  glcd_update();//update glcd
  }

  if(menu==1||menu==2||menu==3||menu==4||menu==5||menu==6||menu==7)  //Menu de calibração do sensor L1 (offset)
 {
   glcd_fillScreen(OFF);//clear glcd
   
   sprintf(string, "Calibracao           dos Sensores"); // converte char em texto
   string[40] = '\0';
   glcd_text57(1, 1, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "Offset:"); // converte char em texto
   glcd_text57(1, 25, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "Ref:"); // converte char em texto
   glcd_text57(1, 35, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   sprintf(string, "%f",ref_L1); // Converte adc em texto
   glcd_text57(50, 35, string, 1, on);//printf (linha, coluna, string,tamanho, color)

   glcd_update();//update glcd
   
   if(menu==1)
  { 
   sprintf(string, "%lf",Offset_L1); // Converte adc em texto
   glcd_text57(50, 25, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_rect(0, 24, 80, 32, yes, on);
   if(!input(pin_E1)&&Offset_L1<10) //incrementa && valor maximo
   {
     Offset_L1= offset_L1+0.1;
   }
   if(!input(pin_C2)&&Offset_L1>=0) //decrementa && valor minimo
   {
     Offset_L1= offset_L1-0.1;
   }
   glcd_update();//update glcd
  }
  
  if(menu==2)  //Menu de calibração do sensor L1 (Ref)
  {
   glcd_rect(0, 34, 80, 42, yes, on);
     
   if(!input(pin_E1)&&ref_L1<30) //incrementa && valor maximo
   {
     ref_L1=ref_L1+0.1;
   }
   if(!input(pin_C2)&&ref_L1>5) //decrementa && valor minimo
   {
     ref_L1=ref_L1-0.1;
   }
   
   glcd_update();//update glcd
  
  }//if menu=2

  if(menu==1||menu==2) //Strings em comum - calibração do sensor L1 
  {
   sprintf(string, "%.1f", (float)L1); // Converte adc em texto   
   glcd_text57(18, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
  
   sprintf(string, "L1:"); // Converte char em texto
   string[3] = '\0';
   glcd_text57(1, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_update();//update glcd
  
  }//if menu = 2 ou 3
  
  if(menu==3)  //Menu de calibração do sensor L2 (offset)
  {
   sprintf(string, "%lf",Offset_L2); // Converte adc em texto
   glcd_text57(50, 25, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_rect(0, 24, 80, 32, yes, on);
   
   if(!input(pin_E1)&&Offset_L2<10) //incrementa && valor maximo
   {
     Offset_L2 = offset_L2+0.1;
   }
   if(!input(pin_C2)&&Offset_L2>0) //decrementa && valor minimo
   {
     Offset_L2= offset_L2-0.1;
   }
   glcd_update();//update glcd
  
  }//if menu=4
  
  if(menu==4)  //Menu de calibração do sensor L2 (Ref)
  {
   glcd_rect(0, 34, 80, 42, yes, on);
   
   if(!input(pin_E1)&&ref_L2<30) //incrementa && valor maximo
   {
     ref_L2=ref_L2+0.1;
   }
   if(!input(pin_C2)&&ref_L2>5) //decrementa && valor minimo
   {
     ref_L2=ref_L2-0.1;
   }
   
   glcd_update();//update glcd
  
  }//if menu=5

  if(menu==3||menu==4) //Strings em comum - calibração do sensor L2 
  { 
   sprintf(string, "%.1f", (float)L2); // Converte adc em texto
   glcd_text57(18, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
     
   sprintf(string, "L2:"); // Converte char em texto
   string[3] = '\0';
   glcd_text57(1, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_update();//update glcd
  
  }//if menu = 4 ou 5
  
  if(menu==5)  //Menu de calibração do sensor L3 (offset)
  {
   sprintf(string, "%lf",Offset_L3); // Converte adc em texto
   glcd_text57(50, 25, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_rect(0, 24, 80, 32, yes, on);

   if(!input(pin_E1)&&Offset_L3<10) //incrementa && valor maximo
   {
     Offset_L3 = offset_L3+0.1;
   }
   if(!input(pin_C2)&&Offset_L3>0) //decrementa && valor minimo
   {
     Offset_L3 = offset_L3-0.1;
   }
   glcd_update();//update glcd
  
  }//if menu=6
  
  if(menu==6)  //Menu de calibração do sensor L3 (Ref)
  {
   glcd_rect(0, 34, 80, 42, yes, on);
   
   if(!input(pin_E1)&&ref_L3<30) //incrementa && valor maximo
   {
     ref_L3=ref_L3+0.1;
   }
   if(!input(pin_C2)&&ref_L3>5) //decrementa && valor minimo
   {
     ref_L3=ref_L3-0.1;
   }
   
   glcd_update();//update glcd
  
  }//if menu=7

  if(menu==5||menu==6) //Strings em comum - calibração do sensor L3 
  {
   sprintf(string, "%.1f", (float)L3); // Converte adc em texto
   
   glcd_text57(18, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
     
   sprintf(string, "L3:"); // Converte char em texto
   string[3] = '\0';
   glcd_text57(1, 45, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   glcd_update();//update glcd
  
  }//if menu = 6 ou 7
 
 }//if menu==1,2,3,4,5,6,7
 
}//void menus

 void calculos_potencia()
 {
  if (menu==0&&menu_lib==0)
  {
   //CALCULA CORRENTE MEDIA EM AMPERES
   corrente_media= L1+L2+L3;
   corrente_media= corrente_media/3;
  
   //calcula potencia real em Watts
   pot_real=tensao_rede*corrente_media*1.732*0.85;
   
   sprintf(string, "%.1lf", (float)pot_real); // Converte adc em texto

   if(pot_real>10000)
   {
     string[8] = '\0';
     string[7] = 'W';
   }
   
   if(pot_real<10000)
   {
     string[7] = '\0';
     string[6] = 'W';
   }

   if(pot_real<1000)
   {
     string[6] = '\0';
     string[5] = 'W';
   }
   if(pot_real<100)
   {
     string[5] = '\0';
     string[4] = 'W';
   }
   if(pot_real<10)
   {
     string[4] = '\0';
     string[3] = 'W';
   }
   
   glcd_text57(60, 40, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
     
   sprintf(string, "P(Real)="); // Converte char em texto
   string[8] = '\0';
   glcd_text57(1, 40, string, 1, on);//printf (linha, coluna, string,tamanho, color)
   
   
   
   //Calcula potenia em HP
   pot_HP = 3*(pot_real/736);
     
   sprintf(string, "%2.0d", pot_HP); // Converte adc em texto
   if(pot_HP<100)
   {
     string[5] = '\0';
     string[4] = 'P';
     string[3] = 'H';
   }
   if(pot_HP<10)
   {
     string[4] = '\0';
     string[3] = 'P';
     string[2] = 'H';
   }
   
   glcd_text57(53, 50, string, 1, on);//printf (linha, coluna, string,tamanho, color) 
     
   sprintf(string, "P(HP)="); // Converte char em texto
   string[8] = '\0';
   glcd_text57(1, 50, string, 1, on);//printf (linha, coluna, string,tamanho, color)

  } //if menu=0
 } //void calculos_potencia
 
 
//...............................................................................................................
void main(void)
{
   
   setup_ADC_ports (AN0_AN1_AN3);         //Habilita portas Analógicas
   setup_ADC(ADC_CLOCK_INTERNAL);         //Conversor AD em 4MHz (CLOCK INTERNO)
   delay_ms(1);
   glcd_init_graph();                     //glcd modo graphico
   delay_ms(1);
   glcd_fillScreen(off);                  //glcd clear full pixel 0
   delay_ms(1);
   glcd_update();                         //glcd update  
   enable_interrupts(int_rda);            //Habilita interrupções
   enable_interrupts(global);             //Habilita interrupções globais
   
   
   // LÊ VALORES ANTES SALVOS NA MEMORIA EEPROM
   TempoET=              read_eeprom (1);            
   offset_L1=            read_int16_eeprom (10);
   ref_L1=               read_int16_eeprom (20);
   offset_L2=            read_int16_eeprom (30);
   ref_L2=               read_int16_eeprom (40);
   offset_L3=            read_int16_eeprom (50);
   ref_L3=               read_int16_eeprom (60);
   
   def_eeprom=          read_eeprom (70);
   
   //Valores default de calibração
   if(def_eeprom!=6)
   {
    Offset_L1=0;       //offset sensor 1
    ref_L1=0;            //Referencia sensor 1
    Offset_L2=0;       //offset sensor 2
    ref_L2=0;     
    Offset_L3=0;       //offset sensor 2
    ref_L3=0;            //Referencia sensor 2
    TempoET=25;
   }
   
   
   
   
   
//.................................................................................................................   
   while (true) //loop principal
   {  
      //Fluxo Principal
      leitura_sensores();
      calculos_potencia();
      glcd_update();                      
      controle_menu();
      menus();
      controle_saidas();
      
      
      //Salva valores na EEPROM
      leituraEeprom=read_eeprom(1);
      if(leituraEeprom!=TempoET)
      {write_eeprom(1,TempoET);}
      
      leituraEeprom=read_int16_eeprom(10);
      if(leituraEeprom!=offset_L1)
      {write_int16_eeprom(10,offset_L1);}
      
      leituraEeprom=read_int16_eeprom(20);
      if(leituraEeprom!=ref_L1)
      {write_int16_eeprom(20,ref_L1);}
      
      leituraEeprom=read_int16_eeprom(30);
      if(leituraEeprom!=offset_L2)
      {write_int16_eeprom(30,offset_L2);}
      
      leituraEeprom=read_int16_eeprom(40);
      if(leituraEeprom!=ref_L2)
      {write_int16_eeprom(40,ref_L2);}
      
      leituraEeprom=read_int16_eeprom(50);
      if(leituraEeprom!=offset_L3)
      {write_int16_eeprom(50,offset_L3);}
      
      leituraEeprom=read_int16_eeprom(60);
      if(leituraEeprom!=ref_L3)
      {write_int16_eeprom(60,ref_L3);}
      
      def_eeprom=6;
      leituraEeprom=read_eeprom(70);
      if(leituraEeprom!=def_eeprom)
      {write_eeprom( 70,def_eeprom);}
      
   }//loop principal
}//main
