


Proiectul a reprezentat implementarea unei platforme de anunturi online.Am implementat in C atat codul pentru client, cat si pentru server.In lipsa unei interfete grafice, trimiterea comenzilor si afisarea informatiilor se va face in terminal, in urma tastarii unor comenzi predefinite. </br>
Au fost implementate functionalitatile de baza ale unei platforme online: </br>
  -creare cont | deconectare | resetare parola </br>
  -cautare anunturi dupa filtru </br> 
  -istoric tranzactii | vizualizare oferte | negociere prin intermediul platformei </br>
Pentru majoritatea functionalitatilor este necesara conectarea cu un cont pe platforma. 
Stocarea datelor necesare(precum parola useri,username etc.) a fost facuta in sqlite3(compilare cu comanda: gcc ./ex.c -o ./ex -lsqlite3 -std=c99). </br>
O descriere mai detaliata se afla in Springer_Lecture_Notes. </br>
