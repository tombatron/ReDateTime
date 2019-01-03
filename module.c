#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "redismodule.h"
#include "rmutil/strings.h"

struct TimeDescription
{
    int weekOfYear;
    int month;
    int day;
    int year;
}

struct TimeDescription GetDateTimeDetails(long epochTime);

struct TimeDescription GetDateTimeDetails(long epochTime)
{
    time_t convertedTime = epochTime;
    struct tm *wt = localtime(&convertedTime);

    struct TimeDescription *td = RedisModule_Alloc(sizeof *x);

    td->weekOfYear = (int)ceil((double)wt->tm_yday / 7.0f);
    td->month = wt->tm_mon + 1;
    td->day = wt->tm_mday;
    td->year = wt->tm_year + 1900;

    return td;
}



int DateTimeInfo(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    const char *sargs[argc];
    RMUtil_StringConvert(argv, sargs, argc, RMUTIL_STRINGCONVERT_COPY);

    time_t parsedSeconds;
    sscanf(sargs[1], "%ld", &parsedSeconds);

    struct tm *parsedTime;
    parsedTime = localtime(&parsedSeconds);

    int weekOfYear = (int)ceil((double)parsedTime->tm_yday / 7.0f);

    RedisModule_ReplyWithArray(ctx, 8);

    char *monthLabelString = "month";
    RedisModuleString *monthLabel = RedisModule_CreateString(ctx, monthLabelString, sizeof(char) * 5);
    RedisModule_ReplyWithString(ctx, monthLabel);

    int monthValueInt = parsedTime->tm_mon + 1;
    RedisModuleString *monthValue = RedisModule_CreateStringPrintf(ctx, "%d", monthValueInt);
    RedisModule_ReplyWithString(ctx, monthValue);

    char *dayLabelString = "day";
    RedisModuleString *dayLabel = RedisModule_CreateString(ctx, dayLabelString, sizeof(char) * 3);
    RedisModule_ReplyWithString(ctx, dayLabel);

    RedisModuleString *dayValue = RedisModule_CreateStringPrintf(ctx, "%d", parsedTime->tm_mday);
    RedisModule_ReplyWithString(ctx, dayValue);    

    char *yearLabelString = "year";
    RedisModuleString *yearLabel = RedisModule_CreateString(ctx, yearLabelString, sizeof(char) * 4);
    RedisModule_ReplyWithString(ctx, yearLabel);

    int yearValueInt = parsedTime->tm_year + 1900;
    RedisModuleString *yearValue = RedisModule_CreateStringPrintf(ctx, "%d", yearValueInt);
    RedisModule_ReplyWithString(ctx, yearValue);

    char *stringWeekOfYearLabel = "week_of_year";
    RedisModuleString *weekOfYearLabel = RedisModule_CreateString(ctx, stringWeekOfYearLabel, sizeof(char) * 12);
    RedisModule_ReplyWithString(ctx, weekOfYearLabel);

    RedisModuleString *weekOfYearValue = RedisModule_CreateStringPrintf(ctx, "%d", weekOfYear);
    RedisModule_ReplyWithString(ctx, weekOfYearValue);

    RedisModule_Free(monthLabel);
    RedisModule_Free(monthValue);

    RedisModule_Free(dayLabel);
    RedisModule_Free(dayValue);

    RedisModule_Free(yearLabel);
    RedisModule_Free(yearValue);

    RedisModule_Free(weekOfYearLabel);
    RedisModule_Free(weekOfYearValue);

    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (RedisModule_Init(ctx, "datetime", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
    {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "datetime.info", DateTimeInfo, "readonly", 1, 1, 1) == REDISMODULE_ERR)
    {
        return REDISMODULE_ERR;
    }

    return REDISMODULE_OK;
}