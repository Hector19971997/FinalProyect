#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define LeerServidor 1
#define EscribirAServidor 2
#define TerminarConexion 3

struct Datos{
    char nombre[20];
    int opc;
    char Texto[3000];
};

int main()
{

int D;
struct sockaddr_in S;
struct sockaddr Cliente;
socklen_t longCli;
int C;


char Buff[2000];
struct servent *Puerto;
int NumeroDeVecesAEscribir = 0;

struct Datos info;
int fdr;

puts("Creando socket");
D= socket(AF_INET, SOCK_STREAM, 0);
if (D==-1) exit(1);
puts("Inicializando datos");
// Puerto = getservbyname("time", "tcp");

S.sin_family= AF_INET;
S.sin_port = 5005;
S.sin_addr.s_addr= INADDR_ANY;

puts("Enlazando");
if ( bind(D, (struct sockaddr *)&S, sizeof(S))==-1)
{
close (D);
exit(1);
}
puts("En espera....");
if (listen (D, 1) == -1)
{ close (D);
exit(1);
}

C = accept( D, & Cliente, & longCli);
if (C == -1) exit -1;

read(C, Buff, sizeof(Buff));
system("clear");
printf("%s\n", Buff);

printf("Ingresa el usuario por favor");

do
{
	if(Buff[0] == LeerServidor)
	{
	
	read(C, Buff, sizeof(Buff));
	//system("clear");
	//printf("%s\n", Buff);
	}
	else if(Buff[0] == EscribirAServidor)
	{

		scanf("%s",Buff);
		write(C, Buff, sizeof(Buff));
		Buff[0] = LeerServidor;
		NumeroDeVecesAEscribir--;
		Buff[0] = LeerServidor;
	}
	else if(Buff[0] == TerminarConexion)
	{
		close (C);
		Buff[0] = 0xFF;
	}
	else
	{
		read(C, Buff, sizeof(Buff));
	}
	printf("Lonix@Lonix-FS: ");
	
}while( Buff[0]!= 0xFF );

close (C);

}
