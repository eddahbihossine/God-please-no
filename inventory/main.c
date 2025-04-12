#include "../civetweb/include/civetweb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static int file_handler(struct mg_connection *conn, void *cbdata) {
    const struct mg_request_info *req_info = mg_get_request_info(conn);
    const char *filename = strcmp(req_info->local_uri, "./style/style.css") == 0 ? "./style/style.css" : "./templates/index.html";
    const char *content_type = strstr(filename, ".css") ? "text/css" : "text/html";

    FILE *file = fopen(filename, "r");
    if (!file) {
        mg_printf(conn,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "404 Not Found");
        return 404;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size);
    fread(buffer, 1, file_size, file);
    fclose(file);

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", content_type, file_size);
    mg_write(conn, buffer, file_size);

    free(buffer);
    return 200;
}

int main(void) {
    const char *options[] = {
        "listening_ports", "8080",
        0
    };

    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    struct mg_context *ctx = mg_start(&callbacks, 0, options);
    if (!ctx) {
        printf("Failed to start server\n");
        return 1;
    }

    mg_set_request_handler(ctx, "/hello", hello_handler, 0);
    mg_set_request_handler(ctx, "/", file_handler, 0);
    mg_set_request_handler(ctx, "/", file_handler, 0);
    mg_set_request_handler(ctx, "./styles/style.css", file_handler, 0);


    printf("Server running on http://localhost:8080\n");
    getchar(); 
    mg_stop(ctx);
    return 0;
}
