#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#define Lgraspuns 500
#define Lgnume 30
#define Lgadresa 30
#define Lgparola 30
#define Lgcategorie 30
#define Lgbrand 30
#define Lgmodel 30
#define Lgpret 30
#define Nranunt 10
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;
int deconectat=0;
void prelucrareMesaj(char *msg);
void creare_cont(int sd);
void deconectare(int sd);
void logare(int sd);
void resetParola(int sd);
void creareAnunt(int sd);
void vizualizareProduse(int sd);
void cumparaProdus(int sd);
void istoricTranzactii(int sd);
void citireAndScriere(char sir[],int size,int sd);
void stergeAnunt(int sd);
void propuneOferta(int sd);
void anunturileMele(int sd);
void acceptOferta(int sd);
void oferteleMele(int sd);
void news(int sd);
int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char msg[100];		// mesajul trimis

  /* exista toate argumentele in linia de comanda? */
  
  
  /* stabilim portul */
  port = 2026;

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  /* citirea mesajului */
  printf("Lista de comenzi ce pot fi executate:\n");
  printf("creare cont:\n");
  printf("conectare\n");
  printf("deconectare\n");
  printf("reset parola\n");
  printf("creare anunt\n");
  printf("vizualizare produse\n");
  printf("istoric tranzactii\n");
  printf("anunturile mele\n");
  printf("propune oferta\n");
  printf("accept oferta\n");
  printf("ofertele mele\n");

  while(1)
  {
  bzero (msg, 100);
  printf ("Va rugam introduceti numele comenzii pe care doriti sa o executati: ");
  fflush (stdout);
  read (0, msg, 100);
  prelucrareMesaj(msg);
 // printf("%ld",strlen(msg));
  // printf("okkkk");
  /* trimiterea mesajului la server */
  msg[strlen(msg)-1]=0;
  if(strcmp(msg,"deconectare")==0)deconectat=1;
  printf("\n");
  fflush(stdout);
  if (write (sd, msg, 100) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
  if(strcmp(msg,"crearecont")==0)creare_cont(sd);
  if(strcmp(msg,"conectare")==0)logare(sd);
  if(strcmp(msg,"resetparola")==0)resetParola(sd);
  if(strcmp(msg,"creareanunt")==0)creareAnunt(sd);
  if(strcmp(msg,"vizualizareproduse")==0)vizualizareProduse(sd);
  if(strcmp(msg,"propuneoferta")==0)propuneOferta(sd);
  if(strcmp(msg,"istorictranzactii")==0)istoricTranzactii(sd);
  if(strcmp(msg,"stergereanunt")==0)stergeAnunt(sd);
  if(strcmp(msg,"anunturilemele")==0)anunturileMele(sd);
  if(strcmp(msg,"acceptoferta")==0)acceptOferta(sd);
  if(strcmp(msg,"ofertelemele")==0)oferteleMele(sd);
  if(strcmp(msg,"deconectare")==0)deconectare(sd);
  if(strcmp(msg,"stergeanunt")==0)stergeAnunt(sd);
  /* citirea raspunsului dat de server 
     (apel blocant pina cind serverul raspunde) */
	 memset(msg,0,sizeof(msg));
  if (read (sd, msg, 100) < 0)
    {
      perror ("[client]Eroare la read() de la server.\n");
      return errno;
    }
  /* afisam mesajul primit */
  printf ("[client]Mesajul primit este: %s\n", msg);
	   /* inchidem conexiunea, am terminat */
     printf("\n");
  if(strcmp(msg,"Conectare reusita")==0)
   news(sd);
   printf("\n\n");
  }
  close (sd);
}
void creare_cont(int sd)
{
  char nume[Lgnume],parola[Lgparola],gmail[Lgadresa];
 
  printf("introduceti username-ul:");
  fflush(stdout);
  citireAndScriere(nume,sizeof(nume),sd );


  printf("introduceti parola:");
  fflush(stdout);
  citireAndScriere(parola,sizeof(parola),sd);


  printf("introduceti adresa de gmail:");
  fflush(stdout);
  citireAndScriere(gmail,sizeof(gmail),sd);


  printf("nume:%s\n",nume);
  printf("parola:%s\n",parola);
  printf("gmail:%s\n",gmail);
}
void logare(int sd)
{
  char nume[Lgnume],parola[Lgparola],gmail[Lgadresa];
    printf("introduceti username-ul:");
  fflush(stdout);
  citireAndScriere(nume,sizeof(nume),sd );
  printf("introduceti parola:");
  fflush(stdout);
  citireAndScriere(parola,sizeof(parola),sd);
 
 
}
void resetParola(int sd)
{
  char nume[Lgnume], adresa[Lgadresa];
  char parola1[Lgparola],parola2[Lgparola];
  int status;
 printf("Introduceti nume:");
 fflush(stdout);
 citireAndScriere(nume,sizeof(nume),sd);


 printf("Introduceti adresa:");
  fflush(stdout);
 citireAndScriere(adresa,sizeof(adresa),sd);


 if(read(sd,&status,sizeof(int))<0)perror("eroare citire status");
 printf("%d\n",status);
  if(status==1)
  {
    printf("Introduceti parola noua:");
    fflush(stdout);
    citireAndScriere(parola1,sizeof(parola1),sd);
    printf("Reintroduceti parola :");
    fflush(stdout);
    citireAndScriere(parola2,sizeof(parola2),sd);
  }
 }


