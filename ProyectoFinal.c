#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TextFile 0
#define Directory 1

#define LeerServidor 1
#define EscribirAServidor 2
#define TerminarConexion 3


/* Define funtions*/
void SearchCurrentInode(void);
int FillFreeInodeList (void);
void NuevoArchivoTexto(void);
int SizeofTheArray (char *Ptr);
void PrintLIL (void );
void AsignarBloquesLibres(int NumBloques, char Textoescrito[1000], int TamanoVariable, int TipoDeArchivo);
void PrintLBL(void);
int FillInode (int Filesize, int TypeSelect);
void createRootDirectory(void);
void WriteInDirectory(int NumInode);
void PrintDirectory(void);
void CreateNewDir(void);
void BuscarArchivoAEliminar(void);
void LimpiarInodo(int InodoALimpiar);
void IrAdirectorio(void);

struct inode{
int size; //4
int TablaContenidos[5]; //20
char permisos[8]; //6
char TipodeArchivo;
char duenio[10]; //11
char otra[22];//64
};
struct Bloque{
char ContenidoBloque[1024];
};



struct directory
{
int NoI;
char nombre[1024]; //1024
};



int fd;
//int CurrentInode;



/*Define el tamaño del bloque de datos*/
char FileSystem[110][1024];
/* Define el directorio */
struct directory *cwd, raiz[256]={{2,"."},{2,".."}};
//Crea el bootblock con un tamaño de 1024, y el superblock con un tamaño de 1024
char BootBlock[1024]="Soy el bootblock y estoy de adorno!", SBlock[1024];
//Lista de bloques
struct Bloque data[241] ;//= {{2,".",2,".","."}};
//Separa la lista de inodos
struct inode inodeList[3][16]={{{1},{1024,{2},"rwx-wx",'d',"Nadie"}}};
//Define la lista de inodos libres y bloques libres para asignarlos a los archivos
int LIL[10]={0},LBL[10]={12,11,10,9,8,7,6,5,4,3};
//Indicador de posicion actual de la lista de inodos libres
int CurrentLILpos = 0;
int CurrentLBLPos = 9;
int BlocksUsed[10];
char username[10];
int num;
int DirectoryFilePos[100];
int CurrentDirectory = 0;
int PrevDirectory = 0;
int LastDirectory = 0;
char CurrentFileName[10] = {0};
int rootfilePos[20] = {0};

/*Socket */
int SD;
struct sockaddr_in Dir;
struct sockaddr Cliente;
socklen_t longCli;
struct hostent *Host;
char Buff[2000]="--Conectado exitosamente a lonix";

int main()
{
/* Iniciar la comunicacion tcp*/
puts("Buscando al servidor");
Host = gethostbyname("localhost");
if(Host==NULL) exit(1);

puts("inicializando datos");
Dir.sin_family= AF_INET;
Dir.sin_port = 5005;
Dir.sin_addr.s_addr= ((struct in_addr *)(Host->h_addr))->s_addr;

puts("Abriendo socket");
SD=socket(AF_INET, SOCK_STREAM,0);

puts("Conectando");
if (connect(SD, (struct sockaddr *)&Dir, sizeof(Dir))==-1)
{
  close (SD);
  perror("");
  exit(1);
}

Buff[0] = EscribirAServidor;
Buff[1] = 1; //Numero de veces que puede escribir el socket
write(SD, Buff, sizeof(Buff));



//close(SD);
/**/
//system("touch File");

/*Define el tamaño del bloque*/
// char FileSystem[110][1024];
/* Define el directorio */
// struct directory *cwd, raiz[256]={{2,"."},{2,".."}};
//Crea el bootblock con un tamaño de 1024, y el superblock con un tamaño de 2048
// char BootBlock[1024]="size: 110 blocks", SBlock[1024], data[241][1024];
//Separa la lista de inodos
// struct inode inodeList[3][16]={{{1},{1024,{7},"rwx-wx",'d',"RAIZ"}}};
//Define la lista de inodos libres y bloques libres para asignarlos a los archivos
// int LIL[5]={0},LBL[256]={8,9,10,11,12,13};

//system(cd prueba);
/*system("mkdir FileSistem");
fd=open("/home/hector/Documents/FileSystem/FileSistem",O_CREAT|O_WRONLY);
write(fd,BootBlock,sizeof(BootBlock));
write(fd,SBlock,sizeof(SBlock));
write(fd,inodeList,sizeof(inodeList));
write(fd,data,sizeof(data));*/
createRootDirectory();
system("clear");
memset(Buff,0, sizeof(Buff));

printf("Hola Cual es tu nombre %i \n", Buff[0]);
read(SD, Buff, sizeof(Buff));

printf("Bienvenido al sistema %s \n\n",Buff);

/*Llenar la lista de los inodos libres*/
CurrentLILpos = FillFreeInodeList();
memset(Buff,0, sizeof(Buff));
write(SD, Buff, sizeof(Buff));

do{
PrintLIL();
PrintLBL();
PrintDirectory();
printf("\n\r");
printf("Elige un numero\n\n1).-Crear directorio \n 2).-Crear un archivo\n3).-Eliminar archivo o carpeta\n4).-Abrir directorio\n  5).-DirectorioAnterior \n  6).-Salir\n");
scanf("%i", &num);
if(num < 10)
{
switch(num)
{
case 1: //si has elegido el 1 se creara un nuevo directorio
{
CreateNewDir();
printf("\n\r \n\r");
system("clear");
break;
}

case 2://si has elegido el 2
{
//Crea un nuevo archivo con permisos de escritura
NuevoArchivoTexto();
printf("\n\r \n\r");

system("clear");
printf("Se creo el archivo! \n\r");
break;
}

case 3: //si has elegido el 3
{
/*Eliminar archivo*/
BuscarArchivoAEliminar();
system("clear");
break;
}

case 4://si has elegido el 4
{
IrAdirectorio();
system("clear");
//abre un nuevo archivo con permisos de escritura
break;
}
case 5:
{
CurrentDirectory = PrevDirectory;
system("clear");
break;
}
default:
{
	printf("Hasta Pronto %s \n\n\n",username);
	break;
}

}
}
else
{
	num = 6;
}
}while(num!=6);

close(SD);
//close(fd);

return 0;
}

