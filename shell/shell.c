//Jaime Roade Conejo y Jorge Rodríguez Rivera
//jaime.roade y jorge.rodriguez.rivera
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/shm.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include "lista.h"
#include "lista2.h"

#define MAXLINEA 4096
#define COMMAND 256
#define LEERCOMPLETO ((ssize_t)-1)

char meses[][12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char linea[MAXLINEA+1];
int xG, yG, zG;

typedef struct {
    char command[COMMAND];
} Element;
typedef struct {
    unsigned long long *direccion;
    long long tam;
    char time[COMMAND];
    char command[COMMAND];
    int key;
} ElementAllocate;

int TrocearCadena(char * cadena, char * trozos[]);
void cmdAutores(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdGetPid(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdGetPPid(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdPwd(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdChdir(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdFecha(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdHora(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdHistoric(int argc, char *argv[], List *listH, List *listA, Node *listE);
void openDir(char *argv, int n);
void cmdCreateDir(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdListLong(char *argv, int n, DIR *dirp, char *contenido);
void cmdDelete(int argc, char *argv[], List *listH, List *listA, Node *listE);
void cmdExit(int argc, char *argv[], List *listH, List *listA, Node *listE);

typedef struct _comando{
    char *nombre;
    void(*f)(int argc, char *argv[], List *listH, List *listA, Node *listE);
}comando;

comando tablaComandos[];

int TrocearCadena(char * cadena, char * trozos[])
{
    int i=1;
    if((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}


void cmdAutores(int argc, char *argv[], List *listH, List *listA, Node *listE){
    if (argc==2 && strcmp(argv[1], "-l")==0)
        printf("login: jaime.roade\nlogin: jorge.rodriguez.rivera\n");
    else if (argc==1)
        printf("Jaime Roade Conejo\nJorge Rodríguez Rivera\nlogin: jaime.roade\nlogin: jorge.rodriguez.rivera\n");
    else if (argc==2 && strcmp(argv[1], "-n")==0)
        printf("Jaime Roade Conejo\nJorge Rodríguez Rivera\n");
    else printf("Comando no válido\n");

}

void cmdGetPid(int argc, char *argv[], List *listH, List *listA, Node *listE){
    pid_t process_id;
    process_id = getpid();
    printf("%d\n", process_id);
}

void cmdGetPPid(int argc, char *argv[], List *listH, List *listA, Node *listE){
    pid_t p_process_id;
    p_process_id = getppid();
    printf("%d\n", p_process_id);
}

void cmdPwd(int argc, char *argv[], List *listH, List *listA, Node *listE){
    char cwd[MAXLINEA];
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);
}

void cmdChdir(int argc, char *argv[], List *listH, List *listA, Node *listE){
    if (argc==1) {
        char cwd[MAXLINEA];
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
    }
    else if (argc>1) if(chdir(argv[1])!=0) printf("Error: %s\n", strerror(errno));
}

void cmdFecha(int argc, char *argv[], List *listH, List *listA, Node *listE){
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output,128,"%d/%m/%y",tlocal);
    printf("%s\n",output);
}

void cmdHora(int argc, char *argv[], List *listH, List *listA, Node *listE){
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output,128,"%H:%M:%S",tlocal);
    printf("%s\n",output);
}
void liberarHis(Element *element){
    free(element);
}

void procesoComandos(int ntrozos, char *trozos[], List listH, List *listA, Node *listE){
    int i;
    for(i=0; tablaComandos[i].nombre!= NULL; i++)
        if(strcmp(tablaComandos[i].nombre, trozos[0])==0){
            tablaComandos[i].f(ntrozos,trozos, &listH, listA, listE);

            break;
        }
    if(tablaComandos[i].nombre==NULL) printf("comando no reconocido\n");
}

void cmdHistoric(int argc, char *argv[], List *listH, List *listA, Node *listE){
    if(argc==1) {
        for(pos n = first(listH); !end(listH, n); n = next(listH, n)) {
            Element *puntero = element(listH, n);
            printf("%d->%s", n + 1, puntero->command);
        }
    }
    else if(argc == 2 && strcmp(argv[1], "-c")==0)
        ClearList(listH, (void (*)(void *)) liberarHis);

    else if(strncmp(argv[1], "-r", 2) == 0) {
        pos numero = atoi(&argv[1][2]) - 1;
        if (size(listH) >= numero && numero >= 0) {
            int ntrozos; char *trozos[200]; char aux[COMMAND];
            Element *puntero = element(listH, numero);
            strcpy(aux, puntero->command);
            printf("Executing command %s", aux);
            ntrozos = TrocearCadena(aux, trozos);
            procesoComandos(ntrozos, trozos, *listH, listA, listE);
        }
        else printf("Error en el comando\n");
    }
    else if(strncmp(argv[1], "-", 1) == 0){
        int numero = atoi(&argv[1][1]);
        if (size(listH)>=numero && numero > 0)
            for(pos n = first(listH); n < numero; n = next(listH, n)) {
                Element *puntero = element(listH, n);
                printf("%d->%s", n + 1, puntero->command);
            }
    }
}

void openDir(char *argv, int n){
    DIR *dirp;
    struct dirent *dp;
    char directorio_apoyo[MAXLINEA];
    dirp = opendir(argv);
    if (!dirp) {
        perror("opendir()");
        exit(1);
    }
    else {
        printf("********* %s\n", argv);
        while ((dp = readdir(dirp))) {
            if(n == 0 || n == 4 || n == 6 || n == 2 || n == 7) {  //list -dir argv[] || list -dir -rec || list -rec -dir || list -rec
                if (strncmp(dp->d_name, ".", 1) != 0) {
                    strcpy(directorio_apoyo, argv);
                    strcat(directorio_apoyo, "/");
                    strcat(directorio_apoyo, dp->d_name);
                    if(n == 0 || n == 4 || n == 6)
                        cmdListLong(directorio_apoyo, 1, dirp, dp->d_name);
                    else cmdListLong(directorio_apoyo, 0, dirp, dp->d_name); //list -long -dir argv[] || list -dir -long argv[]
                }
            }
            else if(n == 1 || n == 3 || n == 5) {//list -dir -hid argv[] || list -hid -dir argv[]
                strcpy(directorio_apoyo, argv);
                strcat(directorio_apoyo, "/");
                strcat(directorio_apoyo, dp->d_name);
                if(n == 5)
                    cmdListLong(directorio_apoyo, 0, dirp, dp->d_name);
                else
                    cmdListLong(directorio_apoyo, 1, dirp, dp->d_name);
            }
        }
    }
    if (n == 4 || n == 6 || n == 7) {                 //list -rec -dir argv[] || list -dir -rec argv[]
        closedir(dirp);
        dirp = opendir(argv);
        while ((dp = readdir(dirp)) != NULL) {
            if (strncmp(dp->d_name, ".", 1) != 0) {
                if (dp->d_type == DT_DIR) {
                    strcpy(directorio_apoyo, argv);
                    strcat(directorio_apoyo, "/");
                    strcat(directorio_apoyo, dp->d_name);
                    if(n == 4)
                        openDir(directorio_apoyo, 4);
                    else if(n == 6)
                        openDir(directorio_apoyo, 6);
                    else
                        openDir(directorio_apoyo, 7);
                }
            }
        }
    }
    if (n == 3 || n == 5) {               //list -rec -hid -dir argv[] || list -hid -rec -dir argv[]
        closedir(dirp);
        dirp = opendir(argv);
        while ((dp = readdir(dirp)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                if (dp->d_type == DT_DIR) {
                    strcpy(directorio_apoyo, argv);
                    strcat(directorio_apoyo, "/");
                    strcat(directorio_apoyo, dp->d_name);
                    if (n == 3)
                        openDir(directorio_apoyo, 3);
                    else
                        openDir(directorio_apoyo, 5);
                }
            }
        }
    }
}

void cmdCreateDir(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    char cwd[MAXLINEA];
    getcwd(cwd, sizeof(cwd));
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-dir") == 0)) openDir(cwd, 0);

    else if (argc == 2 && strcmp(argv[1], "-dir") != 0) open(argv[1], O_CREAT, S_IRWXU);

    else if (argc == 3 && strcmp(argv[1], "-dir") == 0) mkdir(argv[2], S_IRWXU);
}

char LetraTF (mode_t m){
    switch (m & S_IFMT) { /*and bit a bit con los bits de formato, 0170000 */
        case S_IFSOCK:	return 's'; /*socket */
        case S_IFLNK:	return 'l'; /*symbolic link*/
        case S_IFREG:	return '-'; /*fichero normal*/
        case S_IFBLK:	return 'b'; /*block device*/
        case S_IFDIR:	return 'd'; /*directorio */
        case S_IFCHR:	return 'c'; /*char device*/
        case S_IFIFO:	return 'p'; /*pipe*/
        default:		return '?'; /*desconocido, no deberia aparecer*/
    }
}

char * ConvierteModo (mode_t m, char *permisos){
    strcpy (permisos,"---------- ");
    permisos[0]=LetraTF(m);
    if (m & S_IRUSR) permisos[1]='r'; /*propietario*/
    if (m & S_IWUSR) permisos[2]='w';
    if (m & S_IXUSR) permisos[3]='x';
    if (m & S_IRGRP) permisos[4]='r'; /*grupo*/
    if (m & S_IWGRP) permisos[5]='w';
    if (m & S_IXGRP) permisos[6]='x';
    if (m & S_IROTH) permisos[7]='r'; /*resto*/
    if (m & S_IWOTH) permisos[8]='w';
    if (m & S_IXOTH) permisos[9]='x';
    if (m & S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
    if (m & S_ISGID) permisos[6]='s';
    if (m & S_ISVTX) permisos[9]='t';
    return permisos;
}

void cmdListLong(char *argv, int n, DIR *dirp, char *contenido) {
    struct stat sb;
    struct group *grp;
    struct passwd *pwd;
    char campo[10];
    stat(argv, &sb);
    char month[10], day[10];
    strftime(month, 10, "%m", gmtime(&(sb.st_ctime)));
    strftime(day, 10, "%d %H:%M", localtime(&(sb.st_ctime)));
    int monthNumb = atoi(month);
    grp = getgrgid(sb.st_gid);
    pwd = getpwuid(sb.st_uid);
    if (lstat(argv, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    } else {
        if (n == 1) {
            stat(argv, &sb);
            printf("%8ld\t%5s  \n", sb.st_size, contenido);
        } else {
            switch (sb.st_mode & S_IFMT) {
                case S_IFLNK: {
                    char *buf;
                    ssize_t bufsiz, nbytes;
                    bufsiz = sb.st_size + 1;
                    if (sb.st_size == 0)
                        bufsiz = PATH_MAX;
                    buf = malloc(bufsiz);
                    if (buf == NULL) {
                        perror("malloc");
                        exit(EXIT_FAILURE);
                    }
                    nbytes = readlink(argv, buf, bufsiz);
                    printf("%s %s %ld %s %s %s %ld (%ld) %s -> %.*s\n", meses[monthNumb - 1], day,
                           (long) sb.st_ino, grp->gr_name, pwd->pw_name, ConvierteModo(sb.st_mode, campo), sb.st_size,
                           sb.st_nlink, contenido, (int) nbytes, buf);
                    free(buf);
                    break;
                }
                default: {
                    printf("%s %s %ld %s %s %s %ld (%ld) %s\n", meses[monthNumb - 1], day,
                           (long) sb.st_ino, grp->gr_name, pwd->pw_name, ConvierteModo(sb.st_mode, campo), sb.st_size,
                           sb.st_nlink, contenido);
                    break;
                }
            }
        }
    }
}

void cmdList(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    struct stat sb;
    char cwd[MAXLINEA], apoyo[MAXLINEA];
    getcwd(cwd, sizeof(cwd));
    int dir = 0, rec = 0, lon = 0, hid = 0;
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-hid") == 0) hid = 1;
        else if(strcmp(argv[i], "-rec") == 0) rec = 1;
        else if(strcmp(argv[i], "-long") == 0) lon = 1;
        else if(strcmp(argv[i], "-dir") == 0) dir = 1;
        else break;
    }
    if (argc == 1 || (dir == 1 && argc == 2)){
        openDir(cwd, 0);//list || list -dir
    }

    else if ((rec == 1 && argc == 2) || (rec == 1 && dir == 1 && argc == 3)) { //list -rec || list -dir -rec || list -rec -dir
        openDir(cwd, 6);
    }

    else if((hid == 1 && argc == 2) || (hid == 1 && dir == 1 && argc == 3)){ //list -hid || list -hid -dir || list -dir -hid
        openDir(cwd, 1);
    }
    else if ((lon == 1 && argc == 2) ||
            ((lon == 1 && dir == 1) && argc == 3)){   //list -long || list -long -dir || list -dir -long
        openDir(cwd, 2);
    }

        // list -hid -long -dir || list -hid -long
    else if ((lon == 1 && hid == 1 && argc == 3) || (lon == 1 && dir == 1 && hid == 1 && argc == 4)) {
        openDir(cwd, 5);
    }
        //list -long -rec -dir || list -long -rec
    else if ((lon == 1 && rec == 1 && argc == 3) || (lon == 1 && dir == 1 && rec == 1 && argc == 4)) {
        openDir(cwd, 7);
    }
    else if (dir == 1 && hid == 1 && argc > 3 && lon == 0 && rec == 0){     //list -dir -hid argv[] || list -hid -dir argv[]
        for (int i = 3; i < argc; i++) {
            stat(argv[i], &sb);
            if (strcmp((S_ISDIR(sb.st_mode)) ? "d" : "-", "d") == 0){
                openDir(argv[i], 1);
            }
        }
    }

    else if(dir == 1 && rec == 1 && argc > 3 && lon == 0 && hid == 0){    //list -dir -rec argv[] || list -rec -dir argv[]
        for (int i = 3; i < argc; i++) {
            stat(argv[i], &sb);
            if (strcmp((S_ISDIR(sb.st_mode)) ? "d" : "-", "d") == 0){
                openDir(argv[i], 4);
            }
        }
    }

        //list -dir -rec -hid || list -rec -dir -hid
    else if((dir == 1 && rec == 1 && hid == 1 && argc == 4) || (rec == 1 && hid == 1 && argc == 3)){
        openDir(cwd, 3);
    }
    else if(dir == 1 && argc > 2 && lon == 0 && rec == 0 && hid == 0){  //list -dir argv[]
        for (int i = 2; i < argc; i++) {
            stat(argv[i], &sb);
            if (strcmp((S_ISDIR(sb.st_mode)) ? "d" : "-", "d") == 0){
                openDir(argv[i], 0);
            }
        }
    }
        //list -long -dir -rec -hid
    else if((lon == 1 && rec == 1 && hid == 1 && argc == 4) || (dir == 1 && lon == 1 && rec == 1 && hid == 1 && argc == 5)){
        openDir(cwd, 5);
    }

        //list -dir -hid -rec -long argv[]
    else if((dir == 1 && lon == 1 && rec == 1 && hid == 1 && argc > 5)){
        for (int i = 5; i < argc; i++) {
            stat(argv[i], &sb);
            if (strcmp((S_ISDIR(sb.st_mode)) ? "d" : "-", "d") == 0){
                openDir(argv[i], 5);
            }
        }
    }

        //list -long -dir argv[] || list -dir -long argv[]
    else if (lon == 1 && dir == 1 && argc > 3 && hid == 0 && rec == 0) {
        for (int i = 3; i < argc; i++) {
            stat(argv[i], &sb);
            if (strcmp((S_ISDIR(sb.st_mode)) ? "d" : "-", "d") == 0){
                strcpy(apoyo, argv[i]);
                openDir(apoyo, 2);
            }
        }
    }
    else if (lon == 1 && argc > 2 && dir == 0 && hid == 0 && rec == 0) {           //list -long argv[]
        for (int i = 2; i < argc; i++) {
            cmdListLong(argv[i], 0, opendir("."), argv[i]);
        }
    }


        //list -dir -rec -hid argv[] || list -rec -dir -hid argv[]
    else if(hid == 1 && rec == 1 && dir == 1 && argc > 4 && lon == 0){
        for (int i = 4; i < argc; i++) {
            stat(argv[i], &sb);
            if (strcmp((S_ISDIR(sb.st_mode)) ? "d" : "-", "d") == 0){
                openDir(argv[i], 3);
            }
        }
    }
    else printf("Comando no reconocido\n");
}
void cmdDelete(int argc, char *argv[], List *listH, List *listA, Node *listE){
    char cwd[MAXLINEA];
    getcwd(cwd, sizeof(cwd));
    struct dirent *aux;
    struct stat sb;
    DIR* d;
    char aux1[MAXLINEA];
    char aux2[MAXLINEA];
    char* argumentos[MAXLINEA];
    int auxiliar = 0;

    if(argc == 1){
        opendir(cwd);
    }else{
        if(strcmp(argv[1], "-rec") == 0 && argc == 2){
            opendir(cwd);
        }else{

            if(strcmp(argv[1], "-rec") == 0 && argc > 2){
                for(auxiliar = 2; auxiliar < argc; auxiliar++){
                    if(stat(argv[auxiliar], &sb) == 0){
                        if(remove(argv[auxiliar]) != 0){
                            d = opendir(argv[auxiliar]);
                            while((aux = readdir(d)) != NULL){

                                if(strcmp(aux->d_name, ".") != 0 && strcmp(aux->d_name, "..") != 0){

                                    strcpy(aux1, argv[auxiliar]);
                                    strcat(aux1, "/");
                                    strcat(aux1, aux->d_name);
                                    stat(aux1, &sb);

                                    if(S_ISDIR(sb.st_mode)){
                                        strcpy(aux2, "delete -rec ");
                                        TrocearCadena(strcat(aux2, aux1), argumentos);
                                        cmdDelete(3, argumentos, listH, listA, listE);
                                    }else{
                                        remove(aux1);
                                        printf("%s\n", aux1);
                                    }
                                }
                            }
                            closedir(d);
                            remove(argv[auxiliar]);
                        }
                    }else{
                        puts("Error archivo no existe");    //error archivo no existe
                    }
                }
            }else{
                for(auxiliar = 1; auxiliar < argc; auxiliar++){
                    if(stat(argv[auxiliar], &sb) == 0){
                        remove(argv[auxiliar]);
                    }else{
                        puts("Error archivo no existe");
                    }
                }
            }

        }
    }
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam) /*si tam no es 0 la crea en modo exclusivo */
        flags=flags | IPC_CREAT | IPC_EXCL;
/*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE)
/*no nos vale*/
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
/*si se ha creado y no se puede mapear*/
        if (tam)
/*se borra */
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
/* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
    return (p);
}
void printAllocateList(List *listA, int x) {
    for (pos n = first(listA); !end(listA, n); n = next(listA, n)) {
        ElementAllocate *puntero = element(listA, n);
        if (x == 0) {
            if (strcmp(puntero->command, "malloc") == 0)
                printf("%p: size:%lld. %s %s\n", puntero->direccion, puntero->tam, puntero->command, puntero->time);
            else if (strcmp(puntero->command, "shared memory") == 0)
                printf("%p: size:%lld. %s (key %d) %s\n", puntero->direccion, puntero->tam, puntero->command,
                       puntero->key,
                       puntero->time);
        }
        else if(x == 1){
            if (strcmp(puntero->command, "malloc") == 0)
                printf("%p: size:%lld. %s %s\n", puntero->direccion, puntero->tam, puntero->command,puntero->time);
        }
        else if (x == 2){
            if(strcmp(puntero->command, "shared memory") == 0)
                printf("%p: size:%lld. %s (key %d) %s\n", puntero->direccion, puntero->tam, puntero->command,puntero->key, puntero->time);
        }
    }
}
void Cmd_AlocateCreateShared (char *arg[], List *listaA) /*arg[0] is the key
and arg[1] is the size*/
{
    key_t k;
    size_t tam=0;
    void *p;
    if (arg[0] != NULL)
        k=(key_t) atoi(arg[0]);
    if (arg[0]==NULL || arg[1]==NULL){
        printAllocateList(listaA, 2);
    }
    else if (arg[1]!=NULL) {
        tam = (size_t) atoll(arg[1]);
        if ((p = ObtenerMemoriaShmget(k, tam)) == NULL)
            perror("Imposible obtener memoria shmget");
        else {
            time_t tiempo = time(0);
            struct tm *tlocal = localtime(&tiempo);
            char output[128 + 1];
            strftime(output,128,"%a %b %d %H:%M:%S %Y",tlocal);
            printf("Memoria de shmget de clave %d asignada en %p\n", k, p);
            ElementAllocate *new = malloc(sizeof(ElementAllocate));
            new->direccion = p;
            new->tam = tam;
            new->key = k;
            strcpy(new->command, "shared memory");
            strcpy(new->time, output);
            InsertElement(listaA, new);
        }
    }
}

void Cmd_deletekey (char *args[]) /*arg[0] points to a str containing the key*/
{
    key_t clave;
    int id;
    char *key=args[0];
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("rmkey clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

void cmdMemory(int argc, char *argv[], List *listH, List *listA, Node *listE){
    if (strcmp(argv[1], "-allocate") == 0){
        if(argc == 2){
            printAllocateList(listA, 0);
        }
        else if (argc == 4 && strcmp(argv[2], "-malloc") == 0){
            long long tam = atoi(argv[3]);
            unsigned long long *puntero =  malloc(tam);
            if(puntero == NULL) {
                perror("Malloc failed");
                exit(1);
            }
            else {
                time_t tiempo = time(0);
                struct tm *tlocal = localtime(&tiempo);
                char output[128 + 1];
                strftime(output,128,"%a %b %d %H:%M:%S %Y",tlocal);
                printf("allocated %lld at %p\n", tam, puntero);
                ElementAllocate *new = malloc(sizeof(ElementAllocate));
                new->direccion = puntero;
                new->tam = tam;
                strcpy(new->command, "malloc");
                strcpy(new->time, output);
                InsertElement(listA, new);
            }
        }
        else if (strcmp(argv[2], "-malloc") == 0 && argc == 3){
            printAllocateList(listA, 1);
        }
        else if ((strcmp(argv[2], "-createshared") == 0)){
            char *par[2];
            par[0] = argv[3];
            par[1] = argv[4];
            Cmd_AlocateCreateShared(par, listA);
        }
    }
    else if (strcmp(argv[1], "-dealloc") == 0){
        if (argc == 2){
            printAllocateList(listA, 0);
        }
        else if (strcmp(argv[2], "-malloc") == 0){
            if(argc == 3) {
                printAllocateList(listA, 1);
            }
            else if(argc == 4){
                int tam = atoi(argv[3]);
                for (pos n = first(listA); !end(listA, n); n = next(listA, n)) {
                    ElementAllocate *puntero = element(listA, n);
                    if (puntero->tam == tam && strcmp(puntero->command,"malloc") == 0) {
                        printf("block at address %p deallocated (%s) \n", puntero->direccion, puntero->command);
                        deleteElementFree(listA, n);
                        break;
                    }
                }
            }
        }
        else if (strcmp(argv[2], "-shared") == 0){
            if(argc == 3)
                printAllocateList(listA, 2);
            else if(argc == 4) {
                int key = atoi(argv[3]);
                for (pos n = first(listA); !end(listA, n); n = next(listA, n)) {
                    ElementAllocate *puntero = element(listA, n);
                    if (puntero->key == key && strcmp(puntero->command, "shared memory") == 0) {
                        if (shmdt(&puntero->direccion)) {
                            printf("block at address %p deallocated (shared) \n", puntero->direccion);
                            //Falta el deattached
                            deleteElementFree(listA, n);
                            break;
                        }
                    }
                }
            }
        }
    }
    else if (strcmp(argv[1], "-deletekey") == 0){
        if (argc == 3){
            int key = atoi(argv[2]);
            for (pos n = first(listA); !end(listA, n); n = next(listA, n)) {
                ElementAllocate *puntero = element(listA, n);
                if (puntero != NULL && puntero->key == key) {
                    char *clave = argv[2];
                    Cmd_deletekey(&clave);
                    printf("key %d deleted\n", key);
                    deleteElement(listA, n);
                    break;
                }
            }
        }
    }
    else if (strcmp(argv[1], "-show") == 0){
        if (argc == 2){
            int x, y, z;
            printf("local: x -> %p y -> %p z -> %p\n"
                   "global: xG -> %p yG -> %p zG -> %p\n"
                   "program functions: cmdMemory -> %p cmdExit -> %p cmdDelete -> %p\n",
                   &x, &y, &z, &xG, &yG, &zG, cmdMemory, cmdExit, cmdDelete);
        }
        else if (argc == 3 && strcmp(argv[2], "-malloc") == 0)
            printAllocateList(listA, 1);
        else if (argc == 3 && strcmp(argv[2], "-shared") == 0)
            printAllocateList(listA, 2);
        else if (argc == 3 && strcmp(argv[2], "-all") == 0)
            printAllocateList(listA, 0);
        else if (argc == 3 && strcmp(argv[2], "-vars") == 0){
            int x, y, z;
            printf("local: x -> %p y -> %p z -> %p\n"
                   "global: xG -> %p yG -> %p zG -> %p\n", &x, &y, &z, &xG, &yG, &zG);
        }
        else if (argc == 3 && strcmp(argv[2], "-funcs") == 0){
            printf("program functions: cmdMemory -> %p cmdExit -> %p cmdDelete -> %p\n"
                   "C library functions : printf -> %p puts -> %p strcpy -> %p\n",
                   cmdMemory, cmdExit, cmdDelete, printf, puts, strcpy);
        }
    }
}
/*void cmdMemdump(int argc, char *argv[], List *listH, List *listA){
	char *p = (char*) strtol(argv[1], NULL, 16);
	int cont = 25;
	for(int i = 0; i < cont; i++)
		printf("%
}*/

void cmdMemfill(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    int cont = 128;
    char byte = 65;
    char *addr = (char *) strtoul(argv[1], NULL, 16);
    if (argc >= 2) {
        if (argc == 4) {
            cont = atoi(argv[2]);
            byte = (char) strtoul(argv[3], NULL, 16);
        }
        else if (argc == 3 && strncmp(argv[2], "0x", 2) == 0)
            byte = (char) strtoul(argv[3], NULL, 16);
        else if(argc == 3)
            cont = atoi(argv[2]);
        for (int x = 0; x < cont; x++)
            addr[x] = byte;
        printf("%p memfilled with %d bytes of value %x\n", addr, cont, byte);
    }
}

ssize_t LeerFichero (char *fich, void *p, ssize_t n) {
/*n=-1 indica que se lea todo*/
    ssize_t nleidos, tam = n;
    int df, aux;
    struct stat s;
    if (stat(fich, &s) == -1 || (df = open(fich, O_RDONLY)) == -1)
        return ((ssize_t) -1);
    if (n == LEERCOMPLETO)
        tam = (ssize_t) s.st_size;
    if ((nleidos = read(df, p, tam)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return ((ssize_t) -1);
    }
    close(df);
    return (nleidos);
}

void cmdReadfile(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    ssize_t n;
    char *addr = (char *) strtoul(argv[2], NULL, 16);
    char *fich = (char *) strtoul(argv[1], NULL, 16);
    if (argc == 3)
        n = (ssize_t) NULL;
    else n = atol(argv[3]);
    n = LeerFichero(fich, addr, n);
    if (n == -1)
        printf("reading file %s into memory address %s was not possible", argv[1], argv[2]);
    printf("%ld bytes read from file %s into memory address %s\n", n, argv[1], argv[2]);

}

void cmdGetPriority(int argc, char *argv[], List *listH, List *listA, Node *listE){
    pid_t p_process_id;

    if(argc > 1){
        p_process_id = atoi(argv[1]);
        printf("Prioridad del proceso %d es %d\n", p_process_id, getpriority(PRIO_PROCESS, p_process_id));

    }else{
        p_process_id = getpid();
        printf("Prioridad del proceso %d es %d\n", p_process_id, getpriority(PRIO_PROCESS, p_process_id));
    }
}

void cmdSetPriority(int argc, char *argv[], List *listH, List *listA, Node *listE){
    if(argc == 1){
        cmdGetPriority(argc, argv, listH, listA, listE);
    }
    else if(argc == 2){
        setpriority(PRIO_PROCESS, getpid(), atoi(argv[1]));
        printf("Priority has changed to %s\n", argv[1]);
    }
    else if(argc == 3){
        setpriority(PRIO_PROCESS, atoi(argv[1]), atoi(argv[2]));
        printf("%s priority has changed to %s\n", argv[1], argv[2]);
    }
}

char * NombreUsuario (uid_t uid){
    struct passwd *p;if ((p=getpwuid(uid))==NULL)
        return (" ??????");
    return p->pw_name;
}

uid_t UidUsuario (char * nombre){
    struct passwd *p;
    if ((p=getpwnam (nombre))==NULL)
        return (uid_t) -1;
    return p->pw_uid;
}

void cmdGetUid(int argc, char *argv[], List *listH, List *listA, Node *listE){
        uid_t real = getuid(), efec = geteuid();
        printf("Credencial real: %d, (%s)\n", real, NombreUsuario(real));
        printf("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario(efec));
}

void cmdSetUid(int argc, char *argv[], List *listH, List *listA, Node *listE){
        uid_t uid;
        int u;
        if (argv[0]==NULL || (!strcmp(argv[0],"-l") && argv[1]==NULL)){
            cmdGetUid(argc, argv, listH, listA, listE);
            return;
        }
        if (!strcmp(argv[0],"-l")){
            if ((uid=UidUsuario(argv[1]))==(uid_t) -1){
                printf ("Usuario no existente %s\n", argv[1]);
                return;
            }
        }
        else if ((uid=(uid_t) ((u=atoi (argv[0]))<0)? -1: u) ==(uid_t) -1){
            printf ("Valor no valido de la credencial %s\n",argv[0]);
            return;
        }
        if (setuid (uid)==-1)
            printf ("Imposible cambiar credencial: %s\n", strerror(errno));
}

void cmdFork(int argc, char *argv[], List *listH, List *listA, Node *listE){
    pid_t c, procces;

    int status;

    if(argc > 1){
        c = fork();
        if(c == -1){
            perror("failure");
            exit(EXIT_FAILURE);
        }
        if(c == 0){
            printf("Ejecutando proceso %d\n", getpid());

        }else {                    /* Es el codigo del padre */
            do {
                procces = waitpid(c, &status, WUNTRACED | WCONTINUED);
                if (procces == -1) { perror("waitpid"); exit(EXIT_FAILURE); }

                if (WIFEXITED(status)) {
                    printf("exited, status=%d\n", WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    printf("killed by signal %d\n", WTERMSIG(status));
                } else if (WIFSTOPPED(status)) {
                    printf("stopped by signal %d\n", WSTOPSIG(status));
                } else if (WIFCONTINUED(status)) {
                    printf("continued\n");
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }else{
        cmdGetUid(argc, argv, listH, listA, listE);
    }
}

void cmdExecute(int argc, char *argv[], List *listH, List *listA, Node *listE){
    int pri = 0, i;
    for(i = 0; i < argc; i++) if(strncmp(argv[i], "@", 1) == 0) pri = 1;
    if(pri) {
        setpriority(PRIO_PROCESS, atoi(argv[1]), atoi(argv[argc - 1]));
        if(execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
        else(printf("Executing the program %s after changing"
                    " the priority of the process to %s\n", argv[1], argv[argc  - 1]));
    }
    else{
        if(execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
        else(printf("Executing the program %s\n", argv[1]));
    }
}

void cmdBackground(int argc, char *argv[], List *listH, List *listA, Node *listE){
    pid_t child = fork();
    char *argv3[argc - 2];
    for(int i = 1; i < (argc - 1); i++) argv3[i - 1] = argv[i];
    if(child == -1){
        perror("failure");
        exit(EXIT_FAILURE);
    }
    int i, pri = 0;
    for(i = 0; i < argc; i++) if(strncmp(argv[i], "@", 1) == 0) pri = 1;
    if(pri) {
        if(child == 0){
            setpriority(PRIO_PROCESS, getpid(), atoi(argv[argc - 1]));
            if(execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
            else(printf("Executing the program %s in background after changing"
                        " the priority of the process to %s\n", argv[1], argv[argc  - 1]));
            Process *process = malloc(sizeof(Process));
            process->pid = getpid();
            strcpy(process->command, *argv3);
            int status;
            pid_t pid = waitpid(child, &status, WNOHANG |WUNTRACED |WCONTINUED);
            if(pid == -1) perror("Error en el waitpid");
            else if(pid == 0) strcpy(process->state, "RUNNING");
            else if(WIFSTOPPED(status)) {
                strcpy(process->state, "STOPPED");
                process->value = SIGSTOP;
            }
            else if(WIFEXITED(status)){
                strcpy(process->state, "TERMINATED NORMALLY");
                process->value = getpid();
            }
            else if(WIFSIGNALED(status)) {
                strcpy(process->state, "TERMINATED BY SIGNAL");
                process->value = SIGKILL;
            }
            process->priority = getpriority(PRIO_PROCESS, getpid());
            time_t tiempo = time(0);
            struct tm *tlocal = localtime(&tiempo);
            char output[128];
            strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
            strcpy(process->start, output);
            InsertElementE(listE, process);
        }
    }
    else{
        if(child == 0){
            if(execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
            else(printf("Executing the program %d in background\n", getpid()));
            Process *process = malloc(sizeof(Process));
            process->pid = getpid();
            strcpy(process->command, *argv3);
            int status;
            pid_t pid = waitpid(child, &status, WNOHANG |WUNTRACED |WCONTINUED);
            if(pid == -1) perror("Error en el waitpid");
            else if(pid == 0) strcpy(process->state, "RUNNING");
            else if(WIFSTOPPED(status)) {
                strcpy(process->state, "STOPPED");
                process->value = SIGSTOP;
            }
            else if(WIFEXITED(status)){
                strcpy(process->state, "TERMINATED NORMALLY");
                process->value = getpid();
            }
            else if(WIFSIGNALED(status)) {
                strcpy(process->state, "TERMINATED BY SIGNAL");
                process->value = SIGKILL;
            }
            process->priority = getpriority(PRIO_PROCESS, getpid());
            time_t tiempo = time(0);
            struct tm *tlocal = localtime(&tiempo);
            char output[128];
            strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
            strcpy(process->start, output);
            InsertElementE(listE, process);
        }
    }
}

void cmdForeground(int argc, char *argv[], List *listH, List *listA, Node *listE){
    pid_t child = fork();
    
    if(child == -1){
        perror("failure");
        exit(EXIT_FAILURE);
    }
    int i, pri = 0;
    for(i = 0; i < argc; i++) if(strncmp(argv[i], "@", 1) == 0) pri = 1;
    if(pri) {
        if(child == 0){ 
            if(setpgid(0, 0) < 0){
                perror("setpgid");
            }
            
            char *argv2[argc - 3];
            for(i = 2; i < (argc - 1); i++) argv2[i - 2] = argv[i];
            setpriority(PRIO_PROCESS, getpid(), atoi(argv[argc - 1]));
            if(execvp(argv[1], argv2) == -1) perror("Fallo en la ejecución\n");
            else(printf("Executing the program %s in background after changing"
                        " the priority of the process to %s\n", argv[1], argv[argc  - 1]));
            Process *process = malloc(sizeof(Process));
            process->pid = getpid();
            strcpy(process->command, *argv);
            int status;
            pid_t pid = waitpid(child, &status, WNOHANG);
            if(pid == -1) perror("Error en el waitpid");
            else if(pid == 0) strcpy(process->state, "RUNNING");
            else if(WIFSTOPPED(status)) {
                strcpy(process->state, "STOPPED");
                process->value = SIGSTOP;
            }
            else if(WIFEXITED(status)){
                strcpy(process->state, "TERMINATED NORMALLY");
                process->value = getpid();
            }
            else if(WIFSIGNALED(status)) {
                strcpy(process->state, "TERMINATED BY SIGNAL");
                process->value = SIGKILL;
            }
            process->priority = getpriority(PRIO_PROCESS, getpid());
            time_t tiempo = time(0);
            struct tm *tlocal = localtime(&tiempo);
            char output[128];
            strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
            strcpy(process->start, output);
            InsertElementE(listE, process);
            kill(-child, SIGTERM);
        }
        else {
            int status;
            if(waitpid(child, &status, 0) < 0) perror("Error en el waitpid");
        }
    }
    else{
        if(child == 0){
            if(setpgid(0, 0) < 0){
                perror("setpgid");
            }
            
            char *argv2[argc - 2];
            for(i = 2; i < argc; i++) argv2[i - 2] = argv[i];
            if(execvp(argv[1], argv2) == -1) perror("Fallo en la ejecución\n");
            else(printf("Executing the program %d in background\n", getpid()));
            Process *process = malloc(sizeof(Process));
            process->pid = getpid();
            strcpy(process->command, *argv);
            int status;
            pid_t pid = waitpid(child, &status, 0);
            if(pid == -1) perror("Error en el waitpid");
            else if(pid == 0) strcpy(process->state, "RUNNING");
            else if(WIFSTOPPED(status)) {
                strcpy(process->state, "STOPPED");
                process->value = SIGSTOP;
            }
            else if(WIFEXITED(status)){
                strcpy(process->state, "TERMINATED NORMALLY");
                process->value = getpid();
            }
            else if(WIFSIGNALED(status)) {
                strcpy(process->state, "TERMINATED BY SIGNAL");
                process->value = SIGKILL;
            }
            process->priority = getpriority(PRIO_PROCESS, getpid());
            time_t tiempo = time(0);
            struct tm *tlocal = localtime(&tiempo);
            char output[128];
            strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
            strcpy(process->start, output);
            InsertElementE(listE, process);
            kill(-child, SIGTERM);
        }
        else {
            int status;
            if(waitpid(child, &status, 0) < 0) perror("Error en el waitpid");
        }
    }
    
}

void cmdRunas(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    pid_t child = fork();
    setuid(atoi(argv[1]));
    if(strcmp(argv[argc - 1], "&") == 0) {
        if (child == -1) {
            perror("failure");
            exit(EXIT_FAILURE);
        }
        int i, pri = 0;
        for (i = 0; i < argc; i++) if (strncmp(argv[i], "@", 1) == 0) pri = 1;
        if (pri) {
            if (child == 0) {
                setpriority(PRIO_PROCESS, getpid(), atoi(argv[argc - 1]));
                if (execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
                else
                    (printf("Executing the program %s in background after changing"
                            " the priority of the process to %s\n", argv[1], argv[argc - 1]));
            }
        }
        else {
            if (child == 0) {
                if (execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
                else(printf("Executing the program %d in background\n", getpid()));
            }
        }
    }
    else{
        if (child == -1) {
            perror("failure");
            exit(EXIT_FAILURE);
        }
        int i, pri = 0;
        for (i = 0; i < argc; i++) if (strncmp(argv[i], "@", 1) == 0) pri = 1;
        if (pri) {
            if (child == 0) {
                setpriority(PRIO_PROCESS, getpid(), atoi(argv[argc - 1]));
                if (execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
                else
                    (printf("Executing the program %s in background after changing"
                            " the priority of the process to %s\n", argv[1], argv[argc - 1]));
            }
            else waitpid(child, NULL, 0);
        }
        else {
            if (child == 0) {
                if (execvp(argv[1], argv) == -1) perror("Fallo en la ejecución\n");
                else(printf("Executing the program %d in background\n", getpid()));
            }
            else waitpid(child, NULL, 0);
        }
    }
}

void cmdExecute_as(int argc, char *argv[], List *listH, List *listA, Node *listE){
    int pri = 0, i;
    uid_t uid = atoi(argv[1]);

    for(i = 0; i < argc; i++) if(strncmp(argv[i], "@", 1) == 0) pri = 1;
    if(pri) {
        char *argv2[argc - 3];
        for(i = 2; i < (argc - 1); i++) argv2[i - 2] = argv[i];
        setpriority(PRIO_PROCESS, atoi(argv[1]), atoi(argv[argc - 1]));
        if(execvp(argv[1], argv2) == -1) perror("Fallo en la ejecución\n");
        else(printf("Executing the program %s after changing"
                    " the priority of the process to %s\n", argv[1], argv[argc  - 1]));
    }
    else{
        char *argv2[argc - 2];
        for(i = 2; i < argc; i++) argv2[i - 2] = argv[i];
        setuid(uid);
        if(execvp(argv[1], argv2) == -1) perror("Fallo en la ejecución\n");
        else(printf("Executing the program %s\n", argv[1]));
        }
}


void cmdListprocs(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    Node *node = listE;
    MostrarLista(*listE);
    while(node->next != NULL){
        node = node->next;
        if (strcmp(node->process->state,"RUNNING") == 0){
            printf("pid: %d priority: %d command: %s time: %s process state: %s\n", node->process->pid, node->process->priority, node->process->command, node->process->start, node->process->state);
        }
        else{
            printf("pid: %d priority: %d command: %s time: %s process state: %s %d\n", node->process->pid, node->process->priority, node->process->command, node->process->start, node->process->state, node->process->value);
        }
    }
}

void cmdProc(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    int id = 0;
    Node *node = listE;
    while(node->next != NULL){
        node = node->next;
        if(node->process->pid == atoi(argv[argc - 1])) id = 1;
    }
    if(argc == 1 || id == 0) cmdListprocs(argc, argv, listH, listA, listE);
    Node node2 = *listE;
    if(argc == 2) {
        while (node2.next != NULL) {
            node2 = *node2.next;
            if (node2.process->pid == atoi(argv[argc - 1])) {
                if (strcmp(node->process->state,"RUNNING") == 0){
                    printf("pid: %d priority: %d command: %s time: %s process state: %s\n", node->process->pid, node->process->priority, node->process->command, node->process->start, node->process->state);
                }
                else{
                    printf("pid: %d priority: %d command: %s time: %s process state: %s %d\n", node->process->pid, node->process->priority, node->process->command, node->process->start, node->process->state, node->process->value);
                }
            }
        }
    }
    if(argc == 3) {
        while (node2.next != NULL) {
            node2 = *node2.next;
            if (node2.process->pid == atoi(argv[argc - 1])) {
                int status;
                pid_t pid = waitpid(getpid(), &status, WNOHANG | WUNTRACED | WCONTINUED);
                if (pid == -1) perror("Error en el waitpid");
                else if (WIFEXITED(status)) {
                    printf("Terminated normally");
                } else if (WIFSIGNALED(status)) {
                    printf("Terminated by signal");
                }
                if (BorrarPID(listE, getpid()) == -1) perror("PID no encontrado\n");
            }
        }
    }
}

void cmdDeleteprocs(int argc, char *argv[], List *listH, List *listA, Node *listE) {
    if (strcmp(argv[1], "-term") == 0) {
        Node *node = listE;
        while (node->next != NULL) {
            node = node->next;
            BorrarState(listE, "TERMINATED NORMALLY");
        }
    } else if (strcmp(argv[1], "-sig") == 0) {
        Node *node = listE;
        while (node->next != NULL) {
            node = node->next;
            BorrarState(listE, "TERMINATED BY SIGNAL");
        }
    }
}

void cmdExit(int argc, char *argv[], List *listH, List *listA, Node *listE){
    exit(0);
}

comando tablaComandos[] = {
        {"authors", cmdAutores},
        {"exit", cmdExit},
        {"quit", cmdExit},
        {"end", cmdExit},
        {"getpid", cmdGetPid},
        {"getppid", cmdGetPPid},
        {"pwd", cmdPwd},
        {"chdir", cmdChdir},
        {"date", cmdFecha},
        {"time", cmdHora},
        {"historic", cmdHistoric},
        {"create", cmdCreateDir},
        {"list", cmdList},
        {"delete", cmdDelete},
        {"memory", cmdMemory},
        {"memfill", cmdMemfill},
        //{"memdump", cmdMemdump},
        {"readfile", cmdReadfile},
        {"getpriority", cmdGetPriority},
        {"setpriority", cmdSetPriority},
        {"setuid", cmdSetUid},
        {"getuid", cmdGetUid},
        {"fork", cmdFork},
        {"execute", cmdExecute},
        {"background", cmdBackground},
        {"listprocs", cmdListprocs},
        {"proc", cmdProc},
        {"run-as", cmdRunas},
        {"deleteprocs", cmdDeleteprocs},
        {"execute-as", cmdExecute_as},
        {"foreground", cmdForeground},
        {NULL, NULL}
};


int main(){
    int ntrozos; char *trozos[200];
    List listaH, listaA;
    Node listaE;
    CreateList(&listaH);
    CreateList(&listaA);
    CreateListE(&listaE);
    while(1){   //bucle infinito
        printf("->");
        if(fgets(linea, MAXLINEA, stdin)==NULL) exit(0);
        Element *new = malloc(sizeof(Element));
        strcpy(new->command, linea);
        InsertElement(&listaH, new);
        ntrozos=TrocearCadena(linea, trozos);
        procesoComandos(ntrozos, trozos, listaH, &listaA, &listaE);
    }
}

//gcc -g ...
