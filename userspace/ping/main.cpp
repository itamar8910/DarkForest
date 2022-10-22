
#include "df_unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"
#include "kernel/errs.h"
#include "sys/socket.h"
#include "kernel/Network/packets.h"
#include "df_unistd.h"
#include "NetworkTypes.h"
#include "unistd.h"

int main(char** argv, size_t argc) {

    if (argc != 2)
    {
        printf("Usage: %s <ip>\n", argv[0]);
        return 1;
    }

    Network::IPV4 ip{};
    if (!Network::IPV4::from_string(argv[1], ip))
    {
        printf("Failed to parse ip address\n");
        return 1;
    }

    printf("Pinging: %s\n", ip.to_string().c_str());

    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (sock < 0)
    {
        printf("socket failed\n");
        return 1;
    }

    uint16_t id = 8;
    uint16_t sequence_number = 1;

    sockaddr_in dest {
        .sin_family = PF_INET,
        .sin_port = 0,
        .sin_addr = {ip.to_u32()},
    };

    static constexpr size_t NUM_PINGS = 3;
    for(size_t i = 0; i < NUM_PINGS; ++i)
    {
        IcmpEchoHeader icmp_header {
            .type = IcmpEchoHeader::Type::Request,
            .code=0,
            .checksum=0,
            .identifier=id,
            .sequence_number = sequence_number
        };

        static constexpr size_t payload_size = 32;
        u8 icmp_data[sizeof(IcmpEchoHeader) + payload_size] = {};

        memcpy(icmp_data, &icmp_header, sizeof(icmp_header));
        memset(icmp_data + sizeof(icmp_header), (int)'A', payload_size);

        u32 timestamp = df_time_since_boot_ms();
        memcpy(icmp_data + sizeof(icmp_header), &timestamp, sizeof(u32));

        if (sendto(sock, icmp_data, sizeof(icmp_data), 0, &dest, sizeof(dest)) < 0)
        {
            printf("sendto() failed\n");
            return 1;
        }

        sockaddr_in incoming_addr {};
        size_t incoming_addr_len = sizeof(incoming_addr);
        int res = recvfrom(sock, icmp_data, sizeof(icmp_data), 0, &incoming_addr, &incoming_addr_len);
        if (res < 0)
        {
            if (res == -E_TIMED_OUT)
            {
                printf("Timed Out\n");
                return 1;
            }
            printf("recvfrom failed: %d\n", -res);
            return 1;
        }

        const IcmpEchoHeader* response = reinterpret_cast<const IcmpEchoHeader*>(icmp_data);
        auto incoming_ip = Network::IPV4::from_u32(incoming_addr.sin_addr.s_addr);

        u32 incoming_timestamp {0};
        memcpy(&incoming_timestamp, icmp_data + sizeof(IcmpEchoHeader), sizeof(u32));
        u32 roundtrip_time = df_time_since_boot_ms() - incoming_timestamp;

        printf("Ping response from %s: seq=%d rtt=%dms\n", incoming_ip.to_string().c_str(), response->sequence_number, roundtrip_time);

        ++sequence_number;

        if (i != NUM_PINGS - 1)
        {
            std::sleep_ms(1000);
        }
    }


    return 0;
}
