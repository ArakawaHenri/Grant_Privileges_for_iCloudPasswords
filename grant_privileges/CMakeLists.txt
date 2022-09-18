cmake_minimum_required (VERSION 3.23)

add_executable (grant_privileges "grant_privileges.cpp" "grant_privileges.h")

set_property(TARGET grant_privileges APPEND_STRING PROPERTY LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator'\"")
set_property(TARGET grant_privileges PROPERTY CXX_STANDARD 23)