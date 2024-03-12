#include <sys/types.h>
#include<semaphore.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include<sqlite3.h>
#define Lgraspuns 500
#define Lgadresa 30
#define Lgnume 30
#define Lgparola 30
#define Lgcategorie 30
#define Lgbrand 30
#define Lgmodel 30
#define Lgpret 30
#define Nranunt 10
/* portul folosit */
#define PORT 2026
#define UserFilePath "./userConfig.txt"
#define AnuntFile  "./anuntFile"
/* codul de eroare returnat de anumite apeluri */
extern int errno;
sem_t semaforUsers,semaforAnunturi,semaforOferte,semaforTranzactii;
 sqlite3  *db;
void creare_cont(char * msgrasp,int clientDesc);
int logare(char *msgrasp,int clientDesc,char* username);
void citire(char *sir,int size,int fd);
void resetParola(char *msgrasp,int clientDesc);
void creareAnunt(char * msgrasp,char *username,int clientDesc);
int insertDB(int nrOFArgs,char *TableName  ,...);
void vizualizareProduse(char *msgrasp,int clientDesc);
int selectDB(int nrOFArgs,int nrOfColumns,char * TableName, int clientDesc,...);
void cumparaProdus(char *msgrasp,int clientDesc);
void anunturileMele(char *msgrasp,char * username ,int clientDesc);
void istoricTranzactii(char *msgrasp,char *username,int clientDesc);
void stergeAnunt(char *msgrasp,char *username,int clientDesc);
void waitSemafor(sem_t * x);
void postSemafor(sem_t * x);
void setupBD();
void populare();
void connectDB(sqlite3 **x, int * rc);
void deleteDB(int nrOFArgs,char *TableName  ,...);
int suntConectat(char *msgrasp, int conenctat,int clientDesc);
void deconectare(char *msgrasp,char *username,int clientDesc);
void propuneOferta(char *msgrasp,char *username,int clientDesc);
void acceptOferta(char *msgrasp,char *username,int clientDesc);
void oferteleMele(char *msgrasp,char *username,int clientDesc);
void getField(int nrOFArgs,char *Tablename ,char* returnVal,int nrOfColumn,...);
void updateDB(int nrOFArgs,char *TableName, char *columnName, char *newValue,...);
void news(char *username,int client);
int main ()
{
	
	sem_init(&semaforUsers,1,1);
	sem_init(&semaforAnunturi,1,1);
	sem_init(&semaforOferte,1,1);
	sem_init(&semaforTranzactii,1,1);
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client
    int sd;	
	setupBD();
	populare();
    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server]Eroare la socket().\n");
    	return errno;
    }

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server]Eroare la bind().\n");
    	return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 10) == -1)
    {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }

    /* servim in mod concurent clientii... */
    while (1)
    {
    	int client;
    	int length = sizeof (from);
        
    	printf ("[server-procesparinte]Asteptam la portul %d...\n",PORT);
    	fflush (stdout);

    	/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
    	client = accept (sd, (struct sockaddr *) &from, &length);

    	/* eroare la acceptarea conexiunii de la un client */
    	if (client < 0)
    	{
    		perror ("[server-procesparinte]Eroare la accept().\n");
    		continue;
    	}
    
    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client);
    		continue;
			
    	} else if (pid > 0) {
    		// parinte
    		close(client);
    		/*while(waitpid(-1,NULL,WNOHANG));
    		continue;*/
    	} else if (pid == 0) {
    		// copil
    		close(sd);
            int conectat=0,comandaGasita=0;
			char username[Lgnume];
			
           while(1){
    		/* s-a r;ealizat conexiunea, se astepta mesajul */
    		bzero (msg, 100);
    		printf ("[server-procesfiu]Asteptam mesajul...\n");
    		fflush (stdout);

    		/* citirea mesajului */
			
    		if (read (client, msg, 100) <= 0)
    		{
    			perror ("[server-procesfiu]Eroare la read() de la client.\n");
    			close (client);	/* inchidem conexiunea cu clientul */
    			exit(0);	/* continuam sa ascultam */
    		}
			
			
    		printf ("[server-procesfiu]Mesajul a fost receptionat...%s\n", msg);
			
    		/*pregatim mesajul de raspuns */
    		bzero(msgrasp,100);
    	

    		printf("[server-procesfiu]Trimitem mesajul inapoi...%s\n",msgrasp);

            comandaGasita=0;
    		/* returnam mesajul clientului */
			if(strcmp(msg,"conectare")==0){
				comandaGasita=1;
				if(conectat==1)
				{
					strcat(msgrasp,"sunteti deja conectat sub username-ul de ");
					strcat (msgrasp,username);
				}
				else
				{
					memset(username,0,sizeof(username));
				conectat=logare(msgrasp,client,username);}
			
				
				
			}
			if(strcmp(msg,"resetparola")==0){comandaGasita=1;resetParola(msgrasp,client);}
			if(strcmp(msg,"crearecont")==0){comandaGasita=1;creare_cont(msgrasp,client);}
			if(strcmp(msg,"deconectare")==0){comandaGasita=1;deconectare(msgrasp,username,client); conectat=0;}
			
			if(strcmp(msg,"creareanunt")==0){comandaGasita=1;if(suntConectat(msgrasp,conectat,client)==1) creareAnunt(msgrasp,username,client);}
			if(strcmp(msg,"vizualizareproduse")==0){comandaGasita=1; if(suntConectat(msgrasp,conectat,client)==1)vizualizareProduse(msgrasp,client);}
		 if(strcmp(msg,"istorictranzactii")==0){comandaGasita=1;istoricTranzactii(msgrasp,username,client);}
			if(strcmp(msg,"anunturilemele")==0){;comandaGasita=1; if(suntConectat(msgrasp,conectat,client)==1)anunturileMele(msgrasp,username,client);}
            if(strcmp(msg,"stergeanunt")==0){comandaGasita=1;if(suntConectat(msgrasp,conectat,client)==1)stergeAnunt(msgrasp,username,client);}
			if(strcmp(msg,"propuneoferta")==0){comandaGasita=1;  if(suntConectat(msgrasp,conectat,client)==1)propuneOferta(msgrasp,username,client);}
			 if(strcmp(msg,"ofertelemele")==0){comandaGasita=1;  if(suntConectat(msgrasp,conectat,client)==1)oferteleMele(msgrasp,username,client);}
			if(strcmp(msg,"acceptoferta")==0){comandaGasita=1;  if(suntConectat(msgrasp,conectat,client)==1)acceptOferta(msgrasp,username,client);}
		if(comandaGasita==0)strcat(msgrasp,"comanda inexistenta");
			
    		if (write (client, msgrasp, 100) <= 0)
    		{
    			perror ("[server-procesfiu]Eroare la write() catre client.\n");
    			continue;		/* continuam sa ascultam */
    		}
    		else
    			printf ("[server]Mesajul a fost trasmis cu succes.\n");
			if(strcmp(msgrasp,"Conectare reusita")==0)
			news(username,client);
    		/* am terminat cu acest client, inchidem conexiunea */
           }
    		close (client);
    		exit(0);
    	}

    }				/* while */
}				/* main */
void creare_cont(char* msgrasp,int clientDesc)
{
	char nume[Lgnume],adresa[Lgadresa],parola[Lgparola];
	citire(nume,sizeof(nume),clientDesc);
	citire(parola,sizeof(parola),clientDesc);
	citire(adresa,sizeof(adresa),clientDesc);
	char task[400],arg[400];
	memset(task,'\0',sizeof(task));
	strcat(task,"SELECT * FROM Users;");
	sqlite3 *db;
	sqlite3_stmt *res;
	int statusBD;
	waitSemafor(&semaforUsers);
	connectDB(&db,&statusBD);
	if(statusBD==SQLITE_OK)
    {			
     int rc2=sqlite3_prepare_v2(db,task,-1,&res,0);
	  if(rc2!=SQLITE_OK)
	  printf("eroare:%s",sqlite3_errmsg(db));
     int rc = sqlite3_step(res);
	 int usernameNou=1;
while (rc == SQLITE_ROW) {
	 printf("rc");
    const char *Nume = sqlite3_column_text(res, 0);
	if(strcmp(Nume,nume)==0)usernameNou=0;
    rc = sqlite3_step(res);
}
if (rc != SQLITE_DONE) {
    // Handle errors or unexpected conditions
   perror("eroare sssssss");
}
 if(usernameNou==0)
 {
	strcat(msgrasp,"Exista deja un user cu acest nume");
 }
 else{
	char *err;
    memset(task,0,sizeof(task));
	strcat(task,"Insert into Users(Nume,Parola,Gmail,LastConnection) Values(");
	memset(arg,0,sizeof(arg));
	strcat(arg,"\"");
	strcat(arg,nume);
	strcat(arg,"\",\"");strcat(arg,parola);strcat(arg,"\",\"");strcat(arg,adresa);
	strcat(arg,"\" , STRFTIME('%s'));");
	strcat(task,arg);
  statusBD=sqlite3_exec(db,task,0,0,&err);
  if(statusBD==SQLITE_OK)
   strcat(msgrasp,"S-a creat contul");
   else
    {perror("eroare bd:");
	strcat(msgrasp,"Nu s-a creat contul");}
 } 
 
     
     sqlite3_close(db);
	}
	
	
    postSemafor(&semaforUsers);
	
}
int logare(char *msgrasp,int clientDesc,char* username)
{
	char nume[Lgnume],parola[Lgparola];
	citire(nume,sizeof(nume),clientDesc);
	citire(parola,sizeof(parola),clientDesc);
	char task[100],arg[100];
	memset(task,'\0',sizeof(task));
	strcat(task,"SELECT * FROM Users;");
	sqlite3 *db;
	sqlite3_stmt *res;
	int statusBD;
	waitSemafor(&semaforUsers);
	connectDB(&db,&statusBD);
	 int potrivire=0;
    if(statusBD==SQLITE_OK)
    {	
     
	  statusBD=sqlite3_prepare_v2(db,task,-1,&res,0);
	  if(statusBD!=SQLITE_OK) printf("eroare:%s",sqlite3_errmsg(db));
	  else{
		int rc = sqlite3_step(res);
		
		while(rc==SQLITE_ROW)
		{
           const char * Nume=sqlite3_column_text(res,0);
		   const char *Parola=sqlite3_column_text(res,1);
          if(strcmp(parola,Parola)==0&&strcmp(nume,Nume)==0)
		  {potrivire=1;strcat(username,nume);}
		  rc= sqlite3_step(res);
		}
		if (rc != SQLITE_DONE) {
          // Handle errors or unexpected conditions
            perror("eroare sssssss"); 
           } 
	  }
	   sqlite3_close(db);
	}
	if(potrivire==1){strcat(msgrasp,"Ne bucuram ca ai revenit,");
	  strcat(msgrasp,nume);
	  strcat(msgrasp,"!");
	 strcpy(username,nume);
	}
	else strcat(msgrasp,"Conectare nereusita");
	 postSemafor(&semaforUsers);
	return potrivire;
}
void resetParola(char * msgrasp,int clientDesc)
{
	char nume[Lgnume],adresa[Lgadresa],parola1[Lgparola],parola2[Lgparola];
	citire(nume,sizeof(nume),clientDesc);
	citire(adresa,sizeof(adresa),clientDesc);
	char task[100],arg[100];
	memset(task,'\0',sizeof(task));
	strcat(task,"SELECT * FROM Users;");
	sqlite3 *db;
	sqlite3_stmt *res;
	int statusBD;
	waitSemafor(&semaforUsers);
	connectDB(&db,&statusBD);
	 int potrivire=0;
    if(statusBD==SQLITE_OK)
    {	
     
	  statusBD=sqlite3_prepare_v2(db,task,-1,&res,0);
	  if(statusBD!=SQLITE_OK) printf("eroare:%s",sqlite3_errmsg(db));
	  else{
		int rc = sqlite3_step(res);
		
		while(rc==SQLITE_ROW)
		{
           const char * Nume=sqlite3_column_text(res,0);
		   const char *Adresa=sqlite3_column_text(res,2);
          if(strcmp(adresa,Adresa)==0&&strcmp(nume,Nume)==0)potrivire=1;
		  rc= sqlite3_step(res);
		}
		write(clientDesc,&potrivire,sizeof(potrivire));
		if (rc != SQLITE_DONE) {
          // Handle errors or unexpected conditions
            perror("eroare sssssss"); 
           }
		   else 
		   {
			 
			 if(potrivire==0)strcat(msgrasp,"Nu corespund userul si adresa");
			 else
			 
			 {
				citire(parola1,sizeof(parola1),clientDesc);
				citire(parola2,sizeof(parola2),clientDesc);
				if(strcmp(parola1,parola2)!=0)strcat(msgrasp,"Parolele nu sunt identice");
				else{
			      memset(task,0,sizeof(task));
				  strcat(task,"Update Users set parola=");
				  strcat(task,"\"");
				  strcat(task,parola1);
				  strcat(task,"\" where nume=\""); 
				  strcat(task,nume);
				  strcat(task,"\" ;");	
				  printf("\n%s\n",task);
				  char *err;	
				  int st=sqlite3_exec(db,task,0,0,&err);
				  if(st==SQLITE_OK)
				  strcat(msgrasp,"S-a realizat resetarea parolei");
				  else
				  strcat(msgrasp,"Nu s-a realizat resetarea parolei");
				}
			
			 }
		   } 
	  }
	   sqlite3_close(db);
	}
	else { strcat(msgrasp,"Nu s-a realizat resetarea parolei");
          
	}
	 postSemafor(&semaforUsers);
   
}
void citire(char *sir,int size,int fd)
{
	
	memset(sir,'\0',size);
	if(read(fd,sir,size)<0)perror("eroare citire");
	printf("%s  %ld\n",sir,strlen(sir));
   printf("ok\n");
}
void creareAnunt(char *msgrasp,char *username,int clientDesc)
{
	char Categorie[Lgcategorie],Brand[Lgbrand],Model[Lgmodel],Pret[Lgpret];
	citire(Categorie,sizeof(Categorie),clientDesc);
	citire(Brand,sizeof(Brand),clientDesc);
	citire(Model,sizeof(Model),clientDesc);
	citire(Pret,sizeof(Pret),clientDesc);
	waitSemafor(&semaforAnunturi);
   int status=insertDB(10,"Anunturi","Categorie","Brand" ,"Model","Pret","Vanzator",Categorie,Brand,Model,Pret,username);
   postSemafor(&semaforAnunturi);
   if(status==1)strcat(msgrasp,"S-a creat anuntul");
   else
   strcat(msgrasp,"Nu s-a creat anuntul");
}
int insertDB(int nrOFArgs,char *tableName  ,...)
{
	sqlite3 * db;
	sqlite3_stmt *res;
	int statusBD;
	char task[100],arg[400];
	memset(task,'\0',sizeof(task));
	strcat(task,"Insert into ");
	printf("argv");
	strcat(task,tableName);
	strcat(task,"(");
	connectDB(&db,&statusBD);
	if(statusBD==SQLITE_OK)
	{
      va_list args;
	  va_start(args,nrOFArgs);
	  int i;
      for(i=1;i<=nrOFArgs/2;i++)
	    {
			const char *c=va_arg(args,const char *);
			strcat(task,c);
			if(i!=nrOFArgs/2)strcat(task,",");
		}
		strcat(task,") Values(");
		
		 for(i=1;i<=nrOFArgs/2;i++)
	    {
			const char *c=va_arg(args,const char *);
			strcat(task,"\"");
			strcat(task,c);
			strcat(task,"\"");
			if(i!=nrOFArgs/2)strcat(task,",");
		}
		strcat(task,");");
		 va_end(args);
        	char * err;
			statusBD=sqlite3_exec(db,task,0,0,&err);
			printf("\n%s",task);
		sqlite3_close(db);
		if(statusBD==SQLITE_OK)
		return 1;
		else
		    perror("eroare inserare:");
	}
	return 0;
}
void vizualizareProduse(char *msgrasp,int clientDesc)
{
  char Categorie[Lgcategorie],da[Lgcategorie],Brand[Lgbrand];
  int ok=0;
  citire(Categorie,sizeof(Categorie),clientDesc);

  if(strcmp(Categorie,"Altele")==0)ok=1;
 if(strcmp(Categorie,"Haine")==0)ok=1;
 if(strcmp(Categorie,"Auto")==0)ok=1;
 if(strcmp(Categorie,"Electronice")==0)ok=1;
 if(strcmp(Categorie,"Carti")==0)ok=1;
 write(clientDesc,&ok,sizeof(int));
if(ok==1)
{
  citire(da,sizeof(da),clientDesc);
  int afirmativ=0;
  if(strcmp(da,"DA")==0)
  { 
	 afirmativ=1;
	if(write(clientDesc,&afirmativ,sizeof(int))<0)perror("eroare scriere)");
	  citire(Brand,Lgbrand,clientDesc);
  }
else{
	if(write(clientDesc,&afirmativ,sizeof(int))<0)perror("eroare scriere)");
	memset(Brand,sizeof(Brand),0);
	strcpy(Brand,"%");
}	

  waitSemafor(&semaforAnunturi);
   int status=selectDB(4,5,"Anunturi",clientDesc,"Categorie",Categorie,"Brand",Brand);
   postSemafor(&semaforAnunturi);
   if(status==1)strcat(msgrasp,"S-au afisat  anunturile");
   else
   strcat(msgrasp,"Nu exista anunturi care sa corespunda cererii dvs");
}
else
  strcat(msgrasp,"Categorie inexistenta");
}
void anunturileMele(char *msgrasp,char *username,int clientDesc)
{
	waitSemafor(&semaforAnunturi);
   int status=selectDB(2,4,"Anunturi",clientDesc,"Vanzator",username);
   postSemafor(&semaforAnunturi);
   if(status==1)strcat(msgrasp,"S-au afisat  anunturile");
   else
   strcat(msgrasp,"Nu exista anunturi care sa corespunda cererii dvs");
}
int selectDB(int nrOFArgs,int nrOfColumns,char * TableName,int clientDesc, ...)
{
  sqlite3 * db;
	sqlite3_stmt *res;
	int statusBD;
	char task[100],arg[400];
	memset(task,'\0',sizeof(task));
	strcat(task,"Select * from ");
	strcat(task,TableName);
	strcat(task," where ");
	printf("argv");
	connectDB(&db,&statusBD);
	if(statusBD==SQLITE_OK)
	{
      va_list args;
	  va_start(args,nrOFArgs);
	  int i;
      for(i=1;i<=nrOFArgs;i++)
	  {
		const char *c=va_arg(args,const char *);
		if(i%2==1){
		  strcat(task,c);
		 
		}
		else
		{
          if(strcmp(c,"%")==0)strcat(task," LIKE ");
		   else
		     strcat(task,"=");
			strcat(task,"\"");
			strcat(task,c);
			strcat(task,"\"");
			if(i!=nrOFArgs)
			 strcat(task," AND ");
		}
		
	  }
			strcat(task,";");
			printf("\n%s\n",task);
      va_end(args);

      statusBD=sqlite3_prepare_v2(db,task,-1,&res,0);
	  if(statusBD!=SQLITE_OK) printf("eroare:%s",sqlite3_errmsg(db));
	  else{
		int rc = sqlite3_step(res);
		int seScrie=1;
		while(rc==SQLITE_ROW)
		{
		   char raspuns[Lgraspuns];
		    char *x="";
		 printf("Bucla while:\n");
           memset(raspuns,0,sizeof(raspuns));
		   for(int i=0;i<=nrOfColumns;i++)
           {
              x=sqlite3_column_text(res,i);
			  strcat(raspuns,x);
			  strcat(raspuns," ");
			  printf("%d\n",i);
			  
	    	 };
			 strcat(raspuns,"\n");
			 printf("%s",raspuns);
		 if(write(clientDesc,&seScrie,sizeof(int))<0)perror("eroare scriere seScrie");  
    	 if(write(clientDesc,raspuns,sizeof(raspuns))<0)perror("eroare scriere raspuns"); 
		  rc= sqlite3_step(res);
		  
		}
		seScrie=0;
		if(write(clientDesc,&seScrie,sizeof(int))<0)perror("eroare scriere seScrie"); 
		if (rc != SQLITE_DONE) {
            perror("eroare sssssss"); 
			return 0;
           }
	  }
	  
		sqlite3_close(db);
		
		return 1;
	}

	if(write(clientDesc,&statusBD,sizeof(int))<0)perror("eroare scriere");
	return 0;
}
void waitSemafor(sem_t *x){
sem_wait(x);
}
void postSemafor(sem_t *x){
 sem_post(x);
}
void setupBD()
{
 	sqlite3 * db;
	char * err;
	 int status=sqlite3_open("baza.db",&db);
	 if(status!=SQLITE_OK)
	 {perror("conectare nereusita");
	 sqlite3_close(db);
	 }
	  char *sql1 ="CREATE TABLE IF NOT EXISTS Users(Nume Varchar(30),Parola Varchar(30),Gmail Varchar(30),LastConnection Varchar(30));" ;
        char *sql2="CREATE TABLE IF NOT EXISTS Tranzactii(Nume Varchar(30),Actiuni Varchar(30),Brand Varchar(30),Model Varchar(30),Suma Varchar(30));" ;
	char *sql3="CREATE TABLE IF NOT EXISTS Anunturi(Nranunt INTEGER PRIMARY KEY,Categorie Varchar(30),Brand Varchar(30),Model Varchar(30),Pret Varchar(30),Vanzator Varchar(30));";	
	char *sql4="Create Table IF NOT EXISTS Of(NrAnunt INT,Cumparator Varchar(30) ,Vanzator Varchar(30) ,Pret Varchar(30),Categorie Varchar(30),Brand Varchar(30),Model Varchar(30),Data Varchar(30),SeenC Varchar(30) ,SeenV Varchar(30),AcceptC Varchar(30),AcceptV Varchar(30));";
    status=sqlite3_exec(db,sql1,0,0,&err) ;  
  status=sqlite3_exec(db,sql2,0,0,&err) ;  
  status=sqlite3_exec(db,sql3,0,0,&err);
   status=sqlite3_exec(db,sql4,0,0,&err);
    if(status!=0)
	{
		    fprintf(stderr, "SQL error: %s\n", err);
        
        sqlite3_free(err);        
        sqlite3_close(db);
	}
	sqlite3_close(db);
}

