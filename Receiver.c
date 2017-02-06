// Configuración de los pins LCD
sbit LCD_RS at PORTC.B0;
sbit LCD_EN at RC1_bit;
sbit LCD_D7 at RB7_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D4 at RB4_bit;

// Pin de direcciones
sbit LCD_RS_Direction at TRISC.B0;
sbit LCD_EN_Direction at TRISC1_bit;
sbit LCD_D7_Direction at TRISB7_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB4_bit;

char txt[14]; // Salida LCD
float timer; // Almacena la distancia
char aux; // Almacena los datos recibidos
unsigned char *ptr;
char i = 0; // Variable de control

void interrupt(){
   *(ptr+i) = UART1_Read();
   i++;
   if(i == 4) {
     FloatToStr(timer, txt);
     Lcd_Out(1,1,txt);
     i = 0;
   }
   PIR1.RCIF = 0;
}

void main() {
   // Configuración de puertos
   TRISC.B7 = 1; // RB4 = INPUT
   
   Lcd_Init(); // Inicializar LCD
   
   // Configuración UART
   UART1_Init(9600);
   delay_ms(300);

   // Configuración de las Interrupciones RCIE (Protocolo UART)
   PIR1.RCIF = 0; // Inicializar el flag de interrupcion RCIE
   PIE1.RCIE = 1; // Habilita la interrupción RCIE

   // Configuración global de las Interrupciones
   INTCON.PEIE = 1; // Habilita las interrupciones de tipo 'periferal'
   INTCON.GIE = 1; // Habilita las interrupciones en genal

   ptr = &timer;

   while(1);
}