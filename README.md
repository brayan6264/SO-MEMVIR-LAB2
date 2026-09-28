# Simulador de memoria virtual (paginacion)

Laboratorio de Sistemas Operativos. El programa simula como se traducen direcciones virtuales a fisicas usando paginación de dos niveles. Cuando una pagina no esta en memoria hay un fallo de pagina, y si ya no quedan marcos libres se usa una politica de reemplazo, FIFO o LRU, que se escoge al correr el programa.

## Integrantes

- Juan Camilo Arboleda Arboleda
- Vanesa Herrera Marulanda
- Brayan Stiven Gómez Villa

## Que se necesita

- gcc (con C99)
- make
- valgrind si quieren revisar memory leaks (nosotros lo corrimos en WSL con Ubuntu)

## Compilar

```bash
make all
```

Eso deja el ejecutable `memsim`. Para borrar lo compilado:

```bash
make clean
```

## Como se usa

```bash
./memsim <archivo_entrada> <fifo|lru> [memoria_fisica_kb] [tamano_pagina_bytes]
```

- `archivo_entrada`: el txt con los comandos (el formato esta mas abajo).
- `fifo|lru`: la politica de reemplazo.
- `memoria_fisica_kb`: opcional, por defecto 256. No deja poner menos de 256.
- `tamano_pagina_bytes`: opcional, por defecto 4096. Tiene que ser potencia de 2 entre 1024 y 65536. Con 4096 la direccion queda como en el enunciado (10 bits PT1, 10 bits PT2 y 12 de offset), si se cambia la PT2 sigue con 10 bits y lo que cambia es el offset y la PT1.

Por ejemplo:

```bash
./memsim tests/test1_basico.txt fifo
./memsim tests/test4_fifo_vs_lru.txt lru 256 8192
```

o con el makefile:

```bash
make run ARGS="tests/test1_basico.txt lru"
```

## Archivo de entrada

Un comando por linea:

```
alloc <bytes>
write <direccion_virtual> <valor>
read <direccion_virtual>
free <direccion_virtual>
```

- `alloc` reserva un bloque de memoria virtual, se redondea hacia arriba a paginas completas.
- `write` y `read` hacen la traduccion VA->PA de esa direccion. Si la tabla de segundo nivel no existe se crea, y si la pagina no esta cargada hay fallo de pagina. El `write` ademas marca la pagina como modificada (dirty).
- `free` libera el bloque que empieza en esa direccion. Tiene que ser la que devolvio el `alloc`, no una del medio del bloque.

Ojo: el simulador no guarda el contenido de la memoria. El valor del `write` se lee pero no se guarda en ningun lado y el `read` no imprime nada, lo que importa aca es la traduccion y las estadisticas.

Las direcciones van en decimal. Las lineas vacias o que empiezan con `#` se ignoran.

Ejemplo:

```
alloc 8192
write 0 42
write 4096 99
read 0
read 4096
```

## Salida

Al final imprime algo asi:

```
Total de accesos: N
Total fallos de pagina: M
Hit rate: XX.XX%
Total reemplazos: K
Politica: FIFO|LRU
Tiempo en fallos de pagina: X.XXX ms
Tiempo total: X.XXX ms
```

Los tiempos se miden con `clock()`. En Windows casi siempre salen en 0 porque las pruebas son muy cortas, en Linux si se ven valores.

## Pruebas

En `tests/` estan los txt que usamos:

- `test1_basico.txt`: el ejemplo del enunciado.
- `test2_reemplazos.txt`: 66 paginas seguidas con 64 marcos, para que haya reemplazos.
- `test3_alloc_free.txt`: alloc y free mezclados con reemplazos.
- `test4_fifo_vs_lru.txt`: aqui es donde se nota la diferencia entre FIFO y LRU.

Tambien hay dos pruebas en C (`test_translate.c` y `test_paso2_fifo.c`) que no estan en el makefile, toca compilarlas a mano, por ejemplo:

```bash
gcc -Wall -Werror -std=c99 -Iinclude tests/test_paso2_fifo.c src/domain/page_table.c src/domain/page_config.c src/application/translate.c src/domain/stats.c src/domain/phys_mem.c src/domain/replacement_fifo.c src/application/page_fault.c -o test_paso2_fifo
```

## Estructura

```
include/   los .h, con las mismas carpetas que src
src/
  domain/          tabla de paginas, memoria fisica, FIFO, LRU, estadisticas
  application/     traduccion, fallos de pagina, free, comandos
  infrastructure/  argumentos de consola y la fabrica de politicas
  main.c
obj/       los .o que genera el make (no se suben)
tests/     archivos de prueba
Makefile
```

