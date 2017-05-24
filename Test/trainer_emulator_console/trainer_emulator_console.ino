//The Gooble Bike 2.0!
//Trainer Emulator Console
//genera sul pin 5 una forma d'onda compatibile con il sensore ottenendo il semiperiodo da console
// semiperiodo in mS in 3 cifre 000-999 terminato da NL
//diagnostica: forma d'onda sul pin 3 e valore PWM sul pin 9  per led diagnostico
//riceve il segnale pwm sul pin 2 e lo replica sul pin 3 per led diagnostico
//con ponticello da 3 ad A5 valore analogico riletto sul pin A5 attraverso un filtro RC ed inviato in console
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
int console=1;
int c;
int state=0;
int p=0;

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR,OUTPUT);
  pinMode(MOVING,OUTPUT);
  pinMode(BRAKE,INPUT);
  pinMode(BRAKING,OUTPUT);
  digitalWrite(SENSOR,LOW);
  digitalWrite(MOVING,LOW);
  digitalWrite(BRAKING,LOW);
  tmo=millis();
  dsp=millis();
  Serial.println("Emulator");
}

void loop() {
  //lettura console
  //attuazione console
  if(Serial.available()) {
    c=Serial.read();
    //Serial.write(c);
    switch (state){
      case 0: //waiting msb
        if((c>='0')&&(c<='9')) {
          p=p+(c & 0x0f)*100;
          state=1;
        }
        else {
          state=0;
          p=0;          
        }
        break;
      case 1: //waiting isb
        if((c>='0')&&(c<='9')) {
          p=p+(c & 0x0f)*10;
          state=2;
        }
        else {
          state=0;
          p=0;          
        }
        break;
      case 2: //waiting lsb
        if((c>='0')&&(c<='9')) {
          p=p+(c & 0x0f);
          state=3;
        }
        else {
          state=0;
          p=0;          
        }
        break;
      case 3: //waiting NL
        if((c=='\n')) {
          //elaborazione
          Serial.println(p);
          state=0;
          if(p>0) {
            period=p;
            enabled=1;
          }
          else {
            period=0;
            enabled=false;
          }
          p=0;
        }
        else {
          state=0;
          p=0;          
        }
        break;
      default:  //error
        state=0;
        p=0;
    }
  }
  //plot
  //Serial.println(toggle*5);    
  /*
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
  */
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
  /*
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
  */
  //analogWrite(MOVING,mov);
  //speed=map(cmd,0,1023,0,39);
    
  //gestione freno (ingresso)
  pwm_dig=digitalRead(BRAKE);
  digitalWrite(BRAKING,pwm_dig);
  pwm_an=analogRead(A5);
  pwm_an=map(pwm_an,0,1023,0,255);
}
