#include "../civetweb/include/civetweb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int hello_handler(struct mg_connection *conn, void *cbdata) {
    const struct mg_request_info *req_info = mg_get_request_info(conn);

    if (strcmp(req_info->request_method, "GET") == 0) {
        mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!");
        return 200;
    }

    return 404;
}

char *render_template(const char *filename, const char *placeholder, const char *replacement) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    char *pos = strstr(buffer, placeholder);
    if (!pos) return buffer;

    size_t new_size = size + strlen(replacement) - strlen(placeholder);
    char *output = malloc(new_size + 1);

    strncpy(output, buffer, pos - buffer);
    output[pos - buffer] = '\0';
    strcat(output, replacement);
    strcat(output, pos + strlen(placeholder));

    free(buffer);
    return output;
}
char user_name[100];

static int file_handler(struct mg_connection *conn, void *cbdata) {
    const char *placeholder = "{{name}}";
    const char *replacement = user_name; // use global variable

    char *html = render_template("./templates/index.html", placeholder, replacement);

    if (!html) {
        mg_printf(conn,
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 21\r\n"
            "\r\n"
            "Template load failed.");
        return 500;
    }

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "\r\n%s",
        strlen(html), html);

    free(html);
    return 200;
}



int main(void) {
    const char *options[] = {
        "listening_ports", "8080",
        0
    };

    printf("Enter your name: ");
    scanf("%99s", user_name); 

    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    struct mg_context *ctx = mg_start(&callbacks, 0, options);
    if (!ctx) {
        printf("Failed to start server\n");
        return 1;
    }

    // Set request handlers
    mg_set_request_handler(ctx, "/hello", hello_handler, 0);
    mg_set_request_handler(ctx, "/", file_handler, 0);

    printf("Server running on http://localhost:8080\n");


    while (1) {

    }

    mg_stop(ctx);
    return 0;
}