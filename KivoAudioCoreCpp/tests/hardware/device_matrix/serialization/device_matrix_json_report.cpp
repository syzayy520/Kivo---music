#include "device_matrix_json_report.hpp"

#include <cstddef>
#include <iomanip>
#include <ostream>

#include "../fixture/device_matrix_names.hpp"

namespace device_matrix {
namespace {

void write_json_string(std::ostream& output, std::string_view value) {
    output << '"';
    for (const unsigned char character : value) {
        switch (character) {
        case '"': output << "\\\""; break;
        case '\\': output << "\\\\"; break;
        case '\b': output << "\\b"; break;
        case '\f': output << "\\f"; break;
        case '\n': output << "\\n"; break;
        case '\r': output << "\\r"; break;
        case '\t': output << "\\t"; break;
        default:
            if (character < 0x20u) {
                output << "\\u"
                       << std::hex << std::setw(4) << std::setfill('0')
                       << static_cast<unsigned int>(character)
                       << std::dec << std::setfill(' ');
            } else {
                output << static_cast<char>(character);
            }
            break;
        }
    }
    output << '"';
}

void write_boolean(std::ostream& output, bool value) {
    output << (value ? "true" : "false");
}

void write_endpoint(
    std::ostream& output,
    const EndpointRecord& endpoint,
    bool trailing_comma) {
    output << "    {\n"
           << "      \"identity\": \"0x"
           << std::hex << std::uppercase << endpoint.identity
           << std::dec << std::nouppercase << "\",\n"
           << "      \"friendly_name\": ";
    write_json_string(output, endpoint.friendly_name);
    output << ",\n      \"category\": ";
    write_json_string(output, category_name(endpoint.category));
    output << ",\n      \"state\": ";
    write_json_string(output, state_name(endpoint.state));
    output << ",\n      \"form_factor\": ";
    write_json_string(output, endpoint.form_factor);
    output << ",\n      \"endpoint_driver_version\": ";
    write_json_string(output, endpoint.endpoint_driver_version);
    output << ",\n      \"parent_device_name\": ";
    write_json_string(output, endpoint.parent_device_name);
    output << ",\n      \"parent_driver_provider\": ";
    write_json_string(output, endpoint.parent_driver_provider);
    output << ",\n      \"parent_driver_version\": ";
    write_json_string(output, endpoint.parent_driver_version);
    output << ",\n      \"default_roles\": {\n"
           << "        \"console\": ";
    write_boolean(output, endpoint.default_console);
    output << ",\n        \"multimedia\": ";
    write_boolean(output, endpoint.default_multimedia);
    output << ",\n        \"communications\": ";
    write_boolean(output, endpoint.default_communications);
    output << "\n      },\n"
           << "      \"capabilities\": {\n"
           << "        \"shared_mix_supported\": ";
    write_boolean(output, endpoint.shared_mix_supported);
    output << ",\n        \"exclusive_mix_supported\": ";
    write_boolean(output, endpoint.exclusive_mix_supported);
    output << ",\n        \"default_period_100ns\": "
           << endpoint.default_period_100ns
           << ",\n        \"minimum_period_100ns\": "
           << endpoint.minimum_period_100ns
           << "\n      },\n"
           << "      \"mix_format\": {\n"
           << "        \"format_tag\": " << endpoint.mix_format.format_tag
           << ",\n        \"channels\": " << endpoint.mix_format.channels
           << ",\n        \"sample_rate\": "
           << endpoint.mix_format.sample_rate
           << ",\n        \"bits_per_sample\": "
           << endpoint.mix_format.bits_per_sample
           << ",\n        \"block_alignment\": "
           << endpoint.mix_format.block_alignment
           << "\n      }\n"
           << "    }" << (trailing_comma ? "," : "") << "\n";
}

} // namespace

void write_device_matrix_json(
    std::ostream& output,
    const DeviceInventory& inventory,
    std::string_view machine_id,
    bool inventory_valid) {
    output << "{\n"
           << "  \"schema\": \"kivo.wasapi-device-inventory.v1\",\n"
           << "  \"machine_id\": ";
    write_json_string(output, machine_id);
    output << ",\n  \"inventory_valid\": ";
    write_boolean(output, inventory_valid);
    output << ",\n  \"platform_code\": " << inventory.platform_code
           << ",\n  \"endpoint_count\": " << inventory.endpoints.size()
           << ",\n  \"endpoints\": [\n";
    for (std::size_t index = 0;
         index < inventory.endpoints.size();
         ++index) {
        write_endpoint(
            output,
            inventory.endpoints[index],
            index + 1 < inventory.endpoints.size());
    }
    output << "  ]\n}\n";
}

} // namespace device_matrix
