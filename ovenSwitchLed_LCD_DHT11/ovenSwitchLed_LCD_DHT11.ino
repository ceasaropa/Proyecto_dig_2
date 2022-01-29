//librerias
#include <DHT.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <PID_v2.h>

//pines del lcd
LiquidCrystal lcd(8,9,10,11,12,13);  

//pines y configuracion del keypad
const byte FILAS = 4;
const byte COLUMNAS = 4;
char TECLAS[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte PinFilas[FILAS] = {0,1,2,3};
byte PinColumnas[COLUMNAS] = {4,5,6,7};
Keypad keypad = Keypad(makeKeymap(TECLAS), PinFilas, PinColumnas, FILAS, COLUMNAS);

//pin y configuracion del sensor DHT11
int sensor=14;
DHT dht (sensor, DHT11);

//pines y constantes para el control del oven
#define PIN_INPUT 0
#define PIN_OUTPUT 16
double Setpoint, Input, Output;
double Kp = 10, Ki = 1, Kd = 0;
float tempC = 0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

//pin boton
const int pinbutton = 17; 

//inicializacion de variables
int temperaturaIngresada[2] = {0};
int temperatura = 0;
int digito = 0;
int temp = 0;
int cont = 0;
int t1 = 0;
int t2 = 0;
int i = 0;
int button;


void setup() {
  dht.begin();
  lcd.begin(16,2); //inicializa el display de 16x2
  pinMode(pinbutton, INPUT); //configuro el boton como entrada
  
  //codigo de una sola vez para el oven
  tempC = analogRead(PIN_INPUT) * 0.0048828125 * 100; 
  Input = tempC;
  Setpoint = 38;
  myPID.SetMode(AUTOMATIC);
  
  button=digitalRead(pinbutton); //lectura del boton 
  if(button == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("Botón sin");
    lcd.setCursor(0, 1);
    lcd.print("presionar");
    fijar_temp(); //fijacion de temperatura para el primer tipo de funcionamiento
  }
    else{
      lcd.setCursor(0, 0);
      lcd.print("Botón");
      lcd.setCursor(0, 1);
      lcd.print("presionado");
    }

}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  //funcion condicional para decidir el tipo de funcionamiento
  if(button == HIGH){
    //llamado a la funcion que controla el led y la lectura del DHT11
    tem_act_led(); 

  }
    else {
      //llamado a la funcion que controla la temperatura en el horno
      temp_oven(); 
    }

}

//funcion para fijar la temperatura deseada en el teclado
void fijar_temp(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fijar temp: ");
  do{
     lcd.setCursor(0, 1);
     digito = keypad.waitForKey(); //se esperra a que se ingrese un valor
      if (digito == '*'){  //si el valor ingresado no es un numero se imprime vacio
        lcd.print("                  ");
        lcd.setCursor(0,1);
        cont = 0;
      }
      //Cuando se ingresa un digito, se resta 48 (ver código ASCII) y se almacena
        else{    
          digito -= 48;
          temperaturaIngresada[cont] = digito;
          lcd.print(digito);
          
          cont++;
            if( cont == 2 ){
              //se consigue la temperatura ingresada como un solo valor
              temperatura=(temperaturaIngresada[0]*10)+temperaturaIngresada[1]; 
              t1=temperatura-2;  //temperatura minima
              t2=temperatura+2;  //temperatura maxima
            
            }
    
        }   
    i++; 
  }while(i<2);
  
}


//funcion para leer la temperatura desde el dht11 y variar el led
void tem_act_led(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp deseada: ");
  lcd.print(temperatura);
  delay (1000);
  temp=dht.readTemperature();  //se lee la temperatura desde el dht11
  lcd.setCursor(0, 1);
  lcd.print("Temp actual: ");
  lcd.print(temp);
  
  delay(500);

  //Si la temperatura es menor o igual que el min de temp    
  if(temp<=t1){
    digitalWrite(15,HIGH); //la bombilla se enciende
  }
  //Si la temperatura es mayor o igual que el max de temp    
  if(temp>=t2){
    digitalWrite(15,LOW); //la bombilla se apaga
  }
}

//funcion de control del temperatura e impresion con el horno
void temp_oven(){
  //correccion temperatura en oven 
  tempC = analogRead(PIN_INPUT) * 0.0048828125 * 100; 
  Input = tempC;
  myPID.Compute();
  analogWrite(PIN_OUTPUT, Output);

  //impresion de la temperatura actual en el horno
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp actual: ");
  lcd.print(tempC);
  lcd.print("C");
   
  delay(1000);
}

