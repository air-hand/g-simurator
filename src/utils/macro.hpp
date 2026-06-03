#pragma once

#define DELETE_COPY_AND_ASSIGN(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete

#define DELETE_MOVE_AND_ASSIGN(Class) \
    Class(Class&&) = delete; \
    Class& operator=(Class&&) = delete

#define DECLARE_MOVE_CONSTRUCTOR(Class) \
    Class(Class&&) noexcept; \
    Class& operator=(Class&&) noexcept

#define DECLARE_MOVE_CONSTRUCTOR_DEFAULT(Class) \
    Class(Class&&) noexcept = default; \
    Class& operator=(Class&&) noexcept = default