void creareAnunt(int sd)
{
 char Categorie[Lgcategorie],Brand[Lgbrand],Model[Lgmodel],Pret[Lgpret];
 int conectare;
 if(read(sd,&conectare,sizeof(int))<0)perror("nu s-a citit status de conectare");
 if(conectare==1)
 {
  printf("Momentan pe platforma exista doar urmatoarele categorii:\n Auto | Electronice | Haine | Carti | Altele\n");
 printf("Introduceti Categoria din care face produsul:");
 fflush(stdout);
 citireAndScriere(Categorie,sizeof(Categorie),sd);
printf("Introduceti Brand:");
 fflush(stdout);
 citireAndScriere(Brand,sizeof(Brand),sd);
 printf("Introduceti Model:");
 fflush(stdout);
 citireAndScriere(Model,sizeof(Model),sd);
  printf("Introduceti Pret:");
 fflush(stdout);
 citireAndScriere(Pret,sizeof(Pret),sd);
 }
}
void vizualizareProduse(int sd)
{
  char categorie[Lgcategorie], da[Lgcategorie],brand[Lgbrand];
  char raspuns[Lgraspuns];
   int conectare,ok=0;
   
 if(read(sd,&conectare,sizeof(int))<0)perror("nu s-a citit status de conectare");
 if(conectare==1)
 {
  printf("Momentan pe platforma exista doar urmatoarele categorii:\n Auto | Electronice | Haine | Carti | Altele\n");
  printf("Introduceti categoria de care sunteti interesat:");
  fflush(stdout);
  citireAndScriere(categorie,sizeof(categorie),sd);
  read(sd,&ok,sizeof(int));
   if(ok==1){
  printf("Doriti un anumit brand?In caz afirmativ,va rugam tastati textul \"DA\"  ");
   fflush(stdout);
    citireAndScriere(da,sizeof(da),sd);
  int afirmativ;
   if(read(sd,&afirmativ,sizeof(int))<0)perror("nu s-a citit statusul");
  if(afirmativ==1)
  {
   printf("Introduceti brandul de care sunteti interesat:");
  fflush(stdout);
  citireAndScriere(brand,sizeof(brand),sd);
  }
  printf("\n Se incarca anunturile postate de dvs sub urmatorul format:\n");
  printf("NrAnunt | Categorie | Brand |  Model | Pret cerut | Username vanzator \n");
  printf("\n");
  int seCiteste;
  if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
   read(sd,raspuns,sizeof(raspuns));
   printf(raspuns);
   read(sd,&seCiteste,sizeof(int));
  }
   }
 }
   
}
void propuneOferta(int sd)
{
  char nrAnunt[Nranunt],pret[Lgpret],celalalt[Lgnume];
   int conectare;
 if(read(sd,&conectare,sizeof(int))<0)perror("nu s-a citit status de conectare");
 if(conectare==1)
 {
  printf("Introduceti numarul anuntului pentru produsul care va intereseaza:");
  fflush(stdout);
  citireAndScriere(nrAnunt,sizeof(nrAnunt),sd);
   printf("Introduceti suma pe care doriti sa o acordati:");
  fflush(stdout);
  citireAndScriere(pret,sizeof(pret),sd);
   printf("Introduceti username-ul persoanei careia doriti sa ii trimiteti oferta:");
  fflush(stdout);
  citireAndScriere(celalalt,sizeof(celalalt),sd);
 }
}
void citireAndScriere(char sir[],int size,int sd)
{
  memset(sir,'\0',size);
   read(0,sir,size); 

  sir[strlen(sir)-1]='\0';
  if(write(sd,sir,size)<0)perror("eroare scriere:");
 //  printf("ok");
}
void istoricTranzactii(int sd)
{
  int seCiteste;
  char raspuns[Lgraspuns];
  if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
   read(sd,raspuns,sizeof(raspuns));
   printf(raspuns);
   read(sd,&seCiteste,sizeof(int));
  }
}
void anunturileMele(int sd)
{
  int seCiteste;
  char raspuns[Lgraspuns];
   int conectare;
 if(read(sd,&conectare,sizeof(int))<0)perror("nu s-a citit status de conectare");
 if(conectare==1)
 {
  printf("Se incarca anunturile postate de dvs sub urmatorul format:\n");
  printf("NrAnunt | Categorie | Brand |  Model | Pret cerut\n");
  printf("\n");
  if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
   read(sd,raspuns,sizeof(raspuns));
   printf(raspuns);
   read(sd,&seCiteste,sizeof(int));
  }
 }
}
void stergeAnunt(int sd)
{
 int ok=0;
 char nrAnunt[Nranunt];
 if(read(sd,&ok,sizeof(int))<0)perror("nu s-a citit status de conectare");
 printf("%d",ok);
  if(ok==1)
  {
    printf("introduceti numarul anuntului pe care doriti sa il stergeti:");
    fflush(stdout);
     citireAndScriere(nrAnunt,sizeof(nrAnunt),sd);
  }
}
void oferteleMele(int sd){
  int seCiteste;
  char raspuns[Lgraspuns],raspuns2[Lgraspuns];
   int conectare;
 if(read(sd,&conectare,sizeof(int))<0)perror("nu s-a citit status de conectare");
 if(conectare==1)
 {
  printf("Se incarca ofertele noi  sub formatul:\n");
  printf("NrAnunt | UsernameCumparator | Username vanzator | Suma | Categorie produs | Brand | Model\n");
  printf("\n");
   int status,c1=0,c2=0;
  if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
   read(sd,raspuns,sizeof(raspuns));
   printf(raspuns);
   read(sd,&seCiteste,sizeof(int));
  }
  
   if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
   read(sd,raspuns2,sizeof(raspuns2));
   printf(raspuns2);
   read(sd,&seCiteste,sizeof(int));
  }
 }
}
void news(sd)
{
  printf("Se incarca ofertele noi  sub formatul:\n");
  printf("NrAnunt | Username cumparator | Username vanzator | Suma | Categorie produs | Brand | Model\n");
  printf("\n");
   int status,c1=0,c2=0;
   int seCiteste;
  char raspuns[Lgraspuns],raspuns2[Lgraspuns];

 if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
    c1++;
   read(sd,raspuns,sizeof(raspuns));
   printf(raspuns);
   read(sd,&seCiteste,sizeof(int));
  }
