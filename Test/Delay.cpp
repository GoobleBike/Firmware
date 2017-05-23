/* $Id: Delay.cpp 122 2013-05-20 17:31:34Z peroni $
* Delay 
* realizza una versione non sospensiva della funzione delay
* Il costruttore di default inizializza un ritardo di 1000 mS 
* il construttore parametrico inizializza un ritardo configurabile
* il metodo execute deve essere schedulato direttamente o indirettamente nel loop
* il metodo execute verifica il passare del tempo mediante il metodo millis() e quando 
* il ritardo scade setta il flag di fine conteggio e riparte immediatamente con un nuovo conteggio
* ÃƒÂ© compito dell'applicazione testare il flag di fine conteggio con il metodo isReached
* la lettura del flag cancella l'evento
* Il valore di ritardo puÃƒÂ² essere cambiato in runtime con il metodo setTimer
*/
#include "./Delay.h"
/**********************************************************************************
/*
/* construttore di default
/* inizializza lo stato interno
/*    _reached falso
/*    _now=_before=campionamento attuale di millis
/*    _delay 1000 mS
/*
/**********************************************************************************/
Delay::Delay() {
  _reached=false;       //reset event
  _now=millis();        //set current time to present
  _before=_now;         //set previous time to presente
  _delay=1000;          //set delay to default 1000 mS
}
/**********************************************************************************
/*
/* construttore parametrico
/* inizializza lo stato interno
/*    _reached falso
/*    _now=_before=campionamento attuale di millis
/*    _delay basato sul parametro time
/*
/**********************************************************************************/
Delay::Delay(unsigned long time) {
  _reached=false;       //reset event
  _now=millis();        //set current time to present
  _before=_now;         //set previous time to presente
  _delay=time;          //set delay to specified value
}
/**********************************************************************************
/*
/* scheduled thread
/* questa azione deve essere schedulata direttamente o indirettamente nel loop del controller
/*   legge il tempo attuale
/*   verifica se il ritardo ÃƒÂ¨ passato
/*   se il tempo ÃƒÂ¨ passato
/*      setta il flag di evento
/*      resetta il timer
/*
***********************************************************************************/
void Delay::execute() {
  //legge il tempo attuale  
  _now=millis();
  //verifica se il tempo è scaduto
  if(_now-_before>_delay){ //ritardo passato: setta il flag di evento e resetta il timer
    _reached=true;              //trigger event
    _before=_now;               //reset timer
  }  
}
/**********************************************************************************
/*
/* isReached: verifica se il tempo ÃƒÂ¨ passato
/*            la lettura resetta il flag
/*
***********************************************************************************/
boolean Delay::isReached(){
   boolean ris=_reached;
   _reached=false;                      //retrigger event
   return ris;                          //return event
}  
/**********************************************************************************
/*
/* setTimer: modifica il valore di ritardo
/* e retriggera
/*
***********************************************************************************/
void Delay::setTimer(unsigned long time) {
  _delay=time;                          //set specified delay
  _reached=false;
  _now=millis();        //set current time to present
  _before=_now;         //set previous time to presente
}  


