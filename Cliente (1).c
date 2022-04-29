#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

struct Datos{
  char nombre[20];
  int opc;
  char respuesta[3000];
  char Peticion[100];
};

int main(){
    int fdr, fdw;
    struct Datos Mensaje;
    char orden[20] ="mkfifo ";
    
    fdw = open("servidor",O_WRONLY);
    if(fdr ==-1)
    {
            perror("");
			exit(1);

    }
    printf("nombre");
    scanf("%s", Mensaje.nombre);
	printf("Hola %s, ingresa el numero 1 para consultar el servicio social, o \n 2 en caso que quieras ir a practicas profesionales \n", Mensaje.nombre);
	scanf("%d", &Mensaje.opc);
    do{
		fdw = open("servidor",O_WRONLY);
		write(fdw,&Mensaje, sizeof(Mensaje));
		fdr = open("Regresar", O_RDONLY);
		
        read(fdr, &Mensaje, sizeof(Mensaje));
        printf(" %s\n",Mensaje.respuesta);
        close(fdr);
		close(fdw);
		
		printf("Necesitas algo mas? \n\r");
		scanf("%d", &Mensaje.opc);
    }
    while(1);
    
    return 0;
}
