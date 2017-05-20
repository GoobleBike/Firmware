//The Gooble Bike 2.0!
//Trainer Emulator
//genera sul pin 2 una forma d'onda compatibile con il sensore e proporzionale al poteziometro su A0 
//diagnostica: forma d'onda sul pin 3 valore PWM sul pin 9 
//riceve il segnale pwm sul pin A5 attraverso un filtro RC passabasso (5kOhm,220uF) e lo mostra sul led 3 
// IN 0--/\/\/\--0--0 A5
//         R     |
//              ---
//              --- C
//               |
//               = GND
//console configurabile: 0 = no console, 1 = monitor velocità/pendenza, 2 plot sensore
//collegamento con Yun
//Yun      Emu
//  2  <--  2
//  5  --> RC IN --> A5
// GND --- GND

//iDP 20/5/2017

//hw Gooble Bike 1.0
#define SENSOR  2
#define BRAKE   5
#define BRAKING 3
#define MOVING  9
#define TCAMP   10

long tmo;
long dsp;
int mov;
int speed;
long period=0;
int enabled=0;
int toggle=1;
int pwm_in;
int brake;
int console=2;

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR,OUTPUT);
  pinMode(MOVING,OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(SENSOR,LOW);
  tmo=millis();
  dsp=millis();
}

void loop() {
  //gestione console
  if(console==1){ //console velocità/pendenza
    if(millis()-dsp>1000){
      Serial.print(speed);
      Serial.print('\t');
      Serial.println(brake);    
      dsp=millis();
    }  
  }
  else if (console==2) { //forma d'onda per plot
      Serial.println(toggle*5);    
  }
  //gestione sensore (uscita)
  //generazione forma d'onda
  //verifica se onda abilitata
  if(enabled==1) {  //onda abilitata
    //verifica se semiperiodo terminato
    if(millis()-tmo>period) { //semiperiodo scaduto
      //verifica se semiperiodo on o off
      if (toggle==1) {  //era on
        //commuta ad off  
        toggle=0;
        digitalWrite(SENSOR,LOW);
      }
      else { //era off
        //commuta a on
        toggle=1;
        digitalWrite(SENSOR,HIGH);
      }
      //a semiperiodo scaduto riarma tmo
      tmo=millis();
    }
  }
  else { //onda disabilitata segnale a 0
    toggle=0;
    digitalWrite(SENSOR,LOW);    
  }
  //cambia la velocità
  //legge il pot (0,1023)
  int cmd=analogRead(A0);
  //mappa per LED analogico (0,255)
  mov=map(cmd,0,1023,0,255);
  analogWrite(MOVING,mov);
  //mappa inversa per semiperiodo min 6 (86 imp/sec) max 500 (1 imp/sec)
  period=map(cmd,0,1023,500,6);
  speed=map(cmd,0,1023,0,39);
  //disabilita onda per ingresso 0 (periodo infinito)
  if (cmd==0) {
    enabled=0;
  }
  else {
    enabled=1;
  }
  //gestione freno (ingresso)
  pwm_in=analogRead(A5);
  brake=map(pwm_in,0,980,0,255);
  analogWrite(3,brake);
}
