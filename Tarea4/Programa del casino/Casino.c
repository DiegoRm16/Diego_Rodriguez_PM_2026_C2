#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    int OPC, NJUG, NUM, GANA, i;
    float DIN, PREMIO;

    NJUG = 1;

    do
    {
        printf("\n\n===== MENU DE LOTERIA =====");
        printf("\n1. Jugar");
        printf("\n2. Elegir cantidad de jugadas");
        printf("\n3. Salir");
        printf("\n\nIngrese una opcion: ");
        scanf("%d", &OPC);

        switch(OPC)
        {
            case 1:
                printf("\n--- JUGADAS DE LOTERIA ---");
                printf("\nCantidad de jugadas: %d\n", NJUG);

                i = 1;
                while (i <= NJUG)
                {
                    GANA = (rand() % 40) + 1;
                    printf("\n-- Jugada %d --", i);
                    printf("\nIngrese su numero (1 al 40): ");
                    scanf("%d", &NUM);

                    if (NUM < 1 || NUM > 40)
                    {
                        printf("Numero invalido. Debe estar entre 1 y 40.");
                    }
                    else
                    {
                        printf("Ingrese la cantidad de dinero a apostar: $");
                        scanf("%f", &DIN);

                        if (NUM == GANA)
                        {
                            PREMIO = DIN * 1000;
                            printf("\n*** FELICIDADES! El numero ganador era %d ***", GANA);
                            printf("\nGanaste: $%.2f", PREMIO);
                        }
                        else
                        {
                            printf("\nResultado: Perdiste en esta jugada.");
                            printf("\nEl numero ganador era: %d", GANA);
                        }

                        i = i + 1;
                    }
                }
                break;

            case 2:
                printf("\nIngrese la cantidad de jugadas: ");
                scanf("%d", &NJUG);

                if (NJUG < 1)
                {
                    printf("Cantidad invalida. Se asigna 1 jugada por defecto.");
                    NJUG = 1;
                }
                else
                {
                    printf("Cantidad de jugadas establecida: %d", NJUG);
                }
                break;

            case 3:
                printf("\nGracias por jugar. Hasta luego!\n");
                break;

            default:
                printf("\nOpcion invalida. Ingrese 1, 2 o 3.");
                break;
        }

    } while (OPC != 3);
}
