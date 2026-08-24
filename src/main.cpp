// Minimal Vita app that starts civetweb (HTTP + WebSocket) and serves ux0:data/htmlui
// NOTE: This is a focused example (illustrative); you may need to adapt paths/build flags to your template.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <atomic>
#include <vector>

#include "civetweb.h" // from civetweb source. Put civetweb.c/.h in src/civetweb/

// For Vita, you may need to include and link Vita SDK libs (network) per the Makefile/template.
// This program starts a civetweb server serving UX0_DATA_DIR and registers:
//  - HTTP POST endpoint: /api/native
//  - WebSocket endpoint: /ws

static std::atomic<bool> running(true);

static const char *DOCUMENT_ROOT = "/ux0/data/htmlui"; // where you'll copy index.html etc
static const char *LISTENING = "127.0.0.1:8080";

static int api_handler(struct mg_connection *conn, void *cbdata) {
    const struct mg_request_info *ri = mg_get_request_info(conn);
    // Only accept POST
    if (strcmp(ri->request_method, "POST") != 0) {
        mg_printf(conn,
                  "HTTP/1.1 405 Method Not Allowed\r\n"
                  "Content-Type: text/plain\r\n"
                  "Connection: close\r\n\r\n"
                  "Method Not Allowed");
        return 1;
    }

    long long cl = 0;
    const char *cl_hdr = mg_get_header(conn, "Content-Length");
    if (cl_hdr) cl = atoll(cl_hdr);

    std::vector<char> body;
    if (cl > 0) {
        body.resize((size_t)cl + 1);
        mg_read(conn, body.data(), (size_t)cl);
        body[(size_t)cl] = '\0';
    } else {
        // read until EOF (fallback)
        char buf[1024];
        int n;
        while ((n = mg_read(conn, buf, sizeof(buf))) > 0) {
            body.insert(body.end(), buf, buf + n);
        }
        body.push_back('\0');
    }

    // Very simple JSON-like handling (you can use a real JSON library)
    printf("[native] /api/native POST body: %s\n", body.data());

    // Example: echo back a simple JSON response
    const char *response = "{\"status\":\"ok\",\"message\":\"native received\"}";
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: application/json\r\n"
              "Connection: close\r\n"
              "Content-Length: %zu\r\n\r\n%s",
              strlen(response), response);
    return 1;
}

static int websocket_connect(const struct mg_connection *conn, void *cbdata) {
    printf("[native] websocket_connect\n");
    return 0; // return 0 to accept
}

static void websocket_ready(struct mg_connection *conn, void *cbdata) {
    printf("[native] websocket_ready\n");
    // Optionally send a welcome message
    const char *w = "{\"type\":\"welcome\",\"msg\":\"hello from native\"}";
    mg_websocket_write(conn, WEBSOCKET_OPCODE_TEXT, w, strlen(w));
}

static int websocket_data(struct mg_connection *conn, int flags, char *data, size_t data_len, void *cbdata) {
    // data is NOT null-terminated
    std::string s(data, data + data_len);
    printf("[native] websocket_data: %s\n", s.c_str());

    // Echo an acknowledgment back
    std::string resp = "{\"type\":\"ack\",\"received\":" + std::to_string((int)data_len) + "}";
    mg_websocket_write(conn, WEBSOCKET_OPCODE_TEXT, resp.c_str(), resp.size());
    return 1;
}

int main(int argc, char *argv[]) {
    printf("Starting htmlui-vita server\n");
    // Civetweb options
    const char *options[] = {
        "document_root", DOCUMENT_ROOT,
        "listening_ports", LISTENING,
        "enable_directory_listing", "no",
        "num_threads", "2",
        NULL
    };

    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    struct mg_context *ctx = mg_start(&callbacks, NULL, options);
    if (!ctx) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    // Register HTTP handler for API
    mg_set_request_handler(ctx, "/api/native", api_handler, NULL);

    // Register websocket handlers at /ws
    mg_set_websocket_handler(ctx, "/ws",
                             websocket_connect, websocket_ready, websocket_data, NULL);

    printf("Server started at http://%s/\n", LISTENING);
    printf("Copy your UI to %s and open the Vita browser at that URL.\n", DOCUMENT_ROOT);

    // Run until the app is terminated. Simpler apps just sleep.
    while (running.load()) {
        sceKernelDelayThread(1000 * 1000); // 1 second
        // You can insert code to check for button presses to set running=false
    }

    mg_stop(ctx);
    printf("Server stopped\n");
    return 0;
}