void connectDB(sqlite3 ** db ,int *rc )
{
	
 *rc=sqlite3_open("baza.db",db);
 if((*rc)!=0)
{
   fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
   sqlite3_close(*db);
}
}
void updateDB(int nrOFArgs,char *TableName,char * columnName, char*newValue,...)
{
   sqlite3 * db;
	sqlite3_stmt *res;
	int statusBD;
	char task[100],arg[400];
	memset(task,'\0',sizeof(task));
	strcat(task,"Update ");
	strcat(task,TableName);
	strcat(task," Set ");
	 strcat(task,columnName);
	 strcat(task," =");
	 strcat(task,newValue);
	 strcat(task," WHERE ");
	connectDB(&db,&statusBD);
	if(statusBD==SQLITE_OK)
	{
       va_list args;
	  va_start(args,nrOFArgs);
	  int i;
      for(i=1;i<=nrOFArgs;i++)
	  {
		const char *c=va_arg(args,const char *);
		if(i%2==1){
		  strcat(task,c);
		 
		}
		else
		{
          if(strcmp(c,"%")==0)strcat(task," LIKE ");
		   else
		     strcat(task,"=");
			strcat(task,"\"");
			strcat(task,c);
			strcat(task,"\"");
			if(i!=nrOFArgs)
			 strcat(task," AND ");
		}
		
	  }
			strcat(task,";");va_end(args);
        	char * err;
			statusBD=sqlite3_exec(db,task,0,0,&err);
			printf("\n%s\n",task);
			if(statusBD!=SQLITE_OK)perror("eroare");
         sqlite3_close(db);
	  }
	  
		
}
void propuneOferta(char *msgrasp, char *username,int clientDesc)
{
	char nranunt[Nranunt],suma[Lgpret],celalalt[Lgnume];
	char vanzator[Lgnume],Categorie[Lgcategorie],Model[Lgmodel],Brand[Lgbrand];
	memset(vanzator,0,sizeof(vanzator));
	memset(Categorie,0,sizeof(Categorie));
	memset(Brand,0,sizeof(Categorie));
	memset(Model,0,sizeof(Model));



	char * categorie,*brand,*model;
	citire(nranunt,sizeof(nranunt),clientDesc);
	citire(suma,sizeof(suma),clientDesc);
    citire(celalalt,sizeof(celalalt),clientDesc);

    waitSemafor(&semaforAnunturi);
	getField( 2,"Anunturi",vanzator,5,"Nranunt",nranunt);
    getField(2,"Anunturi",Categorie,1,"Nranunt",nranunt);
    getField(2,"Anunturi",Brand,2,"Nranunt",nranunt);
    getField(2,"Anunturi",Model,3,"Nranunt",nranunt);
  postSemafor(&semaforAnunturi);

	if(strcmp(vanzator,"")==0)strcat(msgrasp,"Numarul anuntului este incorect");
     else
	 {
		char v2[Lgnume];
		memset(v2,0,sizeof(int));
		waitSemafor(&semaforOferte);
		getField(4,"Of",v2,2,"Nranunt",nranunt,"Vanzator",username);
		if(strcmp(v2,"")==0)
		  getField(4,"Of",v2,2,"Nranunt",nranunt,"Cumparator",username);
		postSemafor(&semaforOferte);	
		if(strcmp(v2,"")==0)
		{
			waitSemafor(&semaforOferte);
       printf("Inserare status:%d:",  insertDB( 22,"Of","Nranunt","Cumparator","Vanzator","Pret","Categorie","Brand","Model","SeenC","SeenV","AcceptC","AcceptV", nranunt,username,      vanzator,suma,  Categorie , Brand , Model  ,"1"    ,"0"    ,"1"       ,"0"));
			postSemafor(&semaforOferte);				                                                                                   
		}
		 else{
				waitSemafor(&semaforOferte);
                if(strcmp(vanzator,username)!=0)
			{updateDB( 2,"Of","Pret",suma  ,"Nranunt",nranunt );
             updateDB( 2,"Of","SeenC", "1"  ,"Nranunt",nranunt );
			 updateDB(2,"Of","SeenV", "0"  ,"Nranunt",nranunt);
			 updateDB(2,"Of","AcceptC","1","Nranunt",nranunt);
			 updateDB(2,"Of","AcceptV","0","Nranunt",nranunt);
			}
				 else
				 {
					updateDB( 4,"Of","Pret",suma  ,"Nranunt",nranunt,"Cumparator",celalalt );
             updateDB( 4,"Of","SeenC", "0"  ,"Nranunt",nranunt,"Cumparator",celalalt );
			 updateDB(4,"Of","SeenV", "1"  ,"Nranunt",nranunt,"Cumparator",celalalt);
			 updateDB(4,"Of","AcceptC","0","Nranunt",nranunt,"Cumparator",celalalt);
			 updateDB(4,"Of","AcceptV","1","Nranunt",nranunt,"Cumparator",celalalt);
				 }
              postSemafor(&semaforOferte);

		 }
		   strcat(msgrasp,"Oferta trimisa");
}
	 }

		

