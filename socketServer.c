# include < sys/socket.h >
 # include <sys/types.h> 
# include <sys/un.h> 
# include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{

int D;
struct sockaddr_in S;
struct sockaddr Cliente;
socklen_t longCli;
int C;

char Buff[100]="Mensaje del servidor por tcp";
struct servent *Puerto;

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

printf("Recibido: %s\n", Buff);
strcpy(Buff,"Respuesta del servidor");
write(C, Buff, sizeof(Buff));
close (C);

}
