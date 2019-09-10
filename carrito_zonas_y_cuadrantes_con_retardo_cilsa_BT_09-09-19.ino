#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 7); // RX, TX
  #define IN1 2 
  #define IN2 3
  #define IN3 4
  #define IN4 5
  #define VEL_I 9
  #define VEL_D 10
  
  int cero_x,cero_y,umbral=50,pwmd,pwmi;

  int pwmd_anterior,pwmi_anterior,ret_vel=12;

  String inputString = "";         // a string to hold incoming data
  boolean stringComplete = false;  // whether the string is complete

  
  void determinar_cero(void)
  {
     cero_x=analogRead(A0);
     cero_y =analogRead(A1);
  }

  void serialEvent1() {
  while (mySerial.available()) {
    // get the new byte:
    char inChar = (char)mySerial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void retardo_de_velocidad(void)
{

   if (pwmd > (pwmd_anterior + ret_vel))
   {
      pwmd=pwmd_anterior + ret_vel;
      pwmd_anterior=pwmd;
   }
   if (pwmd < (pwmd_anterior - ret_vel))
   {
      pwmd=pwmd_anterior -ret_vel;
      pwmd_anterior=pwmd;
   }
   
   if (pwmi > (pwmi_anterior + ret_vel))
   {
      pwmi=pwmi_anterior + ret_vel;
      pwmi_anterior=pwmi;
   }
   
   if (pwmi < (pwmi_anterior - ret_vel))
   {
      pwmi=pwmi_anterior - ret_vel;
      pwmi_anterior=pwmi;
   }
}
void setup() {
  // put your setup code here, to run once:
  TCCR1B = TCCR1B & 0b11111000 | 1; // Modifica la frecuencia default de PWM
  Serial.begin(9600);
  pinMode(IN1,OUTPUT);    //configura los pines de Arduino como salidas
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(VEL_I, OUTPUT);
  pinMode(VEL_D, OUTPUT); 
  determinar_cero();  //llama a la función que determina la posición inicial del Joystick
  Serial.print("cero x= ");
  Serial.print(cero_x);
  Serial.print(";cero y= ");
  Serial.println(cero_y);
}

void loop() {
  // put your main code here, to run repeatedly:
  int x,y;
  String ejex,ejey;


  serialEvent1(); 
  if (stringComplete)   // Si recibio una cadena completa...
  {
    stringComplete=0;

    ejex=inputString.substring(0,3);  // Tomo los primeros 3 caracteres (eje x)
    ejey=inputString.substring(4,7);  // Tomo los caracteres del eje y
    /*
    Serial.print(ejex);
    Serial.print("\t");
    Serial.println(ejey);
    */
    x=ejex.toInt();  // Los convierto a entero
    y=ejey.toInt();
    
    inputString = "";
  }
  else
  {
    x = analogRead(A0);  // lectura de la coordenana x actual del joystick
    y = analogRead(A1);  // lectura de la coordenana y actual del joystick
  }
  
  //detenido
  if(abs(y-cero_y)<=umbral && abs(x-cero_x)<=umbral)
  {
    digitalWrite(IN1, LOW);  // detener motor IZQ
    digitalWrite(IN2, LOW);  // detener motor IZQ
    digitalWrite(IN3, LOW);  // detener motor DER
    digitalWrite(IN4, LOW);  // detener motor DER
    analogWrite(VEL_I,0);   // Velocidad motor IZQ=0
    analogWrite(VEL_D,0);   // Velocidad motor DER=0
    
   }
   //Retroceder
   if ((y>=cero_y +umbral)&&(abs(x-cero_x)<=umbral)) 
    {    
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    pwmi=map(y,cero_y,1023,umbral,255);
    pwmd=map(y,cero_y,1023,umbral,255);
    retardo_de_velocidad();
    analogWrite(VEL_I,pwmi);
    analogWrite(VEL_D,pwmd);
    Serial.println("D");
    }
    //Avanzar
    if((y<=cero_y-umbral)&&(abs(x-cero_x)<=umbral))
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      pwmi=map(y,cero_y,0,umbral,255);
      pwmd=map(y,cero_y,0,umbral,255);
      retardo_de_velocidad();
      analogWrite(VEL_I,pwmi);
      analogWrite(VEL_D,pwmd);
      Serial.println("B");
    }
    //Girar
    if((x>=cero_x+umbral)&&(abs(y-cero_y)<=umbral))
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      pwmi=map(x,cero_x,1023,umbral,255);
      pwmd=map(x,cero_x,1023,umbral,255);
      retardo_de_velocidad();
      analogWrite(VEL_I,pwmi);
      analogWrite(VEL_D,pwmd);
      Serial.println("C");
    }
    //Girar
    if((x<=cero_x-umbral)&&(abs(y-cero_y)<=umbral))
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      pwmi=map(x,cero_x,0,umbral,255);
      pwmd=map(x,cero_x,0,umbral,255);
      retardo_de_velocidad();
      analogWrite(VEL_I,pwmi);
      analogWrite(VEL_D,pwmd);
      Serial.println("A");
    }
    //cuadrantes
    if (abs(x-cero_x)>umbral && abs(y-cero_y)>umbral)
    {
      determinar_cuadrante(x-cero_x,y-cero_y);
      /*
      digitalWrite(IN1, LOW);  // detener motor IZQ
      digitalWrite(IN2, LOW);  // detener motor IZQ
      digitalWrite(IN3, LOW);  // detener motor DER
      digitalWrite(IN4, LOW);  // detener motor DER
      analogWrite(VEL_I,0);   // Velocidad motor IZQ=0
      analogWrite(VEL_D,0);   // Velocidad motor DER=0
      */
    }
    Serial.print("A0=");
    Serial.print(x);
    Serial.print('\t');
    Serial.print("A1=");
    Serial.println(y);
    Serial.print("pwmd: ");
    Serial.print(pwmd);
    Serial.print('\t');
    Serial.print("pwmi: ");
    Serial.println(pwmi);
    //delay(1500);
    pwmd=0;
    pwmi=0;
}

