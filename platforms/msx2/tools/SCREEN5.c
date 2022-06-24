#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <alloca.h>

#ifdef XPM_FILE
# include XPM_FILE
#else
# include "mines.xpm"
# define XPM_FILE mines_xpm
#endif


#define MAX_COLORS 16
#define SCREEN5 "SCREEN5"

struct palette {
    int8_t ci;
    uint8_t r;
    uint8_t rr;
    uint8_t g;
    uint8_t gg;
    uint8_t b;
    uint8_t bb;
    const char* s;
};


void register_color(struct palette* palette, int index, int8_t hw_color, int cpp)
{
    unsigned int rr, gg, bb;
    const char* s = XPM_DATA[index + 1];

    sscanf(&s[cpp], "\tc #%02x%02x%02x", &rr, &gg, &bb);
    rr = rr & 0xff;
    gg = gg & 0xff;
    bb = bb & 0xff;

    palette[index].ci = hw_color;
    palette[index].r = (uint8_t) ((double) rr) * 7 / 0xff;
    palette[index].g = (uint8_t) ((double) gg) * 7 / 0xff;
    palette[index].b = (uint8_t) ((double) bb) * 7 / 0xff;
    palette[index].s = s;

    palette[index].rr = rr;
    palette[index].gg = gg;
    palette[index].bb = bb;
}


int8_t find_color(struct palette* palette, int colors, const char* const pos, int cpp)
{
    static int8_t cached_color = 0;

    if (palette[cached_color].ci && strncmp(pos, palette[cached_color].s, cpp) == 0) {
        return palette[cached_color].ci;
    }

    for (int8_t i = 0; i < colors; ++i) {
        if (palette[i].ci && strncmp(pos, palette[i].s, cpp) == 0) {
            cached_color = i;
            return palette[cached_color].ci;
        }
    }

    fprintf(stderr, SCREEN5 ": color index '%.*s' unknown\n", cpp, pos);
    exit(-5);
}


bool is_used_color(const char* const cs, int cpp, int colors, int width, int height)
{
    for (int y = 0; y < height ; ++y) {
        const char* pos = XPM_DATA[y + colors + 1];

        for (int x = 0; x < width; x++) {
            if (strncmp(pos, cs, cpp) == 0) {
                return true;
            }
            pos += cpp;
        }
    }

    return false;
}


int main(int argc, char **argv)
{
    int width;
    int height;
    int colors;
    int used_colors;
    int cpp;

    if (argc < 2) {
        fprintf(stderr, SCREEN5 ": expects name of image array\n");
        exit(-1);
    }

    sscanf(XPM_DATA[0], "%d %d %d %d", &width, &height, &colors, &cpp);
    if (width & 1) {
        fprintf(stderr, SCREEN5 ": expects even width\n");
        exit(-2);
    }
    if (width > 256) {
        fprintf(stderr, SCREEN5 ": expects maximum width of 256 pixels\n");
        exit(-3);
    }

    struct palette* palette = alloca(sizeof(struct palette) * colors);
    memset(palette, 0, sizeof(struct palette) * colors);
    used_colors = 0;

    /* find used colors first */
    for (int c = 0; c < colors; ++c) {
        char* s = XPM_DATA[c + 1];
        fprintf(stderr, "color [%s]\n", s);
        if (is_used_color(s, cpp, colors, width, height) != false) {
            ++used_colors;
            fprintf(stderr, "pixel '%.*s' registered as color #%i from table at %i'\n",
                    cpp, s, used_colors, c + 1);
            register_color(palette, c, used_colors, cpp);
        }
    }
    if (used_colors >= MAX_COLORS) {
        fprintf(stderr, SCREEN5 ": expects max of 15 used colors\n");
        exit(-4);
    }

    printf("#include <stdint.h>\n\n");
    printf("#define %s_WIDTH %u\n", argv[1], width);
    printf("#define %s_HEIGHT %u\n\n", argv[1], height);
    printf("static const uint8_t %s_palette[] = {\n", argv[1]);

    for (int8_t i = 0; i <= colors; ++i) {
        if (palette[i].ci) {
            printf("\t%2i, %u,%u,%u, /* 0x%02X, 0x%02X, 0x%02X */\n",
                   palette[i].ci,
                   palette[i].r, palette[i].g, palette[i].b,
                   palette[i].rr, palette[i].gg, palette[i].bb);
        }
    }

    printf("};\n\nstatic const uint8_t %s_data[] = {\n\t", argv[1]);
    unsigned int pos = 0;

    for (int y = 0; y < height ; ++y) {
        const char* line = XPM_DATA[y + colors + 1];

        for (int x = 0; x < width; x += 2) {
            uint8_t pixel1, pixel2;

            if (pos > 0 && pos % 24 == 0) printf("\n\t");

            pixel1 = find_color(palette, colors, line, cpp);
            line += cpp;

            pixel2 = find_color(palette, colors, line, cpp);
            line += cpp;

            printf("0x%02X,", (pixel1 << 4) | pixel2);

        pos += 2;
        }
    }

    if (pos > 0) printf("\n");
    printf("};\n\n");

    printf("#define %s_SIZE %u\n", argv[1], pos / 2);
}

