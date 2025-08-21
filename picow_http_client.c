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

static int keep_alive_result_fn(EXAMPLE_HTTP_REQUEST_T *req, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    char *data = "hello, i'm some new posted data";
    u16_t data_len = strlen(data) * sizeof(char);

    httpc_post_next(POST_REQUEST, data_len, data, &req->settings, req->http_state);

    // dont keep the connection open after the second post
    return 0;
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

    int post_result = http_client_post_request_sync(cyw43_arch_async_context(), &req1, data_len, data);

    // wait a bit to allow second request to come through
    sleep_ms(1000);

    cyw43_arch_deinit();
    printf("done.\n");
    return 0;
}