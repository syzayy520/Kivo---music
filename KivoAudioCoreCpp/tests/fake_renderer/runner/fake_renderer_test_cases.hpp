#pragma once

struct FakeRendererTestRunner;

void run_manual_clock_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_lifecycle_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_write_generation_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_fault_injection_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_device_lifecycle_fault_tests(
    FakeRendererTestRunner& runner);
void run_fake_renderer_observation_history_tests(FakeRendererTestRunner& runner);
