# Programmateur-EEprom
Permet de flasher les mémoires 28C256 de Atmel

Basé sur un module Arduino Méga 2650

Permet de flasher l'eeprom avec un fichier au format hex.

Ce format est standard pour tous les assembleurs Z80,6502 et 8085.
Les miens sont les résultats de l'assembleur TASM80 .
Format ==>   :1012000001020304050607080910111213141516SC

j'utilise Teraterm comme console rs232.
Ajuster le delais entre deux lignes à 300 ms pour laisser le temps de flasher 
car il faut au moins 8 millisecondes pour que ça marche en mode byte par byte

en mode block ( 64 octets max) il faut aussi 8 à 10 ms entre blocks.
Le code méga2650 a été compilé avec <b>Visual Studio Code + PlatformIO. </b>



