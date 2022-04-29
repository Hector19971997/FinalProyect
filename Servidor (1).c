/******************************************************************************pi
Servidor
*******************************************************************************/
/*pipe con nombre fifo 10k*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct Datos{
    char nombre[20];
    int opc;
    char respuesta[3000];
	char Peticion[100]; 
};

struct informacion
{
    long id;
    char msj[3000];
};
int main()
{  	/* Declaramos variables, fdr es para leer el mkfifo, fdw es escribir en el mkfifo, struct datos es el mensaje que se va a usar para
	comunicar cliente y servidor, y fileptr sera la variable que tendra lo que se lea en el texto, ya sea tanto servicio social o prcaticas*/
    int fdr, fdw, mq;
	int fd[2];
	// Estructura que se usara para la comunicacion Cliente-servidor, ambos archivos tienen la misma estructura para no confundir donde va que
    struct Datos info;
	//Variable para leer el documento de la dependencia que se vaya a revisar
	int Fileptr;
	int DelayCounter;
	// Estructuras para cola de mensajes
	struct informacion MensajeSalida, MensajeLlegada;
	
	//Definimos el id del canal que se va a usar para las colas de mensajes
	MensajeSalida.id = 200;
	MensajeLlegada.id = 200;
    
	// Creamos los archivos para llevar a cabo la comunicacion cliente servidor, "servidor" se usa para mandar el request del cliente al servidor, "Regresar" Se usa para devolver el request del cliente, al cliente con la informacion que se requiere
	system("mkfifo servidor"); 
	system("mkfifo Regresar");

	//Obtenemos la direccion del archivo y pedimos acceso de solo lectura
    fdr = open("servidor",O_RDONLY);
	
	//Creamos la conexion de cola de mensajes (inter procesor comm)
	mq = msgget( 124, IPC_CREAT | 600 | IPC_NOWAIT ); 
	
	/*Validamos que se pudo abrir el archivo "servidor" */
    if(fdr ==-1)
    {
         perror("");
        exit(1);

    }
	
	// Leemos el archivo "servidor" con la direccion que obtuvimos con el comando "open"
	//read(fdr,&info, sizeof(info));
    do{
        fdr = open("servidor",O_RDONLY);
		read(fdr,&info, sizeof(info));
        printf("usuario:%s,opcion%i\n",info.nombre,info.opc);
		memset(MensajeSalida.msj, 0, 3000);
		switch(info.opc)
		{
			case 1:
			{
				Fileptr = open("SS.txt",O_RDONLY);
				read(Fileptr,&MensajeSalida.msj, sizeof(MensajeSalida.msj));
				close(Fileptr);
				msgsnd( mq, &MensajeSalida, sizeof (MensajeSalida), 0 );   // mensaje es una variable de tipo  struct buffer
				msgrcv(mq, &MensajeLlegada, sizeof (MensajeLlegada), MensajeLlegada.id,0 );
				strcpy(info.respuesta,MensajeLlegada.msj);
				strcat(info.respuesta, "\n");
				break;
			}
			case 2:
			{
				pipe(fd);
				switch(fork())
				{
					case 0: //hijo - recibir
					{
						close(fd[0]);
						Fileptr = open("PP.txt",O_RDONLY);
						read(Fileptr,&MensajeSalida.msj, sizeof(MensajeSalida.msj));
						printf("%s\n",MensajeSalida.msj);
						write(fd[1],MensajeSalida.msj,sizeof(MensajeSalida.msj));
						close(Fileptr);
						printf("%s\n",MensajeSalida.msj);
						//printf("1");
						//strcpy(info.respuesta,MensajeSalida.msj);
						break;
					}
					default: //padre
					{
						close(fd[1]);
						for(DelayCounter = 0;DelayCounter <=10000;DelayCounter++)
						{
							
						}
						read(fd[0], MensajeSalida.msj, sizeof(MensajeSalida.msj));
						strcpy(info.respuesta,MensajeSalida.msj);
						//close(fd[1]);
						printf("2");
						break;
					}
				}
				
				//Fileptr = open("PP.txt",O_RDONLY);
				//read(Fileptr,&MensajeSalida.msj, sizeof(MensajeSalida.msj));
				//close(Fileptr);
				break;
			}
			default:
			{
				Fileptr = open("Error.txt",O_RDONLY);
				read(Fileptr,&MensajeSalida.msj, sizeof(MensajeSalida.msj));
				close(Fileptr);
				strcpy(info.respuesta,MensajeSalida.msj);
				strcat(info.respuesta, "\n");
				break;
			}
		}
		close(fdr);
        fdw = open("Regresar",O_WRONLY);
        write(fdw,&info, sizeof(info));
        close(fdw);
    }while(1);
	
	
	
    return 0;
}
