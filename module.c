#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "redismodule.h"
#include "rmutil/strings.h"

int DateTimeInfo(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    const char *sargs[argc];
    RMUtil_StringConvert(argv, sargs, argc, RMUTIL_STRINGCONVERT_COPY);

    time_t parsedSeconds;
    sscanf(sargs[1], "%ld", &parsedSeconds);

    struct tm *parsedTime;
    parsedTime = localtime(&parsedSeconds);

    int weekOfYear = (int)ceil((double)parsedTime->tm_yday / 7.0f);

    RedisModule_ReplyWithArray(ctx, 4);

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