# Automatizirana instrumentacija - Projekt

## -Arduino kod
---

Kod je pisan u okruženju "PlatformIO", koristi Arduino core.

### Kratki pregled programskog koda

Kod vezan uz komunikaciju s raznim senzorima podijeljen je u zasebne datoteke za svaki senzor. Za svaki senzor funkcije su organizirane u zaseban namespace. Koriste se gotove biblioteke za čitanje podataka s registara senzora. Korištene biblioteke za svaki senzor mogu se vidjeti u header datoteci za pojedini senzor.

Header datoteke senzora:

> ``` dps310.h, opt3001.h, sh_hdc2080.h ```

Implementacije funkcija za senzore:

> ``` dps310.cpp, opt3001.cpp, sh_hdc2080.cpp ```

Kod za komunikaciju s računalom smješten je u:

> ``` communication.h, communication.cpp ```

Implementacija Fifo buffera iz zadatka je u:

> ``` fifo.h, fifo.cpp ```

Pomoćne definicije i funckije nalaze se u: 

> ``` util.h, util.cpp ```

Logika se nalazi u ``` main.cpp ``` datoteci.


---

## Serijska komunikacija

### Mjerenja

Vrijednosti senzora šalju se u sljedećem obliku:
```
[svijetlost|temperatura|tlak|vlaga]
```
Primjer poslanog paketa:
```
[127.06|26.70|1028.17|37.19]
```

### Događaji

Događaji se šalju u sljedećem obliku:
```
{kod_eventa}
```
#### Lista Događaja

| Kod     	  | Opis                |
| ----------- | -----------         |
| 10          | Gasi svijetlo       |
| 11          | Pali svijetlo       |
|             |                     |
| 20          | Tlak je pao         |
| 21          | Tlak je narasao     |
|             |                     |
| 30          | Gasi grijanje       |
| 31          | Pali grijanje       |

#### Lista komadni

| Komanda     	              | Opis                                      |
| --------------------------- | ----------------------------------------- |
| $\n                         | Izlista trenutne vrijednosti varijabli    |
| $1=[cijeli broj]\n          | Postavlja MINIMALNU TEMPERATURU °C        |
| $2=[cijeli broj]\n          | Postavlja MAKSIMALNU TEMPERATURU U °C     |
| $3=[cijeli broj]\n          | Postavlja MINIMALNO OSVJETLJENJE U Lux    |

Primjer: postavljanje minimalne temperature na 22°C
Potrebno je serijskom komunikacijom poslati slijedeće
```
$1=22\n
```