void IrAdirectorio(void)
{
	int ContadorNombreDelArchivo = 2;
	int Coincidencias = 0;
	int Coincidencias2 = 1;
	int TamanoDePalabraABuscar;
	int Eliminar = 0;
	int PosicionDeCoinicidencias;
	int InodoDeDirectorio;
	
	printf("Que directorio quieres ir? \n\r");
	scanf("%s", CurrentFileName);
	TamanoDePalabraABuscar = SizeofTheArray(CurrentFileName);

	do
	{
		/*Revisar que si estamos leyendo un caracter y no un espacio vacio */
		if(data[CurrentDirectory].ContenidoBloque[(ContadorNombreDelArchivo*100)+Coincidencias2] != 0)
		{
			if(data[CurrentDirectory].ContenidoBloque[(ContadorNombreDelArchivo*100)+Coincidencias2] == CurrentFileName[Coincidencias])
			{
				Coincidencias++;
				Coincidencias2++;
				PosicionDeCoinicidencias = ContadorNombreDelArchivo;
			}
			else
			{
				ContadorNombreDelArchivo++;
			}
		}
		else
		{
			/*Final del directorio*/
			ContadorNombreDelArchivo = 11;
		}
	}while(ContadorNombreDelArchivo <= 10);
	if( TamanoDePalabraABuscar == Coincidencias)
	{
		InodoDeDirectorio = data[CurrentDirectory].ContenidoBloque[PosicionDeCoinicidencias*100];
		printf("Este inodo : %i \n\r",InodoDeDirectorio);
		printf("Tenia este bloque: %i \n\r",inodeList[0][InodoDeDirectorio].TablaContenidos[0]);
		PrevDirectory = CurrentDirectory;
		CurrentDirectory = inodeList[0][InodoDeDirectorio].TablaContenidos[0];
	}
}

void LimpiarInodo(int InodoALimpiar)
{
	int RecargarBloques;
	CurrentLILpos++;
	
	/* Regresar el inodo a la lista de inodos libres*/
	LIL[CurrentLILpos] = InodoALimpiar;
	
	/*Recargar los bloques a la lista de bloques libres */
	for(RecargarBloques = inodeList[0][LIL[CurrentLILpos]].size; RecargarBloques != 0; RecargarBloques--)
	{
		CurrentLBLPos++;
		LBL[CurrentLBLPos] = inodeList[0][LIL[CurrentLILpos]].TablaContenidos[RecargarBloques-1];
	}
	
	/*Marcar el inodo como libre poniendo su tamaño como 0*/
	inodeList[0][LIL[CurrentLILpos]].size = 0;
	
}

void BuscarArchivoAEliminar(void)
{
	int ContadorNombreDelArchivo = 2;
	int Coincidencias = 0;
	int Coincidencias2 = 1;
	int TamanoDePalabraABuscar;
	int Eliminar = 0;
	int PosicionDeCoinicidencias;
	int InodoALimpiar;
	
	printf("Que archivo quieres eliminar? \n\r");
	scanf("%s", CurrentFileName);
	TamanoDePalabraABuscar = SizeofTheArray(CurrentFileName);
	printf("El tamano es: %i  \n\r", TamanoDePalabraABuscar);
	do
	{
		/*Revisar que si estamos leyendo un caracter y no un 0*/
		if(data[CurrentDirectory].ContenidoBloque[(ContadorNombreDelArchivo*100)+Coincidencias2] != 0)
		{
			//printf(" Esta es la letra que estoy buscando: %c \n\r", CurrentFileName[Coincidencias]);
			//printf("Y esta este es el objetivo: %c \n\r ",data[CurrentDirectory].ContenidoBloque[(ContadorNombreDelArchivo*100)+Coincidencias2]);
			//printf("Estoy apuntando a:  %i  \n\r ", (ContadorNombreDelArchivo*100)+Coincidencias2);
			if(data[CurrentDirectory].ContenidoBloque[(ContadorNombreDelArchivo*100)+Coincidencias2] == CurrentFileName[Coincidencias])
			{
				Coincidencias++;
				Coincidencias2++;
				PosicionDeCoinicidencias = ContadorNombreDelArchivo;
			}
			else
			{
				ContadorNombreDelArchivo++;
			}
		}
		else
		{
			/*Final del directorio*/
			ContadorNombreDelArchivo = 11;
		}
	}while(ContadorNombreDelArchivo <= 10);
	printf("Coincidio %i veces! \n\r",Coincidencias);
	if( TamanoDePalabraABuscar == Coincidencias)
	{
		InodoALimpiar = data[CurrentDirectory].ContenidoBloque[PosicionDeCoinicidencias*100];
		for(Eliminar = PosicionDeCoinicidencias*100;Eliminar < ((PosicionDeCoinicidencias*100)+Coincidencias2); Eliminar++)
		{
			//printf("Voy a eliminar la posicion: %i \n\r", Eliminar);
			data[CurrentDirectory].ContenidoBloque[Eliminar] = 0;
		}
		LimpiarInodo(InodoALimpiar);
	}
	
}

void PrintDirectory(void)
{
	/*Variable que usaremos para contar desde la posicion 0 del bloque hasta la maxima posicion gesitrada en Rootfilepos (variable que lleva la posicion de cada dirctorio)*/
	int DirectoryStart = 0;
	int Nameprint = 0;
	//printf("Te encuentras en: %i\n\r",CurrentDirectory);
	printf("%i ", data[CurrentDirectory].ContenidoBloque[0]);
	printf("%c \n\r", data[CurrentDirectory].ContenidoBloque[1]);
	DirectoryStart++;
	printf("%i ", data[CurrentDirectory].ContenidoBloque[100]);
	printf("%c ", data[CurrentDirectory].ContenidoBloque[101]);
	printf("%c \n\r", data[CurrentDirectory].ContenidoBloque[102]);
	DirectoryStart++;
	Nameprint = DirectoryStart*100;
	do
	{
		/*Entra la primera vez y nameprint apunta a 200*/
		if(Nameprint%100 == 0)
		{
			printf("%i ", data[CurrentDirectory].ContenidoBloque[Nameprint]);
			Nameprint++;
		}
		else
		{
			if(data[CurrentDirectory].ContenidoBloque[Nameprint] != 0)
			{
				printf("%c", data[CurrentDirectory].ContenidoBloque[Nameprint]);
				Nameprint++;
			}
			else
			{
				printf("\n\r");
				DirectoryStart++;
				Nameprint = DirectoryStart*100;
			}
			
		}
		
	}while( DirectoryStart <= rootfilePos[CurrentDirectory]  );
}

void CreateNewDir(void)
{
	int InodeUsed = 0;
	printf("Ingresa el nombre del nuevo directorio \n\r");
	scanf("%s", CurrentFileName);
	InodeUsed = FillInode ((int)1, Directory);
	WriteInDirectory(InodeUsed);
	AsignarBloquesLibres(0,0,0,Directory);
}

void createRootDirectory(void)
{
/*I am root*/
data[0].ContenidoBloque[0] = 2;
data[0].ContenidoBloque[1] = '.';
data[0].ContenidoBloque[100] = 2;
data[0].ContenidoBloque[101] = '.';
data[0].ContenidoBloque[102] = '.';
rootfilePos[0] = 2;
}



void WriteInDirectory(int NumInode)
{
int copyCounter = 0;
int fileNameCounter = 0;
printf("Voy a Escribir en el directorio! \n\r");

data[CurrentDirectory].ContenidoBloque[rootfilePos[CurrentDirectory]*100] = NumInode;
copyCounter = ((rootfilePos[CurrentDirectory]*100) + 1);

do
{

data[CurrentDirectory].ContenidoBloque[copyCounter] = CurrentFileName[fileNameCounter];
//printf("Este caracter estoy escribiendo en el dir: %c \n\r", data[CurrentDirectory].ContenidoBloque[copyCounter] );
fileNameCounter++;
copyCounter++;

}while(CurrentFileName[fileNameCounter] != 0);


rootfilePos[CurrentDirectory]++;
}



void NuevoArchivoTexto (void)
{
char Textoescrito[10000];
int TamanoVariable;
float NumeroDeBloquesANecesitar =0;
int InodeUsed;

printf("Ingresa el texto que quieres que contenga el archivo \n\r");
scanf("%s", Textoescrito);
//gets(Textoescrito);
TamanoVariable = SizeofTheArray(Textoescrito);
printf("Este es tu texto: %s y el tamaño es: %i \n\r", Textoescrito, TamanoVariable);
NumeroDeBloquesANecesitar = (((float)TamanoVariable)/1024)+1;
printf(" Voy a necesitar %i bloques para este archivo \n\r", (int)NumeroDeBloquesANecesitar);

printf("Ingresa el nombre del archivo \n\r");
scanf("%s", CurrentFileName);

AsignarBloquesLibres(NumeroDeBloquesANecesitar, Textoescrito, TamanoVariable, TextFile);
InodeUsed = FillInode (NumeroDeBloquesANecesitar, TextFile);
WriteInDirectory(InodeUsed);



//CurrentLILpos;
//PrintLBL();

}



int FillInode (int Filesize, int TypeSelect)
{
int BlockusedCounter = 0;
int InodeUsed;

inodeList[0][LIL[CurrentLILpos]].size = Filesize;
strcpy(inodeList[0][LIL[CurrentLILpos]].permisos, "rwx-wx");
printf("Esto es el contenido de mi inodo: \n\r");
do{
inodeList[0][LIL[CurrentLILpos]].TablaContenidos[BlockusedCounter] = BlocksUsed[BlockusedCounter];
printf("Bloques: %i \n\r", inodeList[0][LIL[CurrentLILpos]].TablaContenidos[BlockusedCounter]);
BlockusedCounter++;
}while(BlocksUsed[BlockusedCounter] != 0);

if(TypeSelect == 0)
{
inodeList[0][LIL[CurrentLILpos]].TipodeArchivo = 't';
}
else if(TypeSelect == 1)
{
inodeList[0][LIL[CurrentLILpos]].TipodeArchivo = 'd';
inodeList[0][LIL[CurrentLILpos]].size  = 1;
/*Solo se necesita un solo bloque para el inido*/
inodeList[0][LIL[CurrentLILpos]].TablaContenidos[0] = LBL[CurrentLBLPos];
}
else
{
inodeList[0][LIL[CurrentLILpos]].TipodeArchivo ='e';
}
strcpy(inodeList[0][LIL[CurrentLILpos]].duenio, username);
//for(BlockusedCounter = 0;BlockusedCounter <= ;BlockusedCounter++)

//inodeList[0][LIL[CurrentLILpos]].TablaContenidos;

printf("Inodo: %i \n\r", LIL[CurrentLILpos]);

printf("Tamano: %i \n\r", inodeList[0][LIL[CurrentLILpos]].size);
printf("Permisos: %s \n\r", inodeList[0][LIL[CurrentLILpos]].permisos);
printf("Tipo: %c \n\r", inodeList[0][LIL[CurrentLILpos]].TipodeArchivo);
printf("Dueno: %s \n\r", inodeList[0][LIL[CurrentLILpos]].duenio);

InodeUsed = LIL[CurrentLILpos];
LIL[CurrentLILpos] = 0;
CurrentLILpos--;

return InodeUsed;
}




