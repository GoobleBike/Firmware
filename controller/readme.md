//The Gooble Bike 2.0!
//Versione per Bologna Experience
//con interrupt
//Basata su hw Arduino Yun e sul trainer Elite RealAxiom vers. 6 (wired)
//Legge gli impulsi del sensore di velocità e li converte in velocità [km/h] con una base dei tempi di 1s nel range 0-39 km/h.
//Invia ogni secondo la velocità al server attraverso una connessione HTTP client e riceve il valore della pendenza [%]
//Attua il valore di pendenza nel range 0-20% se il rullo è in movimento.
//23/5/2017
