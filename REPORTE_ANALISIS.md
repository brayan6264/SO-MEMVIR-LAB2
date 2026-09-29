# Reporte de análisis — Simulador de memoria virtual (paginación)

**Integrantes:** _Juan Camilo Arboleda, Brayan Gomez, Vanesa Herrera_
**Política de reemplazo asignada al grupo:** _(completar: FIFO o LRU)_ — el simulador implementa ambas políticas para poder compararlas (sección 5).

## 1. Descripción de las estructuras de datos

- **`pte_t`** (`include/page_table.h`): entrada de tabla de páginas con `frame` (marco físico) y los bits `valid`, `accessed` y `dirty`.
- **`page_table_t`** (`src/page_table.c`): tabla de dos niveles. El nivel 1 es un arreglo de punteros (1024 con páginas de 4KB); cada uno apunta a una tabla de nivel 2 de 1024 `pte_t`, creada bajo demanda la primera vez que se necesita (`page_table_get_pte` con `create=true`).
- **Dirección virtual (32 bits)**: con la página por defecto de 4KB se divide en `PT1` (bits 31-22), `PT2` (bits 21-12) y `offset` (bits 11-0).
- **`page_config_t`** (`src/page_config.c`): guarda el tamaño de página y calcula a partir de él la división de la dirección. El tamaño se configura con el 4.º argumento del programa (potencia de 2 entre 1KB y 64KB, 4KB por defecto). `PT2` se mantiene en 10 bits, el `offset` tiene log2(tamaño de página) bits y `PT1` usa los bits restantes: 8KB da 9/10/13 y 1KB da 12/10/10. La tabla de páginas, la memoria física y el asignador de direcciones reciben este tamaño al crearse.
- **`phys_mem_t`** (`src/phys_mem.c`): pila de marcos libres (`free_stack`) más un arreglo `owner` que indica qué `pte_t` ocupa cada marco. Entregar y devolver marcos es O(1). El tamaño es configurable por línea de comandos con un mínimo de 256KB (64 marcos).
- **`replacement_policy_t`** (`include/replacement.h`): interfaz Strategy con punteros a función (`on_load`, `on_access`, `select_victim`, `on_release`, `destroy`) y un estado opaco `self`. FIFO y LRU se eligen en tiempo de ejecución sin cambiar el resto del simulador.
- **`vaddr_alloc_table_t`** (`src/vaddr_alloc.c`): lista enlazada de bloques `{base, size}` reservados por `alloc`. Solo administra direcciones virtuales; no toca la memoria física.
- **`page_release`** (`src/page_release.c`): al hacer `free`, libera los marcos de las páginas del bloque, limpia sus PTE y avisa a la política (`on_release`) para que deje de considerar esos marcos.
- **`stats_t`** (`include/stats.h`): accesos, fallos, reemplazos y tiempo (tiempo acumulado atendiendo fallos de página y tiempo total de la simulación, medidos con `clock()`).

## 2. Política de reemplazo

### 2.1 FIFO (`src/replacement_fifo.c`)

Cola circular de marcos en orden de carga: `on_load` encola al final y `select_victim` desencola por el frente. `on_access` no hace nada, porque FIFO solo considera cuándo se cargó la página. `on_release` quita de la cola un marco liberado por `free`, para que la cola nunca tenga marcos repetidos ni supere su capacidad. Las operaciones normales son O(1) y la liberación es O(n).

### 2.2 LRU (`src/replacement_lru.c`)

Reloj lógico (`counter`) que avanza en cada `on_load` y `on_access`; `last_used[frame]` guarda la última marca de tiempo de cada marco. `select_victim` recorre los marcos y elige el de menor `last_used`: O(n) por reemplazo. `on_release` no necesita hacer nada, porque `on_load` reescribe la marca cuando el marco se vuelve a usar.

## 3. Resultados de las pruebas

Todas las pruebas corren con la memoria por defecto (256KB = 64 marcos). Las pruebas 2 a 4 tocan más de 64 páginas para forzar reemplazos.

| # | Prueba | Accesos | Fallos | Hit rate | Reemplazos | Política |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `test1_basico.txt` | 4 | 2 | 50.00% | 0 | FIFO y LRU |
| 2 | `test2_reemplazos.txt` | 68 | 68 | 0.00% | 4 | FIFO y LRU |
| 3 | `test3_alloc_free.txt` | 68 | 67 | 1.47% | 2 | FIFO |
| 3 | `test3_alloc_free.txt` | 68 | 66 | 2.94% | 1 | LRU |
| 4 | `test4_fifo_vs_lru.txt` | 88 | 80 | 9.09% | 16 | FIFO |
| 4 | `test4_fifo_vs_lru.txt` | 88 | 72 | 18.18% | 8 | LRU |

