#pragma once

#define DELETE_COPY_AND_ASSIGN(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete
