#pragma once

#define DELETE_COPY_AND_ASSIGN(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete

#define DELETE_MOVE_AND_ASSIGN(Class) \
    Class(Class&&) = delete; \
    Class& operator=(Class&&) = delete
