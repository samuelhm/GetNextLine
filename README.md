# Get Next Line

![C](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![42School](https://img.shields.io/badge/School-42_Barcelona-000000?style=for-the-badge&logo=42&logoColor=white)
![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)

![Memory Management](https://img.shields.io/badge/Skill-Memory_Management-blue?style=flat-square)
![File Descriptors](https://img.shields.io/badge/Concept-File_Descriptors-green?style=flat-square)
![Static Variables](https://img.shields.io/badge/Concept-Static_Variables-9cf?style=flat-square)
![Algorithm Design](https://img.shields.io/badge/Skill-Algorithm_Design-orange?style=flat-square)
![Critical Thinking](https://img.shields.io/badge/Soft_Skill-Critical_Thinking-purple?style=flat-square)

---

## 📝 Descripción

Implementación en C de una función que lee líneas completas desde un file descriptor, manejando eficientemente buffers de tamaño configurable. El proyecto demuestra dominio de memoria dinámica, variables estáticas y manipulación de strings a bajo nivel—habilidades fundamentales para el desarrollo de sistemas.

---

## ✨ Características Principales

- **Lectura línea a línea**: Devuelve cada línea completa incluyendo el carácter `\n` si existe
- **Buffer configurable**: Tamaño de buffer definible en compilación mediante `BUFFER_SIZE`
- **Gestión de memoria eficiente**: Libera automáticamente los recursos tras cada llamada
- **Manejo robusto de errores**: Gestiona EOF, archivos vacíos y errores de lectura
- **Multi-descriptor (Bonus)**: Soporte para leer simultáneamente de múltiples file descriptors
- **Implementación sin dependencias externas**: Utiliza solo funciones de la biblioteca estándar de C

---

## 🛠 Stack Tecnológico

| Categoría | Tecnología |
|-----------|------------|
| Lenguaje | C (C99) |
| Compilación | GCC / Clang |
| Sistema Operativo | Linux / macOS |
| Librerías | `<stdlib.h>`, `<unistd.h>`, `<stddef.h>` |
| Estilo de Código | Norminette 42 |

---

## 🏗 Decisiones Técnicas / Arquitectura

La función `get_next_line` resuelve el desafío de leer archivos de forma incremental sin cargar todo el contenido en memoria. Utiliza una **variable estática** para preservar el estado del buffer entre llamadas consecutivas, almacenando los caracteres que sobrepasan la línea actual hasta la siguiente invocación.

Esta arquitectura resuelve el problema fundamental de alinear discontinuidades entre el tamaño del buffer y las líneas del archivo. La separación entre funciones de lectura (`read_line`) y extracción (`get_line`, `get_save`) aplica el principio de responsabilidad única.

En la **versión bonus**, el diseño se extiende con un array de punteros estáticos indexados por file descriptor (`static char *save[FOPEN_MAX]`), permitiendo gestionar múltiples archivos abiertos simultáneamente sin interferencia entre ellos. El `BUFFER_SIZE` por defecto de 512 bytes equilibra el overhead de syscalls con el uso de memoria.

---

## 📊 Diagrama de Arquitectura

```mermaid
flowchart TD
    subgraph Init["Inicialización"]
        A[get_next_line fd] --> B{fd válido y BUFFER_SIZE > 0?}
        B -->|No| C[return NULL]
        B -->|Sí| D[Reservar buffer temporal]
    end

    subgraph Reading["Lectura Iterativa"]
        D --> E[read_line: leer del fd]
        E --> F{¿newline en save?}
        F -->|No| G[read fd: BUFFER_SIZE bytes]
        G --> H{bytes_read > 0?}
        H -->|Sí| I[strjoin: buffer + save]
        I --> F
        H -->|EOF o Error| J[Retornar save actual]
        F -->|Sí| J
    end

    subgraph Extraction["Extracción de Línea"]
        J --> K[get_line: extraer hasta newline]
        K --> L[get_save: preservar restante]
        L --> M[Liberar buffer temporal]
        M --> N[return línea completa]
    end

    G -.->|"syscall read()"| O[(File Descriptor)]
    O -.->|"bytes_read"| G
```

---

## 🚀 Guía de Instalación

### Prerrequisitos

- GCC o Clang instalado
- Sistema operativo UNIX/Linux/macOS

### Instalación

```bash
# Clonar el repositorio
git clone https://github.com/samuelhm/GetNextLine.git
cd GetNextLine
```

### Compilación

```bash
# Versión mandatory
gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c -o gnl

# Versión bonus (multi-descriptor)
gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c -o gnl_bonus
```

### Ejemplo de Uso

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int fd = open("archivo.txt", O_RDONLY);
    char *line;

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

---

## 📁 Estructura del Proyecto

```
GetNextLine/
├── get_next_line.c              # Implementación principal
├── get_next_line.h              # Header principal
├── get_next_line_utils.c        # Funciones auxiliares
├── get_next_line_bonus.c        # Implementación multi-fd
├── get_next_line_bonus.h        # Header versión bonus
└── get_next_line_utils_bonus.c  # Utilidades versión bonus
```

---

## 📩 Contacto

| Plataforma | Enlace |
|------------|--------|
| **GitHub** | [github.com/samuelhm](https://github.com/samuelhm/) |
| **LinkedIn** | [linkedin.com/in/shurtado-m](https://www.linkedin.com/in/shurtado-m/) |

---

<p align="center"><i>Desarrollado como parte del currículum de 42 Barcelona</i></p>