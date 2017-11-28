
#include <Keypad.h>
#include <max6675.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


int tiempoAnterior = 0,minutosAnterior = 0;
int thermoDO = 10;
int thermoCS = 11;
int thermoCLK = 12;
int vccPin = 15;
int gndPin = 16;
int setPoint = 0;
int tiempo,minutos,oldTemp;
bool estado,estado2;
long int oldMilis = 0;
bool primerSetPoint = false;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
LiquidCrystal_I2C lcd(0x3F, 16, 2);
double temperatura = 0;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

int i =0 ;
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
char customKey = "";
const int rele = 13;

void getTemperatura(){
  if(millis()-oldMilis>1000){
    temperatura = thermocouple.readCelsius();
    oldMilis = millis();
    oldTemp = temperatura;
    //temperatura = random(temperatura-20,temperatura+50);
  }
}
void setup(){
  Serial.begin(9600);
  pinMode(rele,OUTPUT);
  digitalWrite(rele,LOW);
  lcd.begin();
  primerSetPoint = false;
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("Autoclave v1.0");
  lcd.setCursor(0, 0);
  lcd.print("UFPS");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oprima A !");
  lcd.setCursor(0, 1);
  lcd.print("Para temperatura");

}
  
void loop(){
  Serial.print("OLD TEMP = ");
  Serial.println(oldTemp);
  Serial.print("ACTUAL TEMP = ");
  Serial.println(temperatura);
  Serial.println(oldTemp +0.5 <= temperatura );
  Serial.println("-----------------------------------------------------------");
  
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  customKey = "";
  customKey = customKeypad.getKey();
  
  //if (customKey){
    //Serial.println(customKey);
  //&& primerSetPoint 
  if(tiempo>0  && setPoint>10 && primerSetPoint)
  {
    Serial.println(millis()-tiempoAnterior);
    delay(500);
    if(millis()-tiempoAnterior>1000){
      tiempoAnterior = millis();
       if(minutos<60){
        minutos = minutos +1;
        Serial.print("tiempo en segundos trascurrido = ");
        Serial.println(minutos);
       }else{
        tiempo = tiempo - 1;
        minutos = 0;
        lcd.clear();
        Serial.print("Tiempo =");
        if(tiempo<=0){
          estado2 = false;
          digitalWrite(rele,LOW);
          lcd.setCursor(0,0);
          lcd.print("Oprima  D ");
          delay(30000);
          setup();    
        }

        Serial.println(tiempo);
        if(tiempo<=0){ setPoint = 0; tiempo = 0; estado = false;} 
       }


       
    }
    
  }
   if(temperatura<setPoint-7 && setPoint>10 && estado2 ){
      digitalWrite(rele,HIGH);    
      Serial.println("ENCENDIDO!! ");
      Serial.print("temperatura: ");
      if(oldTemp +0.3 <= temperatura || oldTemp -0.5 >= temperatura){
          lcd.setCursor(6,1);
          lcd.print("                               ");
          lcd.setCursor(0,1);
          Serial.println("T = ");
          lcd.setCursor(0, 1);
          lcd.print("T = ");
          lcd.setCursor(7, 1);
          lcd.print("                ");
          lcd.setCursor(4, 1);
          lcd.print(temperatura);
      }
      oldTemp = temperatura ;
      lcd.setCursor(9, 1);
      lcd.print(" E=");
      lcd.setCursor(13,1);
      lcd.print("ON");
      lcd.setCursor(0,0);


   }else if (temperatura >= setPoint && setPoint>10){
      digitalWrite(rele,LOW);
      Serial.print("temperatura: ");
      if(millis()-oldMilis>1000 && oldTemp != temperatura){
        lcd.setCursor(0,1);
        lcd.print("                               ");
        lcd.setCursor(0,1);
        
        Serial.println(temperatura);
        lcd.setCursor(0, 1);
        lcd.print("T = ");
        lcd.setCursor(4, 1);
        lcd.print("                ");
        lcd.setCursor(4, 1);
        lcd.print(temperatura);
        lcd.setCursor(9, 1);
        lcd.print(" E=");
        lcd.setCursor(13,1);
        lcd.print("OFF");
        lcd.setCursor(0,0);
        primerSetPoint = true;
      }
  }
      if(tiempo>0){
        if(tiempo!=tiempoAnterior){
          lcd.setCursor(0,0);  
          lcd.print("                                          "); 
          lcd.setCursor(0,0);  
          lcd.print("Tiempo=");
          if(tiempo == 1){
            lcd.print(0);
          }else{  lcd.print(tiempo); }      
          lcd.setCursor(7,0);
        }
        if(minutos!=minutosAnterior){
          lcd.setCursor(10,0);
          lcd.print(":");
          lcd.setCursor(11,0);
          lcd.print(60-minutos);  
        }
        minutosAnterior = minutos;
        tiempoAnterior = tiempo ;
  }else if(setPoint>0){
    lcd.setCursor(0,0);
    lcd.print("Tiempo= Press B");
  }
  if (customKey=='B'){
   Serial.println("por favor ingrese el tiempo en minutos");
   lcd.clear();
   lcd.print("ingrese Tiempo");
   lcd.setCursor(0, 1);
   lcd.print("T= ");
   char tiempoTemporal [4]= {};
   i=0;
   do{
   
      customKey = customKeypad.getKey();
      //Serial.print("customkey: ");
      //Serial.println(customKey);
      //Serial.println(((String)customKey).length());
      //Serial.println((int)customKey);
      if(((String)customKey).length() > 0 && ((int)customKey)<58) {
           i++;
          tiempoTemporal[i] =((int)customKey);
          lcd.setCursor(4+i, 1);
          lcd.print(customKey);
          Serial.print("NUMERO QUE HE LEIDO EN STRING");
          Serial.print(customKey);
          Serial.print("grabe un nuevo numero: ");
          Serial.println(customKey);
      }
         
     
      }    while(customKey != '*');
    Serial.print("El tiempo seleccionado es ");
    Serial.println(i);// 3 
    for(int j = 1;j<=i-1;j++){
        Serial.print("COMPONENTE ");
        Serial.print(j);
        Serial.print("   ");
        Serial.println((char)tiempoTemporal[j]);
    }

    i = i-1;
    if(i==1){
      tiempo = tiempoTemporal[1]-48;
      Serial.println("i=1");
    }else if(i==2){
      tiempo = tiempoTemporal[2]-48;
      tiempo = tiempo + ((int)tiempoTemporal[1]-48)*10;
      Serial.println("i=2");
    }else if(i==3){
      tiempo = tiempoTemporal[3]-48;
      tiempo = tiempo + ((int)tiempoTemporal[2]-48)*10;
      tiempo = tiempo + ((int)tiempoTemporal[1]-48)*100;
      Serial.println("i=3");
      estado2 = true;
    }
    estado = false;
    Serial.println(tiempo);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Point Time");
    lcd.setCursor(0,1);
    lcd.print(tiempo);
    delay(2000);
    lcd.clear();
   }
 
  
  
  if (customKey=='C'){
     lcd.clear();
     lcd.print("La temperatura ingresada ");
     lcd.setCursor(0, 1);
     lcd.print(temperatura);
  
    
  }
  
  if (customKey=='D'){
   lcd.clear();
   lcd.print("REINICIANDO");
   setup();
  }
   
  if (customKey=='A'){
   lcd.clear();
   lcd.print("ingrese temperatura");
   lcd.setCursor(0, 1);
   lcd.print("T= ");
   Serial.println("por favor ingrese la temperatura ");
   char temperatura [4]= {};
   i=0;
   do{
   
      customKey = customKeypad.getKey();
      //Serial.print("customkey: ");
      //Serial.println(customKey);
      //Serial.println(((String)customKey).length());
      //Serial.println((int)customKey);
      if(((String)customKey).length() > 0 && ((int)customKey)<58) {
           i++;
          temperatura[i] =((int)customKey);
          lcd.setCursor(4+i, 1);
          lcd.print(customKey);
          Serial.print("NUMERO QUE HE LEIDO EN STRING");
          Serial.print(customKey);
          Serial.print("grabe un nuevo numero: ");
          Serial.println(customKey);
      }
         
     
      }    while(customKey != '*');
    Serial.print("la temperatura seleccionada es ");
    Serial.println(i);// 3 
    for(int j = 1;j<=i-1;j++){
        Serial.print("COMPONENTE ");
        Serial.print(j);
        Serial.print("   ");
        Serial.println((char)temperatura[j]);
    }

    i = i-1;
    if(i==1){
      setPoint = temperatura[1]-48;
      Serial.println("i=1");
    }else if(i==2){
      setPoint = temperatura[2]-48;
      setPoint = setPoint + ((int)temperatura[1]-48)*10;
      Serial.println("i=2");
    }else if(i==3){
      setPoint = temperatura[3]-48;
      setPoint = setPoint + ((int)temperatura[2]-48)*10;
      setPoint = setPoint + ((int)temperatura[1]-48)*100;
      Serial.println("i=3");
    }
    Serial.println(setPoint);
   
   estado = false;
   lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Point");
    lcd.setCursor(0,1);
    lcd.print(setPoint);
    delay(2000);
    estado2 = true;
   }
 
    //Serial.println(setPoint);

  
   if(setPoint>10){
      getTemperatura();
      
   }
} 



    


