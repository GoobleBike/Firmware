//The Gooble Bike 2.0!
//Trainer Emulator
//genera sul pin 5 una forma d'onda compatibile con il sensore e inv.proporzionale al poteziometro su A0 
//WARNING: range molto compresso sui valori alti
//diagnostica: forma d'onda sul pin 3 e valore PWM sul pin 9  per led diagnostico
//riceve il segnale pwm sul pin 2 e lo replica sul pin 3 per led diagnostico
//con ponticello da 3 ad A5 valore analogico riletto sul pin A5 attraverso un filtro RC ed inviato in console
//console configurabile: 0 = no console, 1 = monitor velocità/pendenza/pwm in, 2 plot sensore, 3 plot ingresso PWM
//il tipo di console è determinato dagli ingressi 11 e 12
// 11=0,12=0 (0) -> no console, 11=0,12=1 (1) -> monitor, 11=1,12=0 (2) -> plot sensor, 11=1,12=0 (1) -> plot pwm
//collegamento con Yun
//Yun           Emu
//  2  <--\/---  2
//  5  ---/\-->  5
// GND -------- GND

//iDP 20/5/2017

//hw I/O pin invertiti rispetto alla Gooble Bike
#define SENSOR  5
#define BRAKE   2
#define BRAKING 3
#define MOVING  9
#define SW1     11
#define SW2     12

long tmo;
long dsp;
int mov;
int cmd;
int speed;
long period=0;
int enabled=0;
int toggle=1;
int pwm_dig;
int pwm_an;
int console;
int c;

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR,OUTPUT);
  pinMode(MOVING,OUTPUT);
  pinMode(BRAKE,INPUT);
  pinMode(BRAKING,OUTPUT);
  pinMode(SW1,INPUT_PULLUP);
  pinMode(SW2,INPUT_PULLUP);
  digitalWrite(SENSOR,LOW);
  digitalWrite(MOVING,LOW);
  digitalWrite(BRAKING,LOW);
  tmo=millis();
  dsp=millis();
}

void loop() {
  int sw1=digitalRead(SW1);
  int sw2=digitalRead(SW2);
  console=sw1*2+sw2;
  //attuazione console
  if(console==1){ //console velocità/pendenza
    if(millis()-dsp>1000){
      Serial.print("pot: ");
      Serial.print(cmd);
      Serial.print(' ');
      Serial.print("period: ");
      Serial.print(period);
      Serial.print(' ');
      Serial.print("km/h: ");
      Serial.print(speed);
      Serial.print(' ');
      Serial.print("pwm in: ");
      Serial.println(pwm_an);    
      dsp=millis();
    }  
  }
  else if (console==2) { //forma d'onda per plot sensore
      Serial.println(toggle*5);    
  }
  //gestione sensore (uscita)
  else if (console==3) { //forma d'onda per plot pwm
      Serial.println(pwm_dig*5);        
  }
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
  cmd=analogRead(A0);
  if (cmd>2) {
    period=map(cmd,2,1023,500,2);
    enabled=true;
  }
  else {
    period=0;
    enabled=false;  
  }
  mov=map(cmd,0,1023,0,255);
  analogWrite(MOVING,mov);
  speed=map(cmd,0,1023,0,39);
    
  //gestione freno (ingresso)
  pwm_dig=digitalRead(BRAKE);
  digitalWrite(BRAKING,pwm_dig);
  pwm_an=analogRead(A5);
  pwm_an=map(pwm_an,0,1023,0,255);
}