void AsignarBloquesLibres(int NumBloques, char Textoescrito[1000], int TamanoVariable, int TipoDeArchivo)
{
int WriteOnBlock;
int CharTrans;
int SecondCharTrans;
//printf("Este es tu texto: %s \n\r", Textoescrito);
//printf("", LBL[CurrentLBLPos]);
//printf("Estos son los bloques que se usaron %i \n\r", LBL[CurrentLBLPos-WriteOnBlock]);
if(TipoDeArchivo == TextFile)
{
	BlocksUsed[0]=LBL[CurrentLBLPos-WriteOnBlock];
	for(WriteOnBlock = 0; WriteOnBlock < NumBloques; WriteOnBlock++)
	{
	BlocksUsed[WriteOnBlock]=LBL[CurrentLBLPos-WriteOnBlock];

	/*Quemar un bloque libre*/
	LBL[CurrentLBLPos-WriteOnBlock] = 0;
	do
	{
	if(TamanoVariable < SecondCharTrans)
	{
	/*Out of the do-while*/
	CharTrans = 2000;
	}
	data[LBL[CurrentLBLPos-WriteOnBlock]].ContenidoBloque[CharTrans] = Textoescrito[SecondCharTrans];
	CharTrans++;
	SecondCharTrans++;
	}while(CharTrans<=1024);
	CharTrans = 0;

	printf("Estos son los bloques que se usaron %i \n\r", BlocksUsed[WriteOnBlock]);
	}
	/*Ajustar el indice de la lista de bloques libres*/
	CurrentLBLPos = CurrentLBLPos- (WriteOnBlock);
}
else
{
	if(TipoDeArchivo == Directory)
	{
		if(CurrentDirectory == 0)
		{
			data[LBL[CurrentLBLPos]].ContenidoBloque[0] = 2;
		}
		else
		{
			data[LBL[CurrentLBLPos]].ContenidoBloque[0] = 5;	
		}
		data[LBL[CurrentLBLPos]].ContenidoBloque[1] = '.';
		data[LBL[CurrentLBLPos]].ContenidoBloque[100] = 2;
		data[LBL[CurrentLBLPos]].ContenidoBloque[101] = '.';
		data[LBL[CurrentLBLPos]].ContenidoBloque[102] = '.';
		rootfilePos[LBL[CurrentLBLPos]] = 2;
		
		/*Quemar el bloque libre*/
		LBL[CurrentLBLPos] = 0;
			CurrentLBLPos--;
		
	}
}

}



/*void AsignarInodo( void )
{

}*/



/*Puede que haya alguna funcion que haga esto, pero por el momento se queda*/
int SizeofTheArray (char *Ptr)
{
int Size = 0;
int FinishFlag = 0;
do{
if(*Ptr !=0)
{
Ptr++;
Size++;
}
else{
FinishFlag=1;
}
}while(FinishFlag ==0);
//printf("Texto del apuntador %s, el tamaño es: %i \n\r", *Ptr, Size);
return Size;
}




/*void SearchCurrentInode (void)
{
int LILSubtract = 0;
do
{
if(LILSubtract <= 9)
{
if(LIL[9-LILSubtract] != 0)
{
CurrentInode = LIL[127-LILSubtract] ;
printf("Tengo el inodo! %i , en la posicion %i \n\r", CurrentInode, LILSubtract);
}
else
{
LILSubtract++;
}
}
else
{
//rellenar la lista de inidos libres
}

}while (CurrentInode == 0);
}*/



int FillFreeInodeList (void)
{
int ListaDeInodosPosX = 0;
int ListaDeInodosPosY = 0;
int LILpos = 0;
//printf("Si encontre el inodo! Espero un 1 y encontre un....: %i \n\r ", inodeList[0][2].size);
do
{
for(ListaDeInodosPosY = 2; ListaDeInodosPosY <= 1024; ListaDeInodosPosY++)
{
//printf("Voy en la posicion %i \n\r ", LILpos);
if(inodeList[ListaDeInodosPosX][ListaDeInodosPosY].size == 0)
{
LIL[LILpos] = ListaDeInodosPosY;
LILpos++;
//printf("Que estoy contando? %i \n\r ", LILpos);
}
if(LILpos > 9)
{
/*Ya se lleno la lista de inodos libres*/
ListaDeInodosPosY = 1025;
LILpos = 9;
}
}
}while (LILpos < 9);
//printf("Tengo el indo :%i en la ultima posicion de la LIL \n\r", LILpos);//LIL[9]);
return LILpos;
}

void WriteTheInode (void)
{
	
}

void PrintLIL (void )
{
int Counter = 0;
printf("LIL: \n\r");
for(Counter = 0; Counter<= 9; Counter++)
{
printf("Position %i: %i ", Counter ,LIL[Counter]);
}
printf("\n\r");
}

void PrintLBL (void )
{
int Counter = 0;
printf("LBL: \n\r");
for(Counter = 0; Counter<= 9; Counter++)
{
printf("Position %i: %i ", Counter ,LBL[Counter]);
}
printf("Estoy apuntando a : %i \n\r",LBL[CurrentLBLPos]);
printf("\n\r");
}