//sprintf("%d",seCiteste);
   if(read(sd,&seCiteste,sizeof(int))<0)perror("nu s-a citit statusul");
  else
  while(seCiteste==1)
  {
    c2++;
   read(sd,raspuns2,sizeof(raspuns2));
   printf(raspuns2);
   read(sd,&seCiteste,sizeof(int));
  }
  if(c1==0&&c2==0)printf("Nu aveti oferte noi\n");
}
void acceptOferta(int sd)
{
  char nranunt[Nranunt],afirmativ[Nranunt],nume[Lgnume];
   int conectare;
 if(read(sd,&conectare,sizeof(int))<0)perror("nu s-a citit status de conectare");
 if(conectare==1)
 {
  printf("Introduceti numarul anuntului pentru care acceptati oferta:");
 fflush(stdout);
 citireAndScriere(nranunt,sizeof(nranunt),sd);

  printf("Daca dvs dunteti cel care vinde, va rugam introduceti textul \"DA\":");
 fflush(stdout);
 citireAndScriere(afirmativ,sizeof(afirmativ),sd); 
 if(strcmp(afirmativ,"DA")==0)
 {
   printf("Va rugam introduceti username-ul celui care cumpara:");
 fflush(stdout);
 citireAndScriere(nume,sizeof(nume),sd); 
 }
 }
}
void deconectare(int sd)
{

}
void prelucrareMesaj(char *msg)
{
  char msg2[100];
  char * token;
  memset(msg2,sizeof(msg2),0);
  token=strtok(msg," ");
  while(token!=NULL)
  {
    strcat(msg2,token);
   token=strtok(NULL," ");
  }
  strcpy(msg,msg2);
}
