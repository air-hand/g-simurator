#pragma once

#define DELETE_COPY_AND_ASSIGN(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete


#ifdef UTILS_LIB_BUILD
#define UTILS_EXPORT __declspec(dllexport)
#else
#define UTILS_EXPORT __declspec(dllimport)
#endif
