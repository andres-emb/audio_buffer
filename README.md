# Audio Buffer circular

Proyecto para un buffer circular de bloques de audio PCM con profundidad de 16 bits, frecuencia de muestreo de 16 kHz y duración de cada bloque de 10 ms.

## Descripción

El buffer está pensado para recibir audio en bloques desde RTP y servirlos luego a un módulo I2S. Cada bloque contiene:

- 160 muestras por bloque
- 16 bits por muestra
- 10 ms de audio

Esto da una fórmula de:

$$
\text{muestras por bloque} = 16000 \times 0.010 = 160
$$

## Características

- Buffer circular FIFO para bloques de audio.
- Inserción de un bloque con `audio_buffer_push()`.
- Extracción de un bloque con `audio_buffer_pop()`.
- Extracción de varios bloques con `audio_buffer_pop_many()`.
- Consulta no destructiva del siguiente bloque con `audio_buffer_peak()`.
- Consulta no destructiva de varios bloques con `audio_buffer_peak_many()`.
- Consulta del número de bloques almacenados con `audio_buffer_count()`.
- Consulta de capacidad con `audio_buffer_capacity()`.
- Detección de vacío y lleno con `audio_buffer_is_empty()` y `audio_buffer_is_full()`.
- Limpieza total del buffer con `audio_buffer_clear()`.
- Diseño orientado a audio PCM de 16 bits a 16 kHz.

## Estructura del proyecto

- [inc/audio_buffer.h](inc/audio_buffer.h): API del buffer.
- [src/audio_buffer.c](src/audio_buffer.c): implementación del buffer circular.
- [test/test_audio_buffer.c](test/test_audio_buffer.c): pruebas unitarias con Unity.
- [project.yml](project.yml): configuración de Ceedling.
- [Makefile](Makefile): compilación y ejecución del proyecto.

## Compilación y pruebas

Desde la raíz del proyecto:

```bash
make build
make test
```

También se puede ejecutar directamente:

```bash
make clean && make build && make test
```

## Estado verificado

La compilación y las pruebas han sido validadas en este proyecto con Ceedling:

- `TESTED: 14`
- `PASSED: 14`
- `FAILED: 0`
- `IGNORED: 0`

## Consideraciones de uso

Este buffer se usa como cola de bloques de audio para flujos RTP, de forma que:

- cada bloque entrante se encola,
- el consumidor extrae bloques según disponibilidad,
- el buffer recicla posiciones en memoria circularmente,
- el sistema mantiene un ancho de banda y latencia de 10 ms por bloque.

