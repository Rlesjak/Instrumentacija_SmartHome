# Automatizirana instrumentacija - Projekt

## -Arduino kod

---

## Serijska komunikacija

### Mjerenja

Vrijednosti senzora šalju se u sljedećem obliku:
```
[svijetlost|temperatura1|tlak|temperatura2|vlaga]
```
Primjer poslanog paketa:
```
[127.06|26.70|1028.17|26.72|37.19]
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