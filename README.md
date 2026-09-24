# Simulador de memoria virtual (paginación)

Simula la traducción de direcciones virtuales a direcciones físicas usando paginación de dos niveles (parecido a x86-64 pero simplificado). Maneja fallos de página y, cuando ya no queda memoria física libre, aplica una política de reemplazo (FIFO o LRU, se elige al ejecutar el programa).

## Requisitos

- gcc con soporte para C99
- make
- valgrind (opcional, para revisar memory leaks)

## Compilación

```bash
make all
```

Esto genera el ejecutable `memsim`. Para borrar los binarios generados:

```bash
make clean
```

## Uso

```bash
./memsim <archivo_entrada> <fifo|lru> [memoria_fisica_kb] [tamano_pagina_bytes]
```

- `archivo_entrada`: archivo de texto con los comandos a simular (ver formato abajo).
- `fifo|lru`: qué política de reemplazo usar cuando no hay marcos físicos libres.
- `memoria_fisica_kb` (opcional): tamaño de la memoria física en KB. Por defecto 256.
- `tamano_pagina_bytes` (opcional): **no cambia el tamaño de página real.** El tamaño de página es una constante de compilación (`PAGE_SIZE` en `config.h`, 4096 bytes) usada en varios módulos ya probados; no es configurable en tiempo de ejecución en esta versión. Si se pasa un valor distinto de 4096, el programa lo ignora y avisa por `stderr`, pero sigue corriendo con el valor fijo.

Ejemplo:

```bash
./memsim tests/test1_basico.txt fifo
```

También se puede correr con el target `run` del Makefile:

```bash
make run ARGS="tests/test1_basico.txt lru"
```

## Formato del archivo de entrada

Un comando por línea:

```
alloc <bytes>
write <direccion_virtual> <valor>
read <direccion_virtual>
free <direccion_virtual>
```

- `alloc <bytes>`: reserva un bloque de memoria virtual (se redondea hacia arriba al tamaño de página) y lo deja disponible para usarse en los siguientes `write`/`read`.
- `write <direccion_virtual> <valor>`: dispara la traducción VA→PA sobre esa dirección (crea tablas de nivel 2 si hace falta, y un fallo de página si la página no está cargada). El simulador **no almacena el contenido real de memoria**: el `<valor>` se parsea para mantener el formato del comando, pero se descarta — el objetivo es ejercitar la traducción y las estadísticas, no simular el contenido de la memoria byte por byte.
- `read <direccion_virtual>`: dispara la misma traducción VA→PA, sin marcar la página como modificada. Por la misma razón que `write`, no devuelve ni imprime ningún valor leído.
- `free <direccion_virtual>`: libera el bloque que empezó en esa dirección (tiene que ser una dirección devuelta por un `alloc` anterior, no cualquier dirección dentro del bloque).

Ejemplo de archivo de entrada:

```
alloc 8192
write 0 42
write 4096 99
read 0
read 4096
```

## Salida

Al terminar de leer el archivo de entrada, el programa imprime las estadísticas de la corrida:

```
Total de accesos: N
Total fallos de pagina: M
Hit rate: XX.XX%
Total reemplazos: K
Politica: FIFO|LRU
```

## Estructura del proyecto

```
include/   headers (.h) de cada modulo
src/       implementacion (.c)
tests/     archivos de entrada para probar el simulador
Makefile
```

Cada archivo se encarga de una sola cosa: la tabla de páginas y la traducción de direcciones van por separado del manejo de memoria física, los fallos de página y las políticas de reemplazo, y estos a su vez van separados del punto de entrada del programa. La idea es que ningún archivo crezca demasiado ni mezcle responsabilidades distintas.

## Notas de la implementación

- La dirección virtual es de 32 bits y se parte en PT1 (10 bits) / PT2 (10 bits) / offset (12 bits).
- Las tablas de segundo nivel se crean dinámicamente la primera vez que hacen falta, no se reserva todo de entrada.
- La política de reemplazo (FIFO o LRU) se elige por línea de comandos, no hace falta recompilar para cambiarla.
- El simulador prioriza la exactitud de la traducción VA→PA, el manejo de fallos y las estadísticas (que es lo que pide la salida esperada) sobre simular el contenido real de la memoria; ver la sección "Formato del archivo de entrada" arriba.