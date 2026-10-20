#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <stdint.h>

std::string currentTime();
std::string currentTimeMs();
int64_t timestamp();
int64_t timestamp_micro();
bool isQtCreatorParentProc();

#endif // COMMON_H
