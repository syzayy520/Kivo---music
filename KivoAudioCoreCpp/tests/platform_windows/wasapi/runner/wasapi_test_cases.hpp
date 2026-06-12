#pragma once

struct WasapiTestRunner;

void run_wasapi_format_mapping_tests(WasapiTestRunner& runner);
void run_wasapi_error_mapping_tests(WasapiTestRunner& runner);
void run_wasapi_endpoint_notification_tests(WasapiTestRunner& runner);
void run_wasapi_render_worker_tests(WasapiTestRunner& runner);