Los tiempos que se imprimen son del orden de milisegundos o menos: el costo de cada fallo simulado es solo una actualización de estructuras en memoria, y la resolución de `clock()` depende del sistema operativo.

## 4. Análisis: cambio de hit rate y número de reemplazos

**Prueba 1:** cada página se toca dos veces; el primer toque es un fallo y el segundo un acierto (50%). Sobran marcos, así que no hay reemplazos.

**Prueba 2 (acceso secuencial sin reuso):** se escriben 66 páginas con solo 64 marcos y luego se releen las páginas 0 y 1, que ya fueron desalojadas. Como ninguna página se reutiliza antes de ser desalojada, FIFO y LRU desalojan las mismas páginas en el mismo orden: resultados idénticos (0% de hit rate, 4 reemplazos).

**Prueba 3 (alloc/free con reemplazos):** se cargan las páginas 0 y 1, se libera el bloque de la página 1 y luego se llenan los 64 marcos (el marco liberado se reutiliza). Con FIFO, la página 0 es la más antigua y es la primera en salir cuando llega la página 65, así que la lectura final de la página 0 falla. Con LRU, la relectura de la página 0 la vuelve la más reciente, sobrevive y la lectura final es un acierto: LRU obtiene 1 fallo y 1 reemplazo menos.

**Prueba 4 (localidad temporal):** se llenan los 64 marcos, se releen las primeras 8 páginas, se cargan 8 páginas nuevas y se vuelven a leer las primeras 8. FIFO desaloja justo esas 8 páginas (las más antiguas), aunque se acababan de usar, y la segunda relectura produce 8 fallos más. LRU desaloja las páginas 8 a 15, que no se habían vuelto a usar, y la segunda relectura son 8 aciertos. LRU duplica el hit rate (18.18% frente a 9.09%) con la mitad de reemplazos (8 frente a 16).

## 5. Comparación teórica FIFO vs LRU

| Criterio | FIFO | LRU |
| --- | --- | --- |
| Costo por operación | O(1) (cola) | O(1) en `on_access`/`on_load`, O(n) en `select_victim` |
| Usa localidad temporal | No: solo importa cuándo se cargó la página | Sí: importa cuándo se usó por última vez |
| Hit rate típico | Igual o peor que LRU cuando hay reuso | Igual o mejor que FIFO cuando hay reuso |
| Caso patológico | Anomalía de Belady: más marcos pueden producir más fallos | No sufre la anomalía de Belady (es un algoritmo de pila) |
| Memoria extra | Una cola de tamaño = número de marcos | Un arreglo de marcas de tiempo del mismo tamaño |
| Cuándo conviene | Cargas simples y deterministas | Cargas con reuso de datos recientes (la mayoría de programas reales) |

Las pruebas confirman la teoría: sin reuso (prueba 2) ambas políticas son equivalentes; con reuso (pruebas 3 y 4) LRU gana.

## 6. Reproducibilidad

```bash
make all
./memsim tests/test1_basico.txt fifo
./memsim tests/test2_reemplazos.txt lru
./memsim tests/test3_alloc_free.txt fifo
./memsim tests/test4_fifo_vs_lru.txt lru
./memsim tests/test4_fifo_vs_lru.txt lru 256 8192
```

Los contadores son deterministas; los tiempos pueden variar entre máquinas. La última línea usa páginas de 8KB.

## 7. Verificación de memoria (valgrind)

Se ejecutó valgrind 3.26.0 en Ubuntu (WSL 2), después de compilar con `make all`:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./memsim tests/test4_fifo_vs_lru.txt lru
```

```
==4644== HEAP SUMMARY:
==4644==     in use at exit: 0 bytes in 0 blocks
==4644==   total heap usage: 14 allocs, 14 frees, 26,496 bytes allocated
==4644== All heap blocks were freed -- no leaks are possible
==4644== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

El mismo resultado (0 fugas y 0 errores) se obtuvo con las 4 pruebas en FIFO y en LRU, con páginas de 1KB, 8KB y 64KB, con las salidas por error (archivo inexistente, política inválida y tamaño de página inválido) y con las pruebas en C `test_translate` y `test_paso2_fifo`.
