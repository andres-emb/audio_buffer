#include <string.h>

#include "unity.h"
#include "audio_buffer.h"

static audio_block_t test_storage[8];
static audio_buffer_t test_buffer;

void setUp(void)
{
    memset(test_storage, 0, sizeof(test_storage));
    audio_buffer_init(&test_buffer, test_storage, 8);
}

void tearDown(void)
{
    audio_buffer_clear(&test_buffer);
}

void test_buffer_initializes_empty(void)
{
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
    TEST_ASSERT_FALSE(audio_buffer_is_full(&test_buffer));
    TEST_ASSERT_EQUAL_UINT32(0U, audio_buffer_count(&test_buffer));
    TEST_ASSERT_EQUAL_UINT32(8U, audio_buffer_capacity(&test_buffer));
}

void test_buffer_push_and_pop_single_block(void)
{
    audio_block_t block = {{ 100, 200, 300, 400, 500 }};
    audio_block_t output = {{ 0 }};

    audio_buffer_push(&test_buffer, &block);
    audio_buffer_pop(&test_buffer, &output);

    TEST_ASSERT_EQUAL_INT16(100, output.samples[0]);
    TEST_ASSERT_EQUAL_INT16(200, output.samples[1]);
    TEST_ASSERT_EQUAL_INT16(300, output.samples[2]);
    TEST_ASSERT_EQUAL_INT16(400, output.samples[3]);
    TEST_ASSERT_EQUAL_INT16(500, output.samples[4]);
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
}

void test_buffer_pushes_until_full_and_wraps_around(void)
{
    audio_block_t first = {{ 1, 2, 3, 4, 5 }};
    audio_block_t second = {{ 10, 20, 30, 40, 50 }};
    audio_block_t third = {{ 100, 200, 300, 400, 500 }};
    audio_block_t fourth = {{ 1000, 2000, 3000, 4000, 5000 }};
    audio_block_t out = {{ 0 }};

    audio_buffer_init(&test_buffer, test_storage, 3);

    audio_buffer_push(&test_buffer, &first);
    audio_buffer_push(&test_buffer, &second);
    audio_buffer_push(&test_buffer, &third);
    TEST_ASSERT_TRUE(audio_buffer_is_full(&test_buffer));

    audio_buffer_pop(&test_buffer, &out);
    TEST_ASSERT_EQUAL_INT16(1, out.samples[0]);

    audio_buffer_push(&test_buffer, &fourth);
    TEST_ASSERT_EQUAL_UINT32(3U, audio_buffer_count(&test_buffer));

    audio_buffer_pop(&test_buffer, &out);
    TEST_ASSERT_EQUAL_INT16(10, out.samples[0]);
    audio_buffer_pop(&test_buffer, &out);
    TEST_ASSERT_EQUAL_INT16(100, out.samples[0]);
    audio_buffer_pop(&test_buffer, &out);
    TEST_ASSERT_EQUAL_INT16(1000, out.samples[0]);
}

void test_buffer_reads_multiple_blocks(void)
{
    audio_block_t first = {{ 1, 1, 1, 1, 1 }};
    audio_block_t second = {{ 2, 2, 2, 2, 2 }};
    audio_block_t third = {{ 3, 3, 3, 3, 3 }};
    audio_block_t output[2] = {0};

    audio_buffer_init(&test_buffer, test_storage, 5);
    audio_buffer_push(&test_buffer, &first);
    audio_buffer_push(&test_buffer, &second);
    audio_buffer_push(&test_buffer, &third);

    TEST_ASSERT_EQUAL_UINT32(2U, audio_buffer_pop_many(&test_buffer, output, 2));
    TEST_ASSERT_EQUAL_INT16(1, output[0].samples[0]);
    TEST_ASSERT_EQUAL_INT16(2, output[1].samples[0]);
    TEST_ASSERT_EQUAL_UINT32(1U, audio_buffer_count(&test_buffer));
}

void test_buffer_clear_removes_all_elements(void)
{
    audio_block_t first = {{ 11, 22, 33, 44, 55 }};
    audio_block_t second = {{ 66, 77, 88, 99, 110 }};

    audio_buffer_init(&test_buffer, test_storage, 2);
    audio_buffer_push(&test_buffer, &first);
    audio_buffer_push(&test_buffer, &second);

    audio_buffer_clear(&test_buffer);

    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
    TEST_ASSERT_EQUAL_UINT32(0U, audio_buffer_count(&test_buffer));
}

void test_pop_from_empty_buffer_returns_false(void)
{
    audio_block_t output = {{ 0 }};

    TEST_ASSERT_FALSE(audio_buffer_pop(&test_buffer, &output));
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
    TEST_ASSERT_EQUAL_UINT32(0U, audio_buffer_count(&test_buffer));
}

void test_push_to_full_buffer_returns_false(void)
{
    audio_block_t a = {{ 1, 2, 3, 4, 5 }};
    audio_block_t b = {{ 6, 7, 8, 9, 10 }};
    audio_block_t c = {{ 11, 12, 13, 14, 15 }};
    audio_block_t d = {{ 16, 17, 18, 19, 20 }};

    audio_buffer_init(&test_buffer, test_storage, 3);

    TEST_ASSERT_TRUE(audio_buffer_push(&test_buffer, &a));
    TEST_ASSERT_TRUE(audio_buffer_push(&test_buffer, &b));
    TEST_ASSERT_TRUE(audio_buffer_push(&test_buffer, &c));
    TEST_ASSERT_TRUE(audio_buffer_is_full(&test_buffer));
    TEST_ASSERT_FALSE(audio_buffer_push(&test_buffer, &d));
}

void test_pop_many_from_empty_buffer_returns_zero(void)
{
    audio_block_t output[2] = {0};

    TEST_ASSERT_EQUAL_UINT32(0U, audio_buffer_pop_many(&test_buffer, output, 2));
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
}

void test_clear_on_empty_buffer_is_noop(void)
{
    audio_buffer_clear(&test_buffer);

    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
    TEST_ASSERT_EQUAL_UINT32(0U, audio_buffer_count(&test_buffer));
}

void test_pop_with_null_output_returns_false(void)
{
    TEST_ASSERT_FALSE(audio_buffer_pop(&test_buffer, NULL));
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
}

void test_peak_returns_next_block_without_consuming_it(void)
{
    audio_block_t block = {{ 10, 20, 30, 40, 50 }};
    audio_block_t peeked = {{ 0 }};

    audio_buffer_push(&test_buffer, &block);
    TEST_ASSERT_TRUE(audio_buffer_peak(&test_buffer, &peeked));
    TEST_ASSERT_EQUAL_INT16(10, peeked.samples[0]);
    TEST_ASSERT_EQUAL_INT16(20, peeked.samples[1]);
    TEST_ASSERT_EQUAL_INT16(30, peeked.samples[2]);
    TEST_ASSERT_EQUAL_INT16(40, peeked.samples[3]);
    TEST_ASSERT_EQUAL_INT16(50, peeked.samples[4]);
    TEST_ASSERT_EQUAL_UINT32(1U, audio_buffer_count(&test_buffer));
}

void test_peak_on_empty_buffer_returns_false(void)
{
    audio_block_t output = {{ 0 }};

    TEST_ASSERT_FALSE(audio_buffer_peak(&test_buffer, &output));
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
}

void test_peak_many_returns_next_blocks_without_consuming_them(void)
{
    audio_block_t first = {{ 1, 2, 3, 4, 5 }};
    audio_block_t second = {{ 10, 20, 30, 40, 50 }};
    audio_block_t third = {{ 100, 200, 300, 400, 500 }};
    audio_block_t output[2] = {0};

    audio_buffer_push(&test_buffer, &first);
    audio_buffer_push(&test_buffer, &second);
    audio_buffer_push(&test_buffer, &third);

    TEST_ASSERT_EQUAL_UINT32(2U, audio_buffer_peak_many(&test_buffer, output, 2));
    TEST_ASSERT_EQUAL_INT16(1, output[0].samples[0]);
    TEST_ASSERT_EQUAL_INT16(10, output[1].samples[0]);
    TEST_ASSERT_EQUAL_UINT32(3U, audio_buffer_count(&test_buffer));
}

void test_push_with_null_block_and_zero_capacity_returns_false(void)
{
    audio_block_t block = {{ 1, 2, 3, 4, 5 }};
    audio_buffer_t zero_buffer;

    audio_buffer_init(&zero_buffer, test_storage, 0U);

    TEST_ASSERT_FALSE(audio_buffer_push(&zero_buffer, &block));
    TEST_ASSERT_FALSE(audio_buffer_push(&test_buffer, NULL));
    TEST_ASSERT_TRUE(audio_buffer_is_empty(&test_buffer));
}
