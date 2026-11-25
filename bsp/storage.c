#include "sys.h"
#include "storage.h"

#define FLASH_KEYR      REG32(0x40023C04u)
#define FLASH_SR        REG32(0x40023C0Cu)
#define FLASH_CR        REG32(0x40023C10u)

#define STORAGE_BASE    0x0800C000u
#define STORAGE_SECTOR  3u
#define STORAGE_SPAN    0x4000u

#define FLASH_BSY       (1u << 16)
#define FLASH_SER       (1u << 1)
#define FLASH_PG        (1u << 0)
#define FLASH_PSIZE_X32 (2u << 8)
#define FLASH_STRT      (1u << 16)
#define FLASH_LOCK      (1u << 31)
#define FLASH_SR_CLEAR  0x0000F3FAu

typedef struct {
    uint32_t magic;
    uint32_t count;
    sample_t data[STORAGE_SIZE];
} storage_block_t;

#define SLOT_SIZE   ((uint32_t)sizeof(storage_block_t))
#define SLOT_COUNT  (STORAGE_SPAN / SLOT_SIZE)

static const storage_block_t *slot_at(uint32_t index)
{
    return (const storage_block_t *)(STORAGE_BASE + index * SLOT_SIZE);
}

static void flash_unlock(void)
{
    if (FLASH_CR & FLASH_LOCK) {
        FLASH_KEYR = 0x45670123u;
        FLASH_KEYR = 0xCDEF89ABu;
    }
}

static void flash_wait(void)
{
    while (FLASH_SR & FLASH_BSY) {
    }
}

static void flash_erase_sector(uint32_t sector)
{
    flash_wait();
    FLASH_SR = FLASH_SR_CLEAR;
    FLASH_CR = FLASH_SER | (sector << 3);
    FLASH_CR |= FLASH_STRT;
    flash_wait();
    FLASH_CR = 0;
}

static void flash_program_block(uint32_t addr, const storage_block_t *blk)
{
    const uint32_t *src = (const uint32_t *)blk;
    uint32_t words = SLOT_SIZE / 4u;

    for (uint32_t i = 0; i < words; i++) {
        flash_wait();
        FLASH_CR = FLASH_PG | FLASH_PSIZE_X32;
        *(volatile uint32_t *)(addr + i * 4u) = src[i];
        flash_wait();
    }

    FLASH_CR = 0;
}

void storage_load(sample_t *buf, uint32_t *count)
{
    const storage_block_t *last = 0;

    *count = 0;

    for (uint32_t i = 0; i < SLOT_COUNT; i++) {
        const storage_block_t *blk = slot_at(i);

        if (blk->magic == STORAGE_MAGIC && blk->count <= STORAGE_SIZE) {
            last = blk;
        }
    }

    if (last == 0) {
        storage_clear();
        return;
    }

    for (uint32_t i = 0; i < STORAGE_SIZE; i++) {
        buf[i] = last->data[i];
    }

    *count = last->count;
}

int storage_save(const sample_t *buf, uint32_t count)
{
    storage_block_t blk;
    uint32_t slot = SLOT_COUNT;

    for (uint32_t i = 0; i < SLOT_COUNT; i++) {
        if (slot_at(i)->magic == 0xFFFFFFFFu) {
            slot = i;
            break;
        }
    }

    if (count > STORAGE_SIZE) {
        count = STORAGE_SIZE;
    }

    blk.magic = STORAGE_MAGIC;
    blk.count = count;

    for (uint32_t i = 0; i < STORAGE_SIZE; i++) {
        blk.data[i] = buf[i];
    }

    flash_unlock();

    if (slot >= SLOT_COUNT) {
        flash_erase_sector(STORAGE_SECTOR);
        slot = 0;
    }

    flash_program_block(STORAGE_BASE + slot * SLOT_SIZE, &blk);

    FLASH_CR |= FLASH_LOCK;

    return 0;
}

void storage_clear(void)
{
    flash_unlock();
    flash_erase_sector(STORAGE_SECTOR);
    FLASH_CR |= FLASH_LOCK;
}
