#pragma once
struct timeval {long tv_sec;long tv_usec;};
extern long fakeUtc;
inline void gettimeofday(timeval *tv,void *) {tv->tv_sec=fakeUtc;tv->tv_usec=0;}
