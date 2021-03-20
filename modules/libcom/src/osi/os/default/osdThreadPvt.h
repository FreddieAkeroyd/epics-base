#ifndef osdThreadPvth
#define osdThreadPvth

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    HANDLE timer;
    int high_res_timer;    
} osdThreadTimerInfo;

extern void osdThreadGetTimer(osdThreadTimerInfo* info);

#ifdef __cplusplus
}
#endif

#endif /* osdThreadPvth */
