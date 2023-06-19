// Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
// Please refer to the license text that comes with this tendis open source
// project for additional information.

#ifndef SRC_TENDISPLUS_COMMANDS_RELEASE_H_
#define SRC_TENDISPLUS_COMMANDS_RELEASE_H_

#define TENDISPLUS_GIT_SHA1 "97bf3255"
#define TENDISPLUS_GIT_DIRTY "100"
#define TENDISPLUS_BUILD_ID "u20-1687010968"

#include <stdint.h>
uint64_t redisBuildId(void);

#endif  // SRC_TENDISPLUS_COMMANDS_RELEASE_H_
