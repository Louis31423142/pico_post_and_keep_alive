/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"

#define HOST "10.3.195.248"
#define GET_REQUEST "/get_button"
#define POST_REQUEST "/post_button"

// Make the next request
static void request_worker_fn(async_context_t *context, async_at_time_worker_t *worker) {
    char *data = "hello, i'm some new posted data";
    u16_t data_len = strlen(data) * sizeof(char);

    EXAMPLE_HTTP_REQUEST_T *req = (EXAMPLE_HTTP_REQUEST_T*)worker->user_data;
    httpc_post_next(POST_REQUEST, data_len, data, &req->settings, req->http_state);
}

static async_at_time_worker_t request_worker;

static int keep_alive_result_fn(EXAMPLE_HTTP_REQUEST_T *req, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    printf("keep alive fn called!\n");

    char *data = "hello, i'm some new posted data";
    u16_t data_len = strlen(data) * sizeof(char);
    //httpc_post_next(POST_REQUEST, data_len, data, &req->settings, req->http_state);

    /*
    async_context_remove_at_time_worker(req->context, &request_worker);
    if (!err && httpc_result == HTTPC_RESULT_OK && (srv_res >= 200 && srv_res <= 299)) {
        // repeat the request in 5s using the same connection
        request_worker.user_data = req;
        request_worker.do_work = request_worker_fn;
        async_context_add_at_time_worker_in_ms(req->context, &request_worker, 5000);
        return 1; // expect more
    }
    return 0; // stop  
    */

    // i think this keeps the connection alive
    httpc_post_next(POST_REQUEST, data_len, data, &req->settings, req->http_state);
    return 1;
}

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect\n");
        return 1;
    }

    // Do a post request, then keep alive
    EXAMPLE_HTTP_REQUEST_T req1 = {0};
    req1.hostname = HOST;
    req1.url = POST_REQUEST;
    req1.headers_fn = http_client_header_print_fn;
    req1.recv_fn = http_client_receive_print_fn;
    req1.port = 5000;
    req1.result_fn = keep_alive_result_fn;

    char *data = "hello, i'm some posted data";
    u16_t data_len = strlen(data) * sizeof(char);

    int get_result = http_client_post_request_sync(cyw43_arch_async_context(), &req1, data_len, data);

    sleep_ms(5000);

    int keep_alive = httpc_post_next(POST_REQUEST, data_len, data, &req1.settings, req1.http_state);

    cyw43_arch_deinit();
    printf("done.\n");
    return 0;
}