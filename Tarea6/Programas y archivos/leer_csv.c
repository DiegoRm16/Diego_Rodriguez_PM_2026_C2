/*
 * leer_csv.c
 * ---------------------------------------------------------
 * Lee el archivo alumnos.csv, obtiene primero la cantidad de
 * elementos (primera linea del archivo), reserva un arreglo
 * dinamico (malloc) del tamaño exacto y luego carga y muestra
 * los datos de cada Alumno.
 *
 * Formato esperado del CSV:
 * 10
 * Nombre,Apellido,Promedio,Materia
 * Juan,Perez,8.50,Matematica
 * ...
 *
 * Compilar:  gcc -Wall -o leer_csv leer_csv.c
 * Ejecutar:  ./leer_csv alumnos.csv
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

/* Primera linea del archivo = cantidad de alumnos */
int leerCantidad(FILE *archivo) {
    char linea[MAX_LINEA];
    if (!fgets(linea, sizeof(linea), archivo)) return -1;
    return atoi(linea);
}

/* Quita el salto de linea final si existe */
static void limpiarSaltoLinea(char *cadena) {
    size_t largo = strlen(cadena);
    if (largo > 0 && (cadena[largo - 1] == '\n' || cadena[largo - 1] == '\r'))
        cadena[largo - 1] = '\0';
    if (largo > 1 && cadena[largo - 2] == '\r')
        cadena[largo - 2] = '\0';
}

/* Carga los N alumnos en el arreglo ya reservado, separando por comas */
void cargarAlumnos(FILE *archivo, Alumno *alumnos, int n) {
    char linea[MAX_LINEA];

    /* Saltamos la linea de encabezado: Nombre,Apellido,Promedio,Materia */
    fgets(linea, sizeof(linea), archivo);

    for (int i = 0; i < n; i++) {
        if (!fgets(linea, sizeof(linea), archivo)) break;
        limpiarSaltoLinea(linea);

        char *token = strtok(linea, ",");
        if (token) strcpy(alumnos[i].nombre, token);

        token = strtok(NULL, ",");
        if (token) strcpy(alumnos[i].apellido, token);

        token = strtok(NULL, ",");
        if (token) alumnos[i].promedio = (float) atof(token);

        token = strtok(NULL, ",");
        if (token) strcpy(alumnos[i].materia, token);
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
    const char *nombreArchivo = (argc > 1) ? argv[1] : "alumnos.csv";

    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        fprintf(stderr, "Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return EXIT_FAILURE;
    }

    /* 1) Primero se determina la cantidad de elementos (primera linea) */
    int n = leerCantidad(archivo);
    if (n <= 0) {
        fprintf(stderr, "Error: no se pudo determinar la cantidad de alumnos\n");
        fclose(archivo);
        return EXIT_FAILURE;
    }
    printf("Cantidad de alumnos detectada en el CSV: %d\n", n);

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
