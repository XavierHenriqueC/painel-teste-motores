
/***************************************************************************
*                DRIVE PARA LCD GRAFICO 128X64 CHIP ST7920                 *
*   COMANDOS                                                               *
*   #define mode_8_bit user lcd mode 8 bit
*   #define mode_4_bit user lcd mode 4 bit
*   glcd_readByte()//LER DADOS DO LCD                                      *
*   glcd_check_busy()//VERIFICA SE LCD ESTA OCUPADO                        *                              *
*   glcd_instruction(instruções)//SETA INTRUÇÕES                           *
*   glcd_data(dados)//ENVIA DADOS PARA LCD                                 *
*   glcd_init_basic()//INICIA LCD  COMO MODO BASICO                        *
*   glcd_update()//ATUALIZA LCD                                            *
*   glcd_init_graph()//INICIA LCD MODO GRAFICO                             *
*   glcd_pixel( x,  y, color)//CONTROLE DE PIXEL                           *
*   glcd_fillScreen(on)//ENVIO NIVEL 1 PARA TODOS OS PIXEL DO LCD          *
*   glcd_fillScreen(off)//ENVIA NIVEL 0 PARA TODOS OS PIXEL DO LCD         *
*   glcd_plot_image(imagem,largura,altura,coluna,linha,cor) carrega imagem *
*                                                                          *                 
*   PINAGEM DO LCD                                                         *
*   Pin 1 ------------> Gnd                                                *   
*   Pin 2 ------------> +5volts                                            *
*   Pin 3 ------------> Contrast                                           *
*   Pin 4 ------------> Register Select                                    *
*   Pin 5 ------------> Read/Write                                         *
*   Pin 6 ------------> Enable                                             *
*   Pin 7-14 ---------> Data bits                                          *
*   Pin 15 -----------> PSB (parallel=high & serial=low)                   *
*   Pin 16 -----------> NoConnection                                       *
*   Pin 17 -----------> Reset                                              *
*   Pin 18 -----------> Vout                                               *
*   Pin 19 -----------> +5volts                                            *
*   Pin 20 -----------> Gnd                                                *
*                                                                          * 
***************************************************************************/
//DEFINIÇÃO DE PINOS
//PIN DEFINITION
#define  RS        PIN_B2                    //COMMNAD/DATA SELECT
#define  RW        PIN_B3                    //READ/WRITE SELECT             
#define  E         PIN_B4                    //ENABLE SIGNAL                 
#define  RST       PIN_B5                    //RESET SIGNAL 

#if defined(mode_8_bit)// define pin lcd 8 bit
#define  D0        PIN_D0
#define  D1        PIN_D1
#define  D2        PIN_D2
#define  D3        PIN_D3
#define  D4        PIN_D4
#define  D5        PIN_D5
#define  D6        PIN_D6
#define  D7        PIN_D7
#define mode       0x10
#endif

#if defined(mode_4_bit)// define pin lcd 4 bit
#define  D4        PIN_D4
#define  D5        PIN_D5
#define  D6        PIN_D6
#define  D7        PIN_D7
#define mode       0x00 
#endif

#define  set_lcd_bit  0x20 | (mode) //mode 4 bit
#define  ext_instru   0x24 | (mode) //Select extended instruction set
#define  graphic_on   0x26 | (mode) // Graphic display ON. 
#define  set_lcd_on   0x0C //display on cursor off and char blink off
#define  set_mode_lcd 0x06 //entry mode set
#define  clear_lcd    0x01 //display clear

#define  DADOS_IN  INPUT_D()
#define  BUSY_IN   PIN_D7
#define  ON   1
#define  OFF   0
//LCD GRAPHICO 128X64
#define  YVAL 64 // DEFINE 64 LINHAS
#define  XVAL 8 // DEFINE COLUNAS 8X16=128 COLUNAS

#define GLCD_WIDTH   128 //DEFINE 128 COLUNAS 

typedef union
{
  int16 word;
  int8 nbyte[2];
} Dots;

typedef struct
{
  int1 refresh;
  Dots pix[YVAL][XVAL];   // Max dimensions for display (x,y) = (128,32)
  } GD_RAM;             //  (0,0) corresponds to upper lefthand corner.

GD_RAM gdram;


