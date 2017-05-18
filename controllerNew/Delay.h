/* $Id: Delay.h 122 2013-05-20 17:31:34Z peroni $
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
#include <Arduino.h>
#ifndef Delay_h
#define Delay_h
class Delay {
  private:
    //tempo di ritardo configurato in mS
    unsigned long _delay;
    //valore corrente di millis() value in mS
    unsigned long _now;
    //valore precedente di  millis() value in mS
    unsigned long _before;
    //flag di evento di fine conteggio
    boolean _reached;
  public:
    //costruttore di default  (1000 mS)
    Delay();
    //costruttore parametrico con un ritardo programmabile
    Delay(unsigned long time);
    //thread schedulato
    void execute();
    //restituisce il flag dell'evento e se settato lo cancella
    boolean isReached();
    //cambia il ritardo programmato
    void setTimer(unsigned long time);
};
#endif


