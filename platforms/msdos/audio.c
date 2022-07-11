#include <stdint.h>
#include <stdlib.h>

static union {
    uint32_t dword;
    struct {
        uint16_t low_word, high_word;
    };
} orig_bios_irq0_isr;
static uint16_t sample_rate_counter;

static inline void outb(uint16_t port, uint8_t value)
{
    asm __volatile("outb %0, %1" : : "Ral"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    asm __volatile("inb %1, %0" : "=Ral"(value) : "Nd"(port));
    return value;
}

static inline void audio_set_sample_rate(const uint16_t sample_rate)
{
    uint32_t corrected = sample_rate ? (0x1234dc / (uint32_t)sample_rate) : 0;

    sample_rate_counter = corrected & 0xfffe;

    asm __volatile("cli");
    outb(0x43, 0x36);
    outb(0x40, sample_rate_counter & 0xff);
    outb(0x40, sample_rate_counter >> 8);
    asm __volatile("sti");
}

static inline void audio_play_sample(uint8_t sample)
{
    outb(0x43, 0xb0);
    outb(0x42, sample >> 1);
    outb(0x42, 0);
}

__attribute__((used)) static void audio_tick(void)
{
    static uint16_t tick;

    audio_play_sample(rand() & 255);

    tick += sample_rate_counter;

    if (sample_rate_counter > tick) {
        asm __volatile("pushf");
        asm __volatile("lcall *(%0)" : : "b"(&orig_bios_irq0_isr));
    } else {
        outb(0x20, 0x20); /* acknowledge IRQ */
    }
}

void audio_tick_isr(void);
__asm__(".global audio_tick_isr\n"
        "audio_tick_isr:\n"
        "pusha\n"
        "call audio_tick\n"
        "popa\n"
        "iret\n");

void audio_init(void)
{
    /* Save original BIOS IRQ0 handler */
    asm __volatile("int $0x21\n"
                   : "=b"(orig_bios_irq0_isr.low_word),
                     "=e"(orig_bios_irq0_isr.high_word)
                   : "a"(0x3508)
                   : "cc", "memory");

    /* Replace it with ours */
    asm __volatile("int $0x21"
                   :
                   : "d"(audio_tick_isr), "a"(0x2508)
                   : "cc", "memory");

    /* Connect speaker to PIT channel 2 */
    outb(0x61, inb(0x61) | 0x3);

    /* Reprogram PIT Channel 0 to fire IRQ0 at 16KHz */
    audio_set_sample_rate(16000);
}

void audio_shutdown(void)
{
    audio_set_sample_rate(0);

    /* Restore original BIOS IRQ0 routine */
    asm __volatile("int $0x21\n"
                   :
                   : "Rds"(orig_bios_irq0_isr.high_word),
                     "d"(orig_bios_irq0_isr.low_word), "a"(0x2508)
                   : "cc", "memory");

    outb(0x61, inb(0x61) & 0xfc);
}