unsigned int8 glcd_readByte (unsigned int1 address)//LER BYTE DO LCD
{
  unsigned int8 data;   // Stores the data read from the LCD
  if(address==1)
  {
     output_high(rs);
  }
  if(address==0)
  {
     output_low(rs);
  }
  output_high(rw);//GLCD_RW = RW_READ;      // Set for reading
  output_high(e);//GLCD_E = 1;      // Pulse the enable pin
  delay_us(1);
  data=DADOS_IN;      // Get the data from the display's output register
  output_low(e);//GLCD_E = 0;
  return (data);
}

 
void glcd_check_busy()//VERIFICA SE LCD ESTA OCUPADO
{
   int1 busy=1;
   output_low(rs);      // LOW RS and High RW will put the lcd to
   output_high(rw);      // read busy flag and address counter
   while(busy)
   {         // will cycle until busy flag is 0
      output_high(e);
      if(!input(BUSY_IN))
      {
         busy=0;
      }
      output_low(e);
   }
}
 
void glcd_instruction(unsigned char x)//ENVIA INSTRUÇÕES PARA LCD
{
  #if defined(mode_8_bit)// 8 bit mode
   glcd_check_busy();      //must be satisfied before sending instruction
   output_low(rs);      // LOW RS and LOW RW will put the lcd to
   output_low(rw);      // Write instruction mode   
   Delay_us(5);  //Delay
   output_bit(D7,bit_test(x,7)); //send data bit 7 pin D7
   output_bit(D6,bit_test(x,6)); //send data bit 6 pin D6
   output_bit(D5,bit_test(x,5)); //send data bit 5 pin D5
   output_bit(D4,bit_test(x,4)); //send data bit 4 pin D4
   output_bit(D3,bit_test(x,3)); //send data bit 3 pin D3 
   output_bit(D2,bit_test(x,2)); //send data bit 2 pin D2
   output_bit(D1,bit_test(x,1)); //send data bit 1 pin D1
   output_bit(D0,bit_test(x,0)); //send data bit 0 pin D0
   Delay_us(10);
   output_high(e);//anable
   Delay_us(20);
   output_low(e);// disable       
   #endif
 #if defined(mode_4_bit) // 4 bit mode
   glcd_check_busy();      //must be satisfied before sending instruction
   output_low(rs);      // LOW RS and LOW RW will put the lcd to
   output_low(rw);      // Write instruction mode   
   Delay_us(5);//Delay
   output_bit(D7,bit_test(x,7)); 
   output_bit(D6,bit_test(x,6));
   output_bit(D5,bit_test(x,5));
   output_bit(D4,bit_test(x,4));
   Delay_us(5);
   output_high(e); //enable
   Delay_us(5);
   output_low(e); //disable
   Delay_us(5);
   output_bit(D7,bit_test(x,3));//send data low bit 3 pin D7 
   output_bit(D6,bit_test(x,2));//send data low bit 2 pin D6
   output_bit(D5,bit_test(x,1));//send data low bit 1 pin D5
   output_bit(D4,bit_test(x,0));//send data low bit 0 pin D4
   Delay_us(5);
   output_high(e);
   Delay_us(5);
   output_low(e); // disable 
   #endif

}
void glcd_data(unsigned char x)//ENVIA DADOS PARA LCD
{
  #if defined(mode_8_bit) // 8 bit mode
   glcd_check_busy();
   output_high(rs);      // HIGH RS and LOW RW will put the lcd to
   output_low(rw);      // Write data register mode
   Delay_us(5);
   output_bit(D7,bit_test(x,7)); //send data bit 7 pin D7
   output_bit(D6,bit_test(x,6)); //send data bit 6 pin D6
   output_bit(D5,bit_test(x,5)); //send data bit 5 pin D5
   output_bit(D4,bit_test(x,4)); //send data bit 4 pin D4
   output_bit(D3,bit_test(x,3)); //send data bit 3 pin D3 
   output_bit(D2,bit_test(x,2)); //send data bit 2 pin D2
   output_bit(D1,bit_test(x,1)); //send data bit 1 pin D1
   output_bit(D0,bit_test(x,0)); //send data bit 0 pin D0
   Delay_us(10);
   output_high(e);
   Delay_us(20);
   output_low(e); // disable
   #endif
   
   #if defined(mode_4_bit) // 4 bit mode
   glcd_check_busy(); 
   output_high(rs);      // HIGH RS and LOW RW will put the lcd to
   output_low(rw);      // Write data register mode   
   Delay_us(5);
   output_bit(D7,bit_test(x,7));//send data high bit 7 pin D7 
   output_bit(D6,bit_test(x,6));//send data high bit 6 pin D6
   output_bit(D5,bit_test(x,5));//send data high bit 5 pin D5
   output_bit(D4,bit_test(x,4));//send data high bit 4 pin D4
   Delay_us(5);
   output_high(e);
   Delay_us(5);
   output_low(e); // disable 
   Delay_us(5);
   output_bit(D7,bit_test(x,3));//send data low bit 3 pin D7 
   output_bit(D6,bit_test(x,2));//send data low bit 2 pin D6
   output_bit(D5,bit_test(x,1));//send data low bit 1 pin D5
   output_bit(D4,bit_test(x,0));//send data low bit 0 pin D4
   Delay_us(5);
   output_high(e);
   Delay_us(5);
   output_low(e); // disable 
   #endif
}
 
