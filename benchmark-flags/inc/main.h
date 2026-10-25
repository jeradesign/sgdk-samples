#ifndef _MAIN_H_
#define _MAIN_H_


#define ENABLE_ASM

// the tests only call waitMs() to leave sub-test titles / results on screen,
// always outside of the timed sections, so we can safely skip all of them
#define waitMs(ms)      do {} while(0)


void initMemTest();


#endif // _MAIN_H_
