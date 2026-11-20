#pragma once
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <string.h>
#include <strings.h>

#define MELODICA_ACCESS_SIZE 4096

/* libcurl passes individual header lines which need not be NUL terminated. */
static inline size_t MelodicaReadAccess(char *line, size_t size, size_t count, void *destination) {
    const size_t length = size * count;
    const char field[] = "x-melodica-access:";
    const size_t prefix = sizeof(field) - 1;
    if (length < prefix || strncasecmp(line, field, prefix) != 0) return length;
    const char *start = line + prefix;
    const char *end = line + length;
    while (start < end && isspace((unsigned char)*start)) ++start;
    while (end > start && isspace((unsigned char)end[-1])) --end;
    char *access = destination;
    size_t bytes = (size_t)(end - start);
    if (bytes >= MELODICA_ACCESS_SIZE || memchr(start, '\r', bytes) || memchr(start, '\n', bytes)) bytes = 0;
    memcpy(access, start, bytes);
    access[bytes] = '\0';
    return length;
}

/* Preserve the configured tuner for explicitly selected partners and local servers. */
static inline int MelodicaTunerUrl(char *url, size_t length, const char *host,
        const char *port, const char *path, bool secure, const char *partner,
        const char **relay_query) {
    const bool standard = strcmp(host, "tuner.pandora.com") == 0 && partner && strcmp(partner, "android") == 0;
    *relay_query = standard ? strchr(path, '?') : NULL;
    if (*relay_query) return snprintf(url, length, "https://spacegho.st/melodica/api/%s", *relay_query);
    return snprintf(url, length, "%s://%s:%s%s", secure ? "https" : "http", host, secure ? port : "80", path);
}

static inline struct curl_slist *MelodicaRequestHeaders(CURL *http, struct curl_slist *headers,
        const char *query, char *access) {
    if (!query) return headers;
    const char login[] = "?method=auth.userLogin&";
    if (strncmp(query, login, sizeof(login) - 1) == 0) {
        access[0] = '\0';
        curl_easy_setopt(http, CURLOPT_HEADERFUNCTION, MelodicaReadAccess);
        curl_easy_setopt(http, CURLOPT_HEADERDATA, access);
    } else if (*access) {
        char header[MELODICA_ACCESS_SIZE + sizeof("x-melodica-access: ")];
        snprintf(header, sizeof(header), "x-melodica-access: %s", access);
        struct curl_slist *extended = curl_slist_append(headers, header);
        if (extended) headers = extended;
    }
    return headers;
}