void getField(int nrOFArgs,char *Tablename ,char* returnVal,int nrOfColumn,...)
{
	sqlite3 * db;
	sqlite3_stmt *res;
	int statusBD;
	char task[100],arg[400];
	memset(task,'\0',sizeof(task));
	strcat(task,"Select * from ");
	strcat(task,Tablename);
	strcat(task," where ");
   //	printf("argv");
	connectDB(&db,&statusBD);
	if(statusBD==SQLITE_OK)
	{
      va_list args;
	  va_start(args,nrOFArgs);
	  int i;
      for(i=1;i<=nrOFArgs;i++)
	  {
		const char *c=va_arg(args,const char *);
		if(i%2==1){
		  strcat(task,c);
		 
		}
		else
		{
          if(strcmp(c,"%")==0)strcat(task," LIKE ");
		   else
		     strcat(task,"=");
			strcat(task,"\"");
			strcat(task,c);
			strcat(task,"\"");
			if(i!=nrOFArgs)
			 strcat(task," AND ");
		}
		
	  }
			strcat(task,";");
			printf("\n%s\n",task);
      va_end(args);

      statusBD=sqlite3_prepare_v2(db,task,-1,&res,0);
	  if(statusBD!=SQLITE_OK) printf("eroare:%s",sqlite3_errmsg(db));
	  else{
		int rc = sqlite3_step(res);
	      while(rc==SQLITE_ROW)
		  {
            const char *x;
			x=sqlite3_column_text(res,nrOfColumn);
			printf("%s",x);
			printf("\n");
			strcpy(returnVal,x);
			rc= sqlite3_step(res);
		  }
		  if (rc != SQLITE_DONE) {
            perror("eroare sssssss"); 
			
           }
	  }
	    sqlite3_close(db);
	}

}
void acceptOferta(char *msgrasp,char *username, int clientDesc)
{
	char nranunt[Nranunt], afirmativ[Nranunt],cumparator[Lgnume];
	char acc[10];
	char Model[Lgmodel],Brand[Lgbrand],Pret[Lgpret];
	memset(Model,0,sizeof(Model));
	memset(Brand,0,sizeof(Brand));
    memset(Pret,0,sizeof(Pret));
	

	citire(nranunt,sizeof(nranunt),clientDesc);
	citire(afirmativ,sizeof(afirmativ),clientDesc);
	if(strcmp(afirmativ,"DA")==0)
	 {
		citire(cumparator,sizeof(cumparator),clientDesc);
		waitSemafor(&semaforOferte);
		
		getField( 4,"Of",acc,10,"NrAnunt",nranunt,"Cumparator",cumparator);
		getField(4,"Of",Model, 6,"NrAnunt",nranunt,"Cumparator",cumparator);
		getField(4,"Of",Brand, 5,"NrAnunt",nranunt,"Cumparator",cumparator);
		getField(4,"Of",Pret, 3,"NrAnunt",nranunt,"Cumparator",cumparator);
		postSemafor(&semaforOferte);
		printf("okkk");
		if(strcmp(acc,"1")==0){
			waitSemafor(&semaforTranzactii);
			insertDB( 10,"Tranzactii","Nume","Actiuni","Brand","Model","Suma",username,"a vandut",Brand,Model,Pret);
			insertDB( 10,"Tranzactii","Nume","Actiuni","Brand","Model","Suma",cumparator,"a cumparat",Brand,Model,Pret);
		    postSemafor(&semaforTranzactii);
			waitSemafor(&semaforOferte);
            deleteDB( 4, "Of","NrAnunt",nranunt,"Cumparator",cumparator);
			   postSemafor(&semaforOferte);
			strcat(msgrasp,"S-a inregistrat tranzactia");
		}
		else
		  strcat(msgrasp,"Asteptam raspunsul celeilalte parti");
	 }
	 else
	 {
		char v3[Lgnume];
		waitSemafor(&semaforOferte);
		getField(4,"Of",v3,11,"NrAnunt",nranunt,"Cumparator",username);
		getField( 4,"Of",acc,11,"NrAnunt","Cumparator",username);
		postSemafor(&semaforOferte);
		if(strcmp(acc,"1")==0){
           
		   waitSemafor(&semaforTranzactii);
			insertDB( 10,"Tranzactii","Nume","Actiuni","Brand","Model","Suma",v3,"a vandut",Brand,Model,Pret);
			insertDB( 10,"Tranzactii","Nume","Actiuni","Brand","Model","Suma",username,"a cumparat",Brand,Model,Pret);
		    postSemafor(&semaforTranzactii);
			strcat(msgrasp,"S-a inregistrat tranzactia");
			  waitSemafor(&semaforOferte);
            deleteDB( 4, "Of","NrAnunt",nranunt,"Cumparator",username);
			   postSemafor(&semaforOferte);
		}
		else
		  strcat(msgrasp,"Asteptam raspunsul celeilalte parti");
	 }
}
void deconectare(char *msgrasp,char *username, int clientdesc)
{
    if(strlen(username)==0)strcat(msgrasp,"Erati deja deconectat");
	else
	{
		waitSemafor(&semaforUsers);
	updateDB(2,"Users","LastConnection","STRFTIME('%s')","Nume",username);
	postSemafor(&semaforUsers);
	strcat(msgrasp,"Ati fost deconectat");

	}
}
void deleteDB(int nrOFArgs,char *TableName  ,...)
{
  sqlite3 * db;
	sqlite3_stmt *res;
	int statusBD;
	char task[100],arg[400];
	memset(task,'\0',sizeof(task));
	strcat(task,"Delete From ");
	strcat(task,TableName);
	strcat(task," where ");
   //	printf("argv");
	connectDB(&db,&statusBD);
	if(statusBD==SQLITE_OK)
	{
      va_list args;
	  va_start(args,nrOFArgs);
	  int i;
      for(i=1;i<=nrOFArgs;i++)
	  {
		const char *c=va_arg(args,const char *);
		if(i%2==1){
		  strcat(task,c);
		 
		}
		else
		{
          if(strcmp(c,"%")==0)strcat(task," LIKE ");
		   else
		     strcat(task,"=");
			strcat(task,"\"");
			strcat(task,c);
			strcat(task,"\"");
			if(i!=nrOFArgs)
			 strcat(task," AND ");
		}
	  }
	   va_end(args);
        	char * err;
			statusBD=sqlite3_exec(db,task,0,0,&err);
			printf("\n%s",task);
		sqlite3_close(db);
		if(statusBD!=SQLITE_OK)
		    perror("eroare inserare:");
	}	
	
}
int suntConectat(char *msgrasp,int conectat,int clientDesc)
{
	if(conectat==0){strcat(msgrasp,"Trebuie sa va conectati pentru aceasta optiune");}
	
    write(clientDesc,&conectat,sizeof(int));
	return conectat;
}
void oferteleMele(char *msgrasp, char *username,int clientDesc)
{
 waitSemafor(&semaforOferte);
	int x=selectDB( 2,5,"Of",clientDesc,"Vanzator",username);
	updateDB(2, "Of","SeenV","\"1\"","Vanzator",username);
	postSemafor(&semaforOferte);
	waitSemafor(&semaforOferte);
	 x=selectDB( 2,5,"Of",clientDesc,"Cumparator",username);
	  updateDB(2, "Of","SeenC","\"1\"","Cumparator",username);
	postSemafor(&semaforOferte);
	strcat(msgrasp,"S-au incarcat ofertele pentru facute de dvs si pentru dvs");
}
void istoricTranzactii(char *msgrasp,char *username,int clientDesc)
{
	waitSemafor(&semaforTranzactii);
	int x=selectDB( 2,4,"Tranzactii",clientDesc,"Nume",username);
	postSemafor(&semaforTranzactii);
}
void news(char *username,int clientDesc){
  waitSemafor(&semaforOferte);
	int x=selectDB( 4,6,"Of",clientDesc,"Vanzator",username,"SeenV","0");
	printf("%d",x);
	 x=selectDB( 4,6,"Of",clientDesc,"Cumparator",username,"SeenC","0");
	 printf("%d",x);
	 updateDB(2, "Of","SeenV","\"1\"","Vanzator",username);
	 updateDB(2, "Of","SeenC","\"1\"","Cumparator",username);
	postSemafor(&semaforOferte);
}
void stergeAnunt(char *msgrasp,char *username,int clientDesc)
{
	char nrAnunt[Nranunt],v[Lgnume];
	memset(v,0,sizeof(v));
	citire(nrAnunt,sizeof(nrAnunt),clientDesc);
    
	waitSemafor(&semaforAnunturi);
   getField(4,"Anunturi",v,5,"Nranunt",nrAnunt,"Vanzator",username);
	postSemafor(&semaforAnunturi);
	if(strcmp(v,"")==0)strcat(msgrasp,"Nu ati creat un asemenea anunt");
	else
	{
		waitSemafor(&semaforAnunturi);
     deleteDB(2, "Anunturi","Nranunt",nrAnunt);
	postSemafor(&semaforAnunturi);
	waitSemafor(&semaforOferte);
     deleteDB(2, "Of","Nranunt",nrAnunt);
	postSemafor(&semaforOferte);
		strcat(msgrasp,"Anunt sters");
	}
}
void populare()
{
	waitSemafor(&semaforUsers);
	insertDB( 6,"Users","Nume","Parola","Gmail","matei","a","matei@gmail.com");
	insertDB( 6,"Users","Nume","Parola","Gmail","vass","vass","vas@gmail.com");
	postSemafor(&semaforUsers);

	waitSemafor(&semaforAnunturi);
    insertDB( 10,"Anunturi","Categorie","Brand","Model","Pret","Vanzator","Auto","BMW","M3","12222","matei");
	insertDB( 10,"Anunturi","Categorie","Brand","Model","Pret","Vanzator","Electronice","Samsung","S2","12222","vass");
	postSemafor(&semaforAnunturi);
}
