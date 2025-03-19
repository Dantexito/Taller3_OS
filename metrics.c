#include <stdio.h>

#define MAX_PROCESOS 100  // Ajusta según sea necesario

typedef struct {
    int id;
    int tiempo_llegada;
    int duracion;
} Proceso;

int main() {
    FILE *archivo;
    Proceso procesos[MAX_PROCESOS];
    int contador = 0;

    // Abre el archivo en modo lectura
    archivo = fopen("../procesos.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Leer los datos del archivo
    while (fscanf(archivo, "%d %d %d", &procesos[contador].id, 
                                        &procesos[contador].tiempo_llegada, 
                                        &procesos[contador].duracion) == 3) {
        contador++;
    }

    // Cierra el archivo
    fclose(archivo);

    // Mostrar los procesos leídos
    printf("Procesos cargados:\n");
    for (int i = 0; i < contador; i++) {
        printf("ID: %d, Tiempo de llegada: %d, Duración: %d\n",
               procesos[i].id, procesos[i].tiempo_llegada, procesos[i].duracion);
    }

    return 0;
}
