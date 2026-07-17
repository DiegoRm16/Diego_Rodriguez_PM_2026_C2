/*
 * convertir_imagen.c
 * ---------------------------------------------------------
 * Convierte imagenes PNG, JPG/JPEG y BMP a blanco y negro (escala
 * de grises).
 *
 *   1) Lista los archivos de imagen que hay en una carpeta.
 *   2) Muestra un menu para elegir cual convertir.
 *   3) Guarda el resultado como "bn_<nombre_original>" en la
 *      misma carpeta, manteniendo el formato original.
 *
 * DEPENDENCIA (obligatoria, no son C++ ni Python):
 *   Este programa usa "stb_image.h" y "stb_image_write.h", dos
 *   librerias de un solo archivo, escritas en C puro y de dominio
 *   publico, que son el estandar de facto para leer/escribir
 *   PNG/JPG/BMP en C sin depender de librerias del sistema
 *   (libpng, libjpeg, etc). Hacer un decodificador JPEG/PNG propio
 *   desde cero (Huffman + DCT para JPG, Inflate/zlib para PNG)
 *   escapa el alcance de un programa de este tipo, por eso se usan
 *   estos headers, tal como se usarian stdio.h o string.h.
 *
 *   Antes de compilar, descargar estos dos archivos y ponerlos en
 *   la misma carpeta que convertir_imagen.c:
 *     https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
 *     https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
 *
 * Compilar:  gcc -Wall -o convertir_imagen convertir_imagen.c -lm
 * Ejecutar:  ./convertir_imagen [carpeta]      (por defecto: carpeta actual)
 * ---------------------------------------------------------
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_ARCHIVOS 200
#define MAX_NOMBRE   256

/* Escribe en 'destino' la extension (en minusculas, sin el punto) de 'nombre' */
void obtenerExtension(const char *nombre, char *destino) {
    const char *punto = strrchr(nombre, '.');
    int i = 0;
    destino[0] = '\0';
    if (!punto) return;
    punto++;
    while (*punto && i < 9) {
        destino[i] = (char) tolower((unsigned char) *punto);
        i++;
        punto++;
    }
    destino[i] = '\0';
}

/* Devuelve 1 si el archivo tiene una extension de imagen soportada */
int esImagenSoportada(const char *nombre) {
    char ext[10];
    obtenerExtension(nombre, ext);
    return (strcmp(ext, "png") == 0 || strcmp(ext, "jpg") == 0 ||
            strcmp(ext, "jpeg") == 0 || strcmp(ext, "bmp") == 0);
}

/* 1) Lista los archivos de imagen de 'carpeta' en el arreglo 'archivos'.
      Devuelve la cantidad encontrada, o -1 si no se pudo abrir la carpeta. */
int listarImagenes(const char *carpeta, char archivos[][MAX_NOMBRE], int maximo) {
    DIR *dir = opendir(carpeta);
    if (!dir) {
        fprintf(stderr, "Error: no se pudo abrir la carpeta '%s'\n", carpeta);
        return -1;
    }

    struct dirent *entrada;
    int cantidad = 0;
    while ((entrada = readdir(dir)) != NULL && cantidad < maximo) {
        if (esImagenSoportada(entrada->d_name)) {
            strncpy(archivos[cantidad], entrada->d_name, MAX_NOMBRE - 1);
            archivos[cantidad][MAX_NOMBRE - 1] = '\0';
            cantidad++;
        }
    }
    closedir(dir);
    return cantidad;
}

/* Carga la imagen, la pasa a escala de grises y la guarda como bn_<nombre> */
int convertirABlancoYNegro(const char *carpeta, const char *nombreArchivo) {
    char rutaEntrada[512];
    snprintf(rutaEntrada, sizeof(rutaEntrada), "%s/%s", carpeta, nombreArchivo);

    int ancho, alto, canalesOriginales;
    /* Forzamos 3 canales (RGB) para simplificar el calculo de luminancia */
    unsigned char *datos = stbi_load(rutaEntrada, &ancho, &alto, &canalesOriginales, 3);
    if (!datos) {
        fprintf(stderr, "Error al cargar la imagen '%s': %s\n", nombreArchivo, stbi_failure_reason());
        return 0;
    }

    unsigned char *gris = (unsigned char *) malloc((size_t) ancho * alto * 3);
    if (!gris) {
        fprintf(stderr, "Error: no se pudo reservar memoria\n");
        stbi_image_free(datos);
        return 0;
    }

    /* Formula estandar de luminancia perceptual (ITU-R BT.601) */
    for (int i = 0; i < ancho * alto; i++) {
        unsigned char r = datos[i * 3 + 0];
        unsigned char g = datos[i * 3 + 1];
        unsigned char b = datos[i * 3 + 2];
        unsigned char nivel = (unsigned char) (0.299 * r + 0.587 * g + 0.114 * b);
        gris[i * 3 + 0] = nivel;
        gris[i * 3 + 1] = nivel;
        gris[i * 3 + 2] = nivel;
    }
    stbi_image_free(datos);

    char extension[10];
    obtenerExtension(nombreArchivo, extension);

    char rutaSalida[512];
    snprintf(rutaSalida, sizeof(rutaSalida), "%s/bn_%s", carpeta, nombreArchivo);

    int resultado = 0;
    if (strcmp(extension, "png") == 0) {
        resultado = stbi_write_png(rutaSalida, ancho, alto, 3, gris, ancho * 3);
    } else if (strcmp(extension, "bmp") == 0) {
        resultado = stbi_write_bmp(rutaSalida, ancho, alto, 3, gris);
    } else { /* jpg o jpeg */
        resultado = stbi_write_jpg(rutaSalida, ancho, alto, 3, gris, 90);
    }

    free(gris);

    if (!resultado) {
        fprintf(stderr, "Error al guardar la imagen en '%s'\n", rutaSalida);
        return 0;
    }

    printf("-> Imagen convertida y guardada como: %s\n", rutaSalida);
    return 1;
}

/* 2) Menu para elegir el archivo a convertir */
void mostrarMenu(char archivos[][MAX_NOMBRE], int cantidad) {
    printf("\nImagenes encontradas:\n\n");
    for (int i = 0; i < cantidad; i++) {
        printf("  %2d) %s\n", i + 1, archivos[i]);
    }
    printf("   0) Salir\n");
}

int main(int argc, char *argv[]) {
    const char *carpeta = (argc > 1) ? argv[1] : ".";

    char archivos[MAX_ARCHIVOS][MAX_NOMBRE];
    int cantidad = listarImagenes(carpeta, archivos, MAX_ARCHIVOS);

    if (cantidad <= 0) {
        printf("No se encontraron imagenes (.png, .jpg, .jpeg, .bmp) en '%s'\n", carpeta);
        return EXIT_FAILURE;
    }

    int opcion;
    do {
        mostrarMenu(archivos, cantidad);
        printf("\nSeleccione el numero de archivo a convertir a blanco y negro: ");

        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n') { /* limpiar entrada invalida */ }
            printf("Entrada invalida.\n");
            opcion = -1;
            continue;
        }

        if (opcion == 0) {
            printf("Saliendo...\n");
            break;
        }

        if (opcion < 1 || opcion > cantidad) {
            printf("Opcion fuera de rango. Intente de nuevo.\n");
            continue;
        }

        convertirABlancoYNegro(carpeta, archivos[opcion - 1]);

    } while (opcion != 0);

    return EXIT_SUCCESS;
}
