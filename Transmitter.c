unsigned short x; // Variable Auxiliar para leer PORTB  -- 16 bits
float timer; // Almacena el número de flancos contados  -- 32bits
unsigned char *aux; // Puntero para enviar el resultado mediante UART -- 8bits

void interrupt() {
   // Echo. Empieza o acaba el sensor a medir
   if (INTCON.RBIF)
    {
      if(PORTB.B4) { // ECHO en flanco de subida -> TIMER0 empieza a contar
        TMR0H = 0; TMR0L = 0; // Inicializar registros
        T0CON = 0x80; // Configuración para empezar a contar: T0CON.TMR0ON = 1
      } 
      else { // ECHO en flanco de bajada -> TIMER0 para de contar: Calcular distancia
         T0CON = 0; // Parar el timer0
         timer = TMR0L;
         timer = timer + (TMR0H << 8);
         
         if(timer < 6.0e4) { // Si t3 < 60ms => medicion valida
            timer = timer * 1.657e-2; // Conversion del timer a distancia (cm)
         } 
         else { // Si t3 > 60ms => medicion invalida
            timer = 0; // Cifra error de la medición
         }
         // Transmitir datos
         aux = &timer;
         UART1_Write(*aux);
         delay_ms(100);
         UART1_Write(*(aux+1));
         delay_ms(100);
         UART1_Write(*(aux+2));
         delay_ms(100);
         UART1_Write(*(aux+3));
      }
      x = PORTB; // Hay que leer el puerto B para poder borrar el bit RBIF
      INTCON.RBIF = 0;
   }
}

void main() {
   // Configuración de puertos
   TRISA.B0 = 0; // RA0 = OUTPUT (AN0)
   PORTA.B0 = 0; // Inicializar RA0
   TRISB.B4 = 1; // RB4 = INPUT
   TRISC.B6 = 0; // RC6 = OUTPUT


   ADCON1 = 0x07; // Señales digitales (PIC18F452)

   // Configuración de las interrupciones del TIMER0 y su registro
   T0CON = 0x00; // Configuración del registro del TIMER0 (prescaler = 2)
   INTCON.TMR0IF = 0; // bit 2: Incializar flag de interrupción
   INTCON.TMR0IE = 1; // bit 5: Interrupción TIMER0 habilitada

   // Configuración de la interrupción RB4
   INTCON2.RBPU = 0;
   x = PORTB; // Asignación a la variable global del valor de PORTB
   INTCON.RBIF = 0; // bit 1: Inicializar flag de interrupciones
   INTCON.RBIE = 1; // bit 4: Interrupciones RB habilitada por cambio de nivel

   // Configuración UART
   UART1_Init(9600);
   delay_ms(300);

   // Configuración global de las Interrupciones
   INTCON.GIE = 1; // INTCON - bit 7: Habilitar interrupciones

   while(1){
     // Enviar señal al US-100 para que mida la distancia
     // Trigger cada 1.5 segundos
     PORTA.B0 = 1;
     delay_us(10);
     PORTA.B0 = 0;
     delay_ms(1500);
  }
}