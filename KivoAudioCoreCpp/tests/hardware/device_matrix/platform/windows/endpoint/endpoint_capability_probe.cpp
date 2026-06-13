#include "endpoint_capability_probe.hpp"

#include <Windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>

namespace device_matrix::windows {

using Microsoft::WRL::ComPtr;

bool probe_endpoint_capabilities(
    IMMDevice& device,
    EndpointRecord& record,
    long& platform_code) {
    ComPtr<IAudioClient> client;
    auto code = device.Activate(
        __uuidof(IAudioClient),
        CLSCTX_INPROC_SERVER,
        nullptr,
        reinterpret_cast<void**>(client.GetAddressOf()));
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }

    WAVEFORMATEX* mix_format = nullptr;
    code = client->GetMixFormat(&mix_format);
    if (FAILED(code) || mix_format == nullptr) {
        platform_code = FAILED(code) ? code : E_UNEXPECTED;
        return false;
    }
    record.mix_format = {
        mix_format->wFormatTag,
        mix_format->nChannels,
        mix_format->nSamplesPerSec,
        mix_format->wBitsPerSample,
        mix_format->nBlockAlign
    };

    WAVEFORMATEX* closest = nullptr;
    const auto shared_code = client->IsFormatSupported(
        AUDCLNT_SHAREMODE_SHARED,
        mix_format,
        &closest);
    record.shared_mix_supported = shared_code == S_OK;
    CoTaskMemFree(closest);
    record.exclusive_mix_supported = client->IsFormatSupported(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
        mix_format,
        nullptr) == S_OK;

    code = client->GetDevicePeriod(
        &record.default_period_100ns,
        &record.minimum_period_100ns);
    CoTaskMemFree(mix_format);
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }
    return true;
}

} // namespace device_matrix::windows
