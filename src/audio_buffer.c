#include "audio_buffer.h"

void audio_buffer_init(audio_buffer_t *buffer, audio_block_t *storage, size_t capacity)
{
    if (buffer == NULL)
    {
        return;
    }

    buffer->storage = storage;
    buffer->capacity = capacity;
    buffer->head = 0U;
    buffer->tail = 0U;
    buffer->count = 0U;
}

bool audio_buffer_push(audio_buffer_t *buffer, const audio_block_t *block)
{
    if ((buffer == NULL) || (block == NULL) || (buffer->capacity == 0U) || (buffer->storage == NULL))
    {
        return false;
    }

    if (audio_buffer_is_full(buffer))
    {
        return false;
    }

    buffer->storage[buffer->head] = *block;
    buffer->head = (buffer->head + 1U) % buffer->capacity;
    buffer->count++;
    return true;
}

bool audio_buffer_pop(audio_buffer_t *buffer, audio_block_t *out_block)
{
    if ((buffer == NULL) || (out_block == NULL) || (buffer->capacity == 0U) || audio_buffer_is_empty(buffer))
    {
        return false;
    }

    *out_block = buffer->storage[buffer->tail];
    buffer->tail = (buffer->tail + 1U) % buffer->capacity;
    buffer->count--;
    return true;
}

bool audio_buffer_peak(const audio_buffer_t *buffer, audio_block_t *out_block)
{
    if ((buffer == NULL) || (out_block == NULL) || (buffer->capacity == 0U) || audio_buffer_is_empty(buffer))
    {
        return false;
    }

    *out_block = buffer->storage[buffer->tail];
    return true;
}

size_t audio_buffer_peak_many(const audio_buffer_t *buffer, audio_block_t *out_blocks, size_t max_blocks)
{
    size_t peeked = 0U;
    size_t index = 0U;

    if ((buffer == NULL) || (out_blocks == NULL) || (max_blocks == 0U) || (buffer->capacity == 0U))
    {
        return 0U;
    }

    while ((peeked < max_blocks) && !audio_buffer_is_empty(buffer) && (index < buffer->count))
    {
        out_blocks[peeked] = buffer->storage[(buffer->tail + index) % buffer->capacity];
        peeked++;
        index++;
    }

    return peeked;
}

size_t audio_buffer_pop_many(audio_buffer_t *buffer, audio_block_t *out_blocks, size_t max_blocks)
{
    size_t popped = 0U;

    if ((buffer == NULL) || (out_blocks == NULL) || (max_blocks == 0U) || (buffer->capacity == 0U))
    {
        return 0U;
    }

    while ((popped < max_blocks) && !audio_buffer_is_empty(buffer))
    {
        if (!audio_buffer_pop(buffer, &out_blocks[popped]))
        {
            break;
        }
        popped++;
    }

    return popped;
}

void audio_buffer_clear(audio_buffer_t *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    buffer->head = 0U;
    buffer->tail = 0U;
    buffer->count = 0U;
}

size_t audio_buffer_count(const audio_buffer_t *buffer)
{
    if (buffer == NULL)
    {
        return 0U;
    }

    return buffer->count;
}

size_t audio_buffer_capacity(const audio_buffer_t *buffer)
{
    if (buffer == NULL)
    {
        return 0U;
    }

    return buffer->capacity;
}

bool audio_buffer_is_empty(const audio_buffer_t *buffer)
{
    return (buffer != NULL) && (buffer->count == 0U);
}

bool audio_buffer_is_full(const audio_buffer_t *buffer)
{
    return (buffer != NULL) && (buffer->capacity != 0U) && (buffer->count >= buffer->capacity);
}