void glcd_fillScreen (unsigned int1 color)//GRAVA 1 OU 0 EM TODOS OS PIXEL
{
  int8 v, h;
  int16 d;
  d = (color == ON ? 0xFFFF : 0x0000);
 // d = (color == ON ? 0x0000 : 0xFFFF );
  for (v=0; v < YVAL; v++)//32
  {
    for (h=0; h < XVAL; h++)//16
    {
      gdram.pix[v][h].word = d;
    }
  }
  gdram.refresh = TRUE;
}


void glcd_update ()//ATUALIZA LCD
{
  int8 v, h;
  if (gdram.refresh)//verifica se foi atualizado 
  {
    for (v=0; v <YVAL; v++) //64 linhas
    {
      if(v<32)
      {
         glcd_instruction( 0x80 | v);   // Set Vertical Address.
         glcd_instruction( 0x80);   // Set Horizontal Address.           
      }
      else
      {
         glcd_instruction( 0x80 | (v - 32));   // Set Vertical Address.
         glcd_instruction( 0x88);   // Set Horizontal Address.           
      }
      for (h=0; h <XVAL; h++) //8x16=128 colunas
      {
        glcd_data( gdram.pix[v][h].nbyte[1]);   // Write High Byte.
        glcd_data( gdram.pix[v][h].nbyte[0]);   // Write Low Byte.
      }
    }
    gdram.refresh = FALSE;
  }
}



void glcd_init_graph()//INICIA EM MODO GRAPHICO
{
   delay_ms(40);
   output_low(rst);  //reset LCD
   delay_us(1);                     
   output_high(rst);  //LCD normal operation
   glcd_instruction(set_lcd_bit);  //set 4 and 8  bit operation and basic instruction set
   delay_us(144);
   glcd_instruction(0x0C);  //display on cursor off and char blink off
   delay_us(100);
   glcd_instruction(clear_lcd);  //display clear
   delay_ms(10);
   glcd_instruction(set_mode_lcd);  //entry mode set
   delay_us(72);                 
   glcd_instruction(ext_instru);  // Select extended instruction set.
   delay_us (72);   
   glcd_instruction(graphic_on);  // Graphic display ON.
   delay_us (72);
   glcd_fillScreen (OFF); // clear all pixel
   glcd_update (); // update display

}

void glcd_init_basic()//INICIA EM MODO BASICO
{
   delay_ms(40);
   output_low(rst);         //reset LCD
   delay_us(1);                     
   output_high(rst);        //LCD normal operation
   glcd_instruction(set_lcd_bit);   //set 4 bit operation and basic instruction set
   delay_us(144);
   glcd_instruction(0x0C);   //display on cursor off and char blink off
   delay_us(100);
   glcd_instruction(clear_lcd);   //display clear
   delay_ms(10);
   glcd_instruction(set_mode_lcd);   //entry mode set
   delay_us(72);                 
}
//              0     0        0
void glcd_pixel(int8 x, int8 y, int1 color)//GRAVA NOVO VALOR DOS PIXEL
{
  int8 v, h, b;
  v = y;
  h = x/16;
  b = 15 - (x%16);
 
  if (color == ON)
  {
   bit_set (gdram.pix[v][h].word, b);
  }
 
  else
  {
   bit_clear (gdram.pix[v][h].word, b);
  }
  gdram.refresh = TRUE;
}
//                       
void glcd_plot_image(ROM char *img,int width,int height,int x,int y,int inverse)//CARREGA IMAGEM
{
   unsigned int i=0, j=0, k=0;
   unsigned int16 count=0;
   
   //glcd_fillScreen(OFF);                        //Clears the screen (opt.)
   for(j=0;j<height;j++)//linhas
      {   
         for(;i<width;)//colunas
         {
           for(k=0;k<8;k++)//bite
            {
               if(inverse)
               {//           0  ,0  ,
                 glcd_pixel(i+x,j+y,~bit_test(img[count],(k)));
               }
               else
               {
                  glcd_pixel(i+x,j+y,bit_test(img[count],(k)));
               }
               i++;
            }
            count++;
         }
      i=0;
      }
} 
///////////////////////////////////////////////////////////////////////////////////
