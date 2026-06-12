#include "endpoint_inventory.hpp"

#include <array>
#include <string>
#include <utility>

#include <mmdeviceapi.h>
#include <wrl/client.h>

#include "../apartment/com_apartment.hpp"
#include "endpoint_probe.hpp"

namespace device_matrix::windows {

namespace {

using Microsoft::WRL::ComPtr;

[[nodiscard]] std::wstring default_endpoint_id(
    IMMDeviceEnumerator& enumerator,
    ERole role) {
    ComPtr<IMMDevice> endpoint;
    if (FAILED(enumerator.GetDefaultAudioEndpoint(
            eRender,
            role,
            endpoint.GetAddressOf()))) {
        return {};
    }
    LPWSTR value = nullptr;
    if (FAILED(endpoint->GetId(&value))) {
        return {};
    }
    std::wstring result{value};
    CoTaskMemFree(value);
    return result;
}

} // namespace

DeviceInventory enumerate_active_render_endpoints() {
    DeviceInventory result;
    ComApartment apartment;
    if (FAILED(apartment.result())) {
        result.platform_code = apartment.result();
        return result;
    }

    ComPtr<IMMDeviceEnumerator> enumerator;
    auto code = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(enumerator.GetAddressOf()));
    if (FAILED(code)) {
        result.platform_code = code;
        return result;
    }

    const std::array<std::wstring, 3> default_ids{
        default_endpoint_id(*enumerator.Get(), eConsole),
        default_endpoint_id(*enumerator.Get(), eMultimedia),
        default_endpoint_id(*enumerator.Get(), eCommunications)
    };

    ComPtr<IMMDeviceCollection> endpoints;
    code = enumerator->EnumAudioEndpoints(
        eRender,
        DEVICE_STATE_ACTIVE,
        endpoints.GetAddressOf());
    if (FAILED(code)) {
        result.platform_code = code;
        return result;
    }

    UINT count = 0;
    code = endpoints->GetCount(&count);
    if (FAILED(code)) {
        result.platform_code = code;
        return result;
    }
    result.endpoints.reserve(count);

    for (UINT index = 0; index < count; ++index) {
        ComPtr<IMMDevice> endpoint;
        code = endpoints->Item(index, endpoint.GetAddressOf());
        if (FAILED(code)) {
            result.platform_code = code;
            return result;
        }
        LPWSTR endpoint_id = nullptr;
        code = endpoint->GetId(&endpoint_id);
        if (FAILED(code)) {
            result.platform_code = code;
            return result;
        }
        const std::wstring id{endpoint_id};
        CoTaskMemFree(endpoint_id);

        EndpointRecord record;
        if (!probe_endpoint(
                *endpoint.Get(),
                id == default_ids[0],
                id == default_ids[1],
                id == default_ids[2],
                record,
                result.platform_code)) {
            return result;
        }
        result.endpoints.push_back(std::move(record));
    }
    return result;
}

} // namespace device_matrix::windows