void determinar_cuadrante(int relativo_x,int relativo_y)
{
  //char cuad='0';
  float modulo;
  float x,y,tangente,angulo;
  
  if (abs(relativo_x)>umbral && abs(relativo_y)>umbral)
  {
    
    x=float(relativo_x);
    y=float(relativo_y);
    modulo=float(sqrt(pow(x,2)+pow(y,2)));
    if (modulo>500)
    {
      modulo=500;
    }
    tangente=abs(y/x);
    if (tangente>0 && tangente<0.363){angulo=0.2;}
    if (tangente>0.363 && tangente<0.577){angulo=0.3;}
    if (tangente>0.577 && tangente<0.84){angulo=0.4;}
    if (tangente>0.84 && tangente<1.2){angulo=0.5;}
    if (tangente>1.2 && tangente<1.73){angulo=0.6;}
    if (tangente>1.73 && tangente<5.67){angulo=0.7;}
    if (relativo_x>0 && relativo_y>0) // cuadrante 3
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        pwmd=map(modulo,0,500,umbral,255);
        pwmi=map(modulo*angulo,0,500,umbral,255);
        retardo_de_velocidad();
        analogWrite(VEL_D,pwmd );  
        analogWrite(VEL_I,pwmi );
        //cuad='1';
        Serial.println("3");
    }
    if (relativo_x<0 && relativo_y>0) // cuadrante 4
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        pwmd=map(modulo*angulo,0,500,umbral,255);
        pwmi=map(modulo,0,500,umbral,255);
        retardo_de_velocidad();
        analogWrite(VEL_D,pwmd );  
        analogWrite(VEL_I,pwmi );
        Serial.println("4");
        //cuad='2';
    }
    if (relativo_x<0 && relativo_y<0) // cuadrante 1
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        pwmd=map(modulo*angulo,0,500,umbral,255);
        pwmi=map(modulo,0,500,umbral,255);
        retardo_de_velocidad();
        analogWrite(VEL_D,pwmd );  
        analogWrite(VEL_I,pwmi );
        Serial.println("1");
        
        //cuad='3';
    }
    if (relativo_x>0 && relativo_y<0) // cuadrante 2
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        pwmd=map(modulo,0,500,umbral,255);
        pwmi=map(modulo*angulo,0,500,umbral,255);
        retardo_de_velocidad();
        analogWrite(VEL_D,pwmd );  
        analogWrite(VEL_I,pwmi );
        Serial.println("2");
        
      
    }
  }
  
}
