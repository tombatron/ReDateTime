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
    long epoch;
};

struct TimeDescription GetDateTimeDetails(long epochTime);
int DisplayTimeDescription(RedisModuleCtx *ctx, struct TimeDescription td);

struct TimeDescription GetDateTimeDetails(long epochTime)
{
    time_t convertedTime = epochTime;
    struct tm *wt = localtime(&convertedTime);

    struct TimeDescription td;

    td.epoch = epochTime;
    td.weekOfYear = (int)ceil((double)wt->tm_yday / 7.0f);
    td.month = wt->tm_mon + 1;
    td.day = wt->tm_mday;
    td.year = wt->tm_year + 1900;

    return td;
}

int DisplayTimeDescription(RedisModuleCtx *ctx, struct TimeDescription td)
{
    RedisModule_ReplyWithArray(ctx, 8);

    char *monthLabelString = "month";
    RedisModuleString *monthLabel = RedisModule_CreateString(ctx, monthLabelString, sizeof(char) * 5);
    RedisModule_ReplyWithString(ctx, monthLabel);

    RedisModuleString *monthValue = RedisModule_CreateStringPrintf(ctx, "%d", td.month);
    RedisModule_ReplyWithString(ctx, monthValue);

    char *dayLabelString = "day";
    RedisModuleString *dayLabel = RedisModule_CreateString(ctx, dayLabelString, sizeof(char) * 3);
    RedisModule_ReplyWithString(ctx, dayLabel);

    RedisModuleString *dayValue = RedisModule_CreateStringPrintf(ctx, "%d", td.day);
    RedisModule_ReplyWithString(ctx, dayValue);    

    char *yearLabelString = "year";
    RedisModuleString *yearLabel = RedisModule_CreateString(ctx, yearLabelString, sizeof(char) * 4);
    RedisModule_ReplyWithString(ctx, yearLabel);

    RedisModuleString *yearValue = RedisModule_CreateStringPrintf(ctx, "%d", td.year);
    RedisModule_ReplyWithString(ctx, yearValue);

    char *stringWeekOfYearLabel = "week_of_year";
    RedisModuleString *weekOfYearLabel = RedisModule_CreateString(ctx, stringWeekOfYearLabel, sizeof(char) * 12);
    RedisModule_ReplyWithString(ctx, weekOfYearLabel);

    RedisModuleString *weekOfYearValue = RedisModule_CreateStringPrintf(ctx, "%d", td.weekOfYear);
    RedisModule_ReplyWithString(ctx, weekOfYearValue);

    return REDISMODULE_OK;
}

int DateTimeInfo(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    RedisModule_AutoMemory(ctx);

    const char *sargs[argc];
    RMUtil_StringConvert(argv, sargs, argc, RMUTIL_STRINGCONVERT_COPY);

    long timeStamp;
    sscanf(sargs[1], "%ld", &timeStamp);

    struct TimeDescription td = GetDateTimeDetails(timeStamp);

    return DisplayTimeDescription(ctx, td);
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