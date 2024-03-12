


Proiectul a reprezentat implementarea unei platforme de anunturi online.Am implementat in C atat codul pentru client, cat si pentru server.
Au fost implementate functionalitatile de baza ale unei platforme online:
  -creare cont | deconectare | resetare parola
  -cautare anunturi dupa filtru
  -istoric tranzactii | vizualizare oferte | negociere prin intermediul platformei
  
Stocarea datelor necesare(precum parola useri,username etc.) a fost facuta in sqlite3(compilare cu comanda: gcc ./ex.c -o ./ex -lsqlite3 -std=c99).
