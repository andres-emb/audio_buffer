#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define AUDIO_SAMPLE_RATE_HZ    16000U
#define AUDIO_BLOCK_DURATION_MS 10U
#define AUDIO_BLOCK_SAMPLE_COUNT ((AUDIO_SAMPLE_RATE_HZ * AUDIO_BLOCK_DURATION_MS) / 1000U)

#define AUDIO_BLOCK_SAMPLES AUDIO_BLOCK_SAMPLE_COUNT

typedef struct
{
    int16_t samples[AUDIO_BLOCK_SAMPLES];
} audio_block_t;

typedef struct
{
    audio_block_t *storage;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
} audio_buffer_t;

void audio_buffer_init(audio_buffer_t *buffer, audio_block_t *storage, size_t capacity);
bool audio_buffer_push(audio_buffer_t *buffer, const audio_block_t *block);
bool audio_buffer_pop(audio_buffer_t *buffer, audio_block_t *out_block);
bool audio_buffer_peak(const audio_buffer_t *buffer, audio_block_t *out_block);
size_t audio_buffer_peak_many(const audio_buffer_t *buffer, audio_block_t *out_blocks, size_t max_blocks);
size_t audio_buffer_pop_many(audio_buffer_t *buffer, audio_block_t *out_blocks, size_t max_blocks);
void audio_buffer_clear(audio_buffer_t *buffer);
size_t audio_buffer_count(const audio_buffer_t *buffer);
size_t audio_buffer_capacity(const audio_buffer_t *buffer);
bool audio_buffer_is_empty(const audio_buffer_t *buffer);
bool audio_buffer_is_full(const audio_buffer_t *buffer);

#endif
