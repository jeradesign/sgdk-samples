#include <genesis.h>

#include "res/gfx.h"


// Compiler flags comparison benchmark (derived from 'benchmark' sample).
// Runs only the tests that exercise C code (memset / memcpy are pure assembly)
// back to back without intermediate screens, then displays all scores on a single screen.
// Build label can be set at build time with EXTRA_FLAGS="... -DBENCH_LABEL=xxx"

#define SGDK_BENCHMARK      "SGDK flag bench v1.0"

#ifndef BENCH_LABEL
#define BENCH_LABEL         default
#endif

#define STRINGIFY_(x)       #x
#define STRINGIFY(x)        STRINGIFY_(x)

#define MAX_TEST            9
#define MAX_SUBTEST         16
#define SCORE_PER_ROW       6


typedef u16 TestFunc(u16 *scores);

typedef struct
{
    const char *name;       // name in totals section (max 8 chars)
    const char *tag;        // tag in details section (3 chars)
    TestFunc *func;
    u16 numSubTest;
} TestDef;


// extern
u16 executeMemAllocTest(u16 *scores);
u16 executeVRamAllocTest(u16 *scores);
u16 executeMathsBasicTest(u16 *scores);
u16 executeMathsAdvTest(u16 *scores);
u16 executeStrSortTest(u16 *scores);
u16 executeBGTest(u16 *scores);
u16 executeBMPTest(u16 *scores);
u16 executeMapTest(u16 *scores);
u16 executeSpritesTest(u16 *scores);


// quick tests first, long time based tests (scrolling / sprites) last
static const TestDef tests[MAX_TEST] =
{
    { "MemAlloc", "MEM", executeMemAllocTest, 6 },
    { "VRAM",     "VRM", executeVRamAllocTest, 8 },
    { "MathBas",  "MTB", executeMathsBasicTest, 6 },
    { "MathAdv",  "MTA", executeMathsAdvTest, 7 },
    { "StrSort",  "STR", executeStrSortTest, 6 },
    { "BG",       "BG ", executeBGTest, 10 },
    { "Bitmap",   "BMP", executeBMPTest, 15 },
    { "Scroll",   "MAP", executeMapTest, 12 },
    { "Sprite",   "SPR", executeSpritesTest, 13 }
};

u16 detailledScores[MAX_TEST][MAX_SUBTEST];
u16 scores[MAX_TEST];


// forward
static void showResults(u32 globalScore);


int main()
{
    u32 globalScore;

    while(TRUE)
    {
        memset(scores, 0, sizeof(scores));
        memset(detailledScores, 0, sizeof(detailledScores));
        globalScore = 0;

        for(u16 i = 0; i < MAX_TEST; i++)
        {
            scores[i] = tests[i].func(detailledScores[i]);
            globalScore += scores[i];

            // restore text display state for next test
            VDP_clearPlane(BG_A, TRUE);
            PAL_setColor(15, 0xEEE);
        }

        showResults(globalScore);

        JOY_waitPress(JOY_1, BUTTON_START);
    }
}


static void showResults(u32 globalScore)
{
    char str[48];
    u16 y;

    VDP_resetScreen();
    PAL_setColor(0, 0x000);
    PAL_setColor(15, 0xEEE);

    VDP_drawText(SGDK_BENCHMARK, 0, 0);
    VDP_drawText("Build: " STRINGIFY(BENCH_LABEL), 0, 1);
    VDP_drawText("GCC " __VERSION__, 0, 2);

    // totals, 2 per row
    y = 4;
    for(u16 i = 0; i < MAX_TEST; i++)
    {
        sprintf(str, "%-8s %5u", tests[i].name, scores[i]);
        VDP_drawText(str, (i & 1) * 20, y);
        if (i & 1) y++;
    }
    y++;
    sprintf(str, "TOTAL    %lu", globalScore);
    VDP_drawText(str, 0, y);

    // detailled scores, SCORE_PER_ROW per row
    y += 2;
    for(u16 i = 0; i < MAX_TEST; i++)
    {
        const u16 *s = detailledScores[i];
        u16 n = tests[i].numSubTest;
        u16 j = 0;

        while(j < n)
        {
            char *dst = str;

            dst += sprintf(dst, "%s ", (j == 0)?tests[i].tag:"   ");
            for(u16 k = 0; (k < SCORE_PER_ROW) && (j < n); k++, j++)
                dst += sprintf(dst, "%5u ", s[j]);

            VDP_drawText(str, 0, y++);
        }
    }

    VDP_drawText("START: run again", 24, 27);
}
