#include <stdio.h>
#include <fcntl.h>
#include "get_next_line.h"

int main(int argc, char *argv[])
{
	int fd = open("a.txt", O_RDONLY);
	if (fd == -1)
	{
		printf("ERROR AL LEER EL ARCHIVO");
		return (1);
	}
	char *texto = get_next_line(fd);
	if (!texto)
	{
		printf("texto no asignado");
		return (0);
	}
	printf("%s\n", texto);
	free(texto);
    texto = get_next_line(fd);
	printf("%s\n", texto);
	free(texto);
    texto = get_next_line(fd);
    printf("%s\n", texto);
    free(texto);

	close(fd);
	return (0);
}
