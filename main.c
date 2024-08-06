#include <stdio.h>
#include <fcntl.h>
#include "get_next_line.h"

int main()
{
	int fd = open("a.txt", O_RDONLY);
	if (fd == -1)
	{
		printf("ERROR AL LEER EL ARCHIVO");
		return (1);
	}
	char *texto = get_next_line(fd);
	while (texto)
	{
		if (!texto)
		{
			printf("texto no asignado");
			return (0);
		}
		printf("%s\n", texto);
		free(texto);
		texto = get_next_line(fd);
	}
    free(texto);

	close(fd);
	return (0);
}
