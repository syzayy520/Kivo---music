#pragma once

struct FfmpegDecodeTestRunner;

void run_ffmpeg_decode_fixture_tests(FfmpegDecodeTestRunner& runner);
void run_ffmpeg_policy_and_corruption_tests(FfmpegDecodeTestRunner& runner);
void run_ffmpeg_header_corruption_tests(FfmpegDecodeTestRunner& runner);
void run_ffmpeg_trailing_junk_tests(FfmpegDecodeTestRunner& runner);
void run_ffmpeg_seek_flush_tests(FfmpegDecodeTestRunner& runner);
void run_ffmpeg_truncated_media_tests(FfmpegDecodeTestRunner& runner);
void run_ffmpeg_unsupported_cue_tests(FfmpegDecodeTestRunner& runner);
