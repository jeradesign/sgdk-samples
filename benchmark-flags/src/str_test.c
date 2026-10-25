#include <genesis.h>

#include "inc/main.h"


#define SORT_LEN        256
#define SORT_PASS       20


// forward
static u16 getScore(u32 op, fix32 time);

// results are accumulated here so compiler can't remove the calls
vu32 strTestSink;

static u16 srcU16[SORT_LEN];
static u16 dataU16[SORT_LEN];
static s32 srcS32[SORT_LEN];
static s32 dataS32[SORT_LEN];


u16 executeStrSortTest(u16 *scores)
{
    fix32 start;
    fix32 end;
    u16 i;
    u16 *score;
    u16 globalScore;
    char str[64];

    score = scores;
    globalScore = 0;

    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("String & sort test...", 2, 0);

    setRandomSeed(0x1234);
    for(i = 0; i < SORT_LEN; i++)
    {
        srcU16[i] = random();
        srcS32[i] = ((s32) random() << 16) | random();
    }

    // sprintf with mixed format
    i = 2000;
    start = getTimeAsFix32(FALSE);
    while(i--)
    {
        sprintf(str, "%d %u %04X %s %c", (s16) i - 1000, i, i, "text", 'A' + (i & 15));
        strTestSink += str[0];
    }
    end = getTimeAsFix32(FALSE);
    *score = getScore(2000, end - start);
    globalScore += *score++;

    // fix32ToStr
    i = 3000;
    start = getTimeAsFix32(FALSE);
    while(i--)
    {
        fix32ToStr(FIX32(i) + (i << 3), str, 3);
        strTestSink += str[0];
    }
    end = getTimeAsFix32(FALSE);
    *score = getScore(3000, end - start);
    globalScore += *score++;

    // intToStr
    i = 5000;
    start = getTimeAsFix32(FALSE);
    while(i--)
    {
        intToStr(((s32) i * 997) - 2000000, str, 1);
        strTestSink += str[0];
    }
    end = getTimeAsFix32(FALSE);
    *score = getScore(5000, end - start);
    globalScore += *score++;

    // intToHex
    i = 5000;
    start = getTimeAsFix32(FALSE);
    while(i--)
    {
        intToHex(((u32) i * 0x10001) ^ 0x5A5A5A5A, str, 8);
        strTestSink += str[0];
    }
    end = getTimeAsFix32(FALSE);
    *score = getScore(5000, end - start);
    globalScore += *score++;

    // qsort u16 (score in sorted elements / s, includes refill copy)
    i = SORT_PASS;
    start = getTimeAsFix32(FALSE);
    while(i--)
    {
        memcpy(dataU16, srcU16, sizeof(dataU16));
        qsort_u16(dataU16, 0, SORT_LEN - 1);
        strTestSink += dataU16[SORT_LEN / 2];
    }
    end = getTimeAsFix32(FALSE);
    *score = getScore(SORT_PASS * SORT_LEN, end - start);
    globalScore += *score++;

    // qsort s32 (score in sorted elements / s, includes refill copy)
    i = SORT_PASS;
    start = getTimeAsFix32(FALSE);
    while(i--)
    {
        memcpy(dataS32, srcS32, sizeof(dataS32));
        qsort_s32(dataS32, 0, SORT_LEN - 1);
        strTestSink += dataS32[SORT_LEN / 2];
    }
    end = getTimeAsFix32(FALSE);
    *score = getScore(SORT_PASS * SORT_LEN, end - start);
    globalScore += *score++;

    VDP_clearPlane(BG_A, TRUE);

    return globalScore;
}

// return speed in op/s (saturated to u16)
static u16 getScore(u32 op, fix32 time)
{
    if (time <= 0) return 0xFFFF;

    u32 speed = F32_toRoundedInt(F32_div(FIX32(op), time));

    return (speed > 0xFFFF)?0xFFFF:speed;
}
