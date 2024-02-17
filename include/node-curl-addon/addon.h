/**
 *
*/

#ifndef HELLO_ADDON_H_INCLUDED
#define HELLO_ADDON_H_INCLUDED

#include <iostream>

#include <curl/curl.h>

#define NODE_CURL_ADDON_VERSION @PROJECT_VERSION@ // CMake will evaluate this when 'configure_file()' runs!

/** Adapted from https://curl.se/libcurl/c/simple.html */
int node_curl_addon_get(const char* url, const bool& follow);

/** Adapted from https://curl.se/libcurl/c/http-post.html */
int node_curl_addon_get(const char* url, const char* data);

#endif // NODE_CURL_ADDON_H_INCLUDED
