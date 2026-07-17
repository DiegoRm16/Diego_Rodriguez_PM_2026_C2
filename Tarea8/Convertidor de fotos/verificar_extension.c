/*
 * verificar_extension.c
 * ---------------------------------------------------------
 * Recorre los archivos de una carpeta y, segun su extension
 * (.png, .jpg/.jpeg, .bmp), verifica si el contenido real del
 * archivo corresponde efectivamente a ese formato, leyendo su
 * "numero magico" (firma de bytes) en el encabezado del archivo.
 * Esto permite detectar archivos con la extension cambiada o
 * corruptos, sin necesidad de decodificar la imagen completa.
 *
 * Firmas usadas:
 *   PNG : 89 50 4E 47 0D 0A 1A 0A
 *   JPG : FF D8 FF
 *   BMP : 42 4D  ("BM")
 *
 * Compilar:  gcc -Wall -o verificar_extension verificar_extension.c
 * Ejecutar:  ./verificar_extension [carpeta]     (por defecto: carpeta actual)
 * ---------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

typedef enum {
    TIPO_DESCONOCIDO,
    TIPO_PNG,
    TIPO_JPG,
    TIPO_BMP
} TipoImagen;

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

/* Lee los primeros bytes del archivo y determina el tipo real segun su firma */
TipoImagen detectarTipoReal(const char *ruta) {
    unsigned char cabecera[8] = {0};
    FILE *archivo = fopen(ruta, "rb");
    if (!archivo) return TIPO_DESCONOCIDO;

    size_t leidos = fread(cabecera, 1, sizeof(cabecera), archivo);
    fclose(archivo);

    static const unsigned char firmaPNG[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    if (leidos >= 8 && memcmp(cabecera, firmaPNG, 8) == 0) return TIPO_PNG;
    if (leidos >= 3 && cabecera[0] == 0xFF && cabecera[1] == 0xD8 && cabecera[2] == 0xFF) return TIPO_JPG;
    if (leidos >= 2 && cabecera[0] == 'B' && cabecera[1] == 'M') return TIPO_BMP;

    return TIPO_DESCONOCIDO;
}

const char *nombreTipo(TipoImagen tipo) {
    switch (tipo) {
        case TIPO_PNG: return "PNG";
        case TIPO_JPG: return "JPG";
        case TIPO_BMP: return "BMP";
        default:       return "DESCONOCIDO";
    }
}

/* Compara la extension declarada del archivo contra el tipo detectado por contenido */
int extensionCoincide(const char *extension, TipoImagen tipoReal) {
    if (strcmp(extension, "png") == 0)  return tipoReal == TIPO_PNG;
    if (strcmp(extension, "bmp") == 0)  return tipoReal == TIPO_BMP;
    if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0) return tipoReal == TIPO_JPG;
    return 0;
}

int main(int argc, char *argv[]) {
    const char *carpeta = (argc > 1) ? argv[1] : ".";

    DIR *dir = opendir(carpeta);
    if (!dir) {
        fprintf(stderr, "Error: no se pudo abrir la carpeta '%s'\n", carpeta);
        return EXIT_FAILURE;
    }

    printf("%-30s %-6s %-14s %-8s\n", "Archivo", "Ext.", "Tipo real", "Valido?");
    printf("--------------------------------------------------------------------\n");

    struct dirent *entrada;
    int total = 0, validos = 0;

    while ((entrada = readdir(dir)) != NULL) {
        char extension[10];
        obtenerExtension(entrada->d_name, extension);

        /* Solo analizamos archivos con extension de imagen soportada */
        if (strcmp(extension, "png") != 0 && strcmp(extension, "jpg") != 0 &&
            strcmp(extension, "jpeg") != 0 && strcmp(extension, "bmp") != 0) continue;

        char ruta[512];
        snprintf(ruta, sizeof(ruta), "%s/%s", carpeta, entrada->d_name);

        TipoImagen tipoReal = detectarTipoReal(ruta);
        int ok = extensionCoincide(extension, tipoReal);

        printf("%-30s %-6s %-14s %-8s\n", entrada->d_name, extension,
               nombreTipo(tipoReal), ok ? "SI" : "NO");

        total++;
        if (ok) validos++;
    }
    closedir(dir);

    printf("--------------------------------------------------------------------\n");
    printf("Total analizados: %d | Validos: %d | Invalidos: %d\n", total, validos, total - validos);

    return EXIT_SUCCESS;
}
