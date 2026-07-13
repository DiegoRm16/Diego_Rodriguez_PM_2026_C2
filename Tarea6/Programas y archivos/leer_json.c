/*
 * leer_json.c
 * ---------------------------------------------------------
 * Lee el archivo alumnos.json, obtiene primero la cantidad de
 * elementos (campo "cantidad"), reserva un arreglo dinamico
 * (malloc) del tamaño exacto y luego carga y muestra los datos
 * de cada Alumno.
 *
 * Formato esperado del JSON:
 * {
 *     "cantidad": 10,
 *     "alumnos": [
 *         { "nombre": "...", "apellido": "...", "promedio": 8.5, "materia": "..." },
 *         ...
 *     ]
 * }
 *
 * Nota: se realiza un parser simple hecho a mano (sin librerias
 * externas como cJSON) ya que se conoce el formato exacto del
 * archivo generado. No es un parser JSON general.
 *
 * Compilar:  gcc -Wall -o leer_json leer_json.c
 * Ejecutar:  ./leer_json alumnos.json
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

/* Busca "clave": valor  en una linea de texto tipo JSON y devuelve 1 si lo encontro */
static int extraerValorString(const char *linea, const char *clave, char *destino) {
    char patron[MAX_CAMPO];
    snprintf(patron, sizeof(patron), "\"%s\"", clave);

    char *pos = strstr(linea, patron);
    if (!pos) return 0;

    pos = strchr(pos + strlen(patron), ':');
    if (!pos) return 0;
    pos++; /* saltamos el ':' */

    /* saltamos espacios */
    while (*pos == ' ') pos++;

    if (*pos == '"') {
        pos++; /* saltamos comilla de apertura */
        char *fin = strchr(pos, '"');
        if (!fin) return 0;
        size_t largo = fin - pos;
        strncpy(destino, pos, largo);
        destino[largo] = '\0';
        return 1;
    }
    return 0;
}

static int extraerValorNumerico(const char *linea, const char *clave, float *destino) {
    char patron[MAX_CAMPO];
    snprintf(patron, sizeof(patron), "\"%s\"", clave);

    char *pos = strstr(linea, patron);
    if (!pos) return 0;

    pos = strchr(pos + strlen(patron), ':');
    if (!pos) return 0;
    pos++;

    *destino = (float) atof(pos);
    return 1;
}

/* Primera pasada: busca el valor de "cantidad" */
int leerCantidad(FILE *archivo) {
    char linea[MAX_LINEA];
    int cantidad = -1;
    float valor;

    while (fgets(linea, sizeof(linea), archivo)) {
        if (strstr(linea, "\"cantidad\"")) {
            if (extraerValorNumerico(linea, "cantidad", &valor)) {
                cantidad = (int) valor;
                break;
            }
        }
    }
    rewind(archivo);
    return cantidad;
}

/* Segunda pasada: carga los N alumnos en el arreglo ya reservado.
   Cada alumno esta en una sola linea del archivo generado. */
void cargarAlumnos(FILE *archivo, Alumno *alumnos, int n) {
    char linea[MAX_LINEA];
    int indice = 0;

    while (fgets(linea, sizeof(linea), archivo) && indice < n) {
        /* Solo procesamos lineas que representen un objeto alumno */
        if (strstr(linea, "\"nombre\"") && strstr(linea, "\"materia\"")) {
            extraerValorString(linea, "nombre", alumnos[indice].nombre);
            extraerValorString(linea, "apellido", alumnos[indice].apellido);
            extraerValorNumerico(linea, "promedio", &alumnos[indice].promedio);
            extraerValorString(linea, "materia", alumnos[indice].materia);
            indice++;
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
    const char *nombreArchivo = (argc > 1) ? argv[1] : "alumnos.json";

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
    printf("Cantidad de alumnos detectada en el JSON: %d\n", n);

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
