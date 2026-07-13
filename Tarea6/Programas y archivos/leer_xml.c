/*
 * leer_xml.c
 * ---------------------------------------------------------
 * Lee el archivo alumnos.xml, obtiene primero la cantidad de
 * elementos (atributo "cantidad" de la etiqueta <Alumnos>),
 * reserva un arreglo dinamico (malloc) del tamaño exacto y
 * luego carga y muestra los datos de cada Alumno.
 *
 * Formato esperado del XML:
 * <Alumnos cantidad="10">
 *     <Alumno>
 *         <Nombre>...</Nombre>
 *         <Apellido>...</Apellido>
 *         <Promedio>...</Promedio>
 *         <Materia>...</Materia>
 *     </Alumno>
 *     ...
 * </Alumnos>
 *
 * Compilar:  gcc -Wall -o leer_xml leer_xml.c
 * Ejecutar:  ./leer_xml alumnos.xml
 * ---------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINEA 256
#define MAX_CAMPO 100

typedef struct {
    char nombre[MAX_CAMPO];
    char apellido[MAX_CAMPO];
    float promedio;
    char materia[MAX_CAMPO];
} Alumno;

/* Extrae el contenido de texto que se encuentra entre <tag> y </tag> */
static int extraerValor(const char *linea, const char *tag, char *destino) {
    char apertura[MAX_CAMPO], cierre[MAX_CAMPO];
    snprintf(apertura, sizeof(apertura), "<%s>", tag);
    snprintf(cierre, sizeof(cierre), "</%s>", tag);

    char *inicio = strstr(linea, apertura);
    if (!inicio) return 0;
    inicio += strlen(apertura);

    char *fin = strstr(inicio, cierre);
    if (!fin) return 0;

    size_t largo = fin - inicio;
    strncpy(destino, inicio, largo);
    destino[largo] = '\0';
    return 1;
}

/* Primera pasada: busca cantidad="N" en la etiqueta <Alumnos ...> */
int leerCantidad(FILE *archivo) {
    char linea[MAX_LINEA];
    int cantidad = -1;

    while (fgets(linea, sizeof(linea), archivo)) {
        char *pos = strstr(linea, "cantidad=\"");
        if (pos) {
            pos += strlen("cantidad=\"");
            cantidad = atoi(pos);
            break;
        }
    }
    rewind(archivo); /* volvemos al inicio para la segunda pasada */
    return cantidad;
}

/* Segunda pasada: carga los N alumnos en el arreglo ya reservado */
void cargarAlumnos(FILE *archivo, Alumno *alumnos, int n) {
    char linea[MAX_LINEA];
    char campo[MAX_CAMPO];
    int indice = 0;

    while (fgets(linea, sizeof(linea), archivo) && indice < n) {
        if (extraerValor(linea, "Nombre", campo))
            strcpy(alumnos[indice].nombre, campo);
        else if (extraerValor(linea, "Apellido", campo))
            strcpy(alumnos[indice].apellido, campo);
        else if (extraerValor(linea, "Promedio", campo))
            alumnos[indice].promedio = (float) atof(campo);
        else if (extraerValor(linea, "Materia", campo)) {
            strcpy(alumnos[indice].materia, campo);
            indice++; /* Materia es el ultimo campo de cada Alumno */
        }
    }
}

void imprimirAlumnos(Alumno *alumnos, int n) {
    printf("\n%-12s %-12s %-10s %-20s\n", "Nombre", "Apellido", "Promedio", "Materia");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-12s %-12s %-10.2f %-20s\n",
               alumnos[i].nombre, alumnos[i].apellido,
               alumnos[i].promedio, alumnos[i].materia);
    }
}

int main(int argc, char *argv[]) {
    const char *nombreArchivo = (argc > 1) ? argv[1] : "alumnos.xml";

    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        fprintf(stderr, "Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return EXIT_FAILURE;
    }

    /* 1) Primero se determina la cantidad de elementos */
    int n = leerCantidad(archivo);
    if (n <= 0) {
        fprintf(stderr, "Error: no se pudo determinar la cantidad de alumnos\n");
        fclose(archivo);
        return EXIT_FAILURE;
    }
    printf("Cantidad de alumnos detectada en el XML: %d\n", n);

    /* 2) Recien ahora se reserva la memoria dinamica del tamaño exacto */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (!alumnos) {
        fprintf(stderr, "Error: no se pudo reservar memoria\n");
        fclose(archivo);
        return EXIT_FAILURE;
    }

    /* 3) Se cargan los datos en el arreglo dinamico */
    cargarAlumnos(archivo, alumnos, n);
    fclose(archivo);

    /* 4) Se imprimen los datos */
    imprimirAlumnos(alumnos, n);

    free(alumnos);
    return EXIT_SUCCESS;
}
