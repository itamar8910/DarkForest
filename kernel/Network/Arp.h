#pragma once

#include "types.h"
#include "constants.h"
#include "NetworkTypes.h"
#include "TaskBlocker.h"
#include "shared_ptr.h"
#include "types/vector.h"

namespace Network
{
class Arp final
{
public:

    static Arp& the();

    bool send_arp_request(const IPV4 target_ip, const IPV4 sender_ip, MAC& out_answer);

    void on_arp_message_received(u8* message, size_t size);

    class PendingRequestBlocker : public TaskBlocker{
    public:
        PendingRequestBlocker(IPV4 target, u32 timeout_ms);
        virtual bool can_unblock() override;
        ~PendingRequestBlocker() override = default;

        MAC answer() const { return m_answer; }
        bool has_answer() const { return m_has_answer; }
        void set_answer(MAC answer) { m_answer = answer; m_has_answer = true;}
        IPV4 target() const { return m_target; }
        
    private:
        IPV4 m_target {};
        u32 m_timeout_timestamp {0};
        MAC m_answer {};
        bool m_has_answer {false};
    };

private:
    Arp() = default;


    Vector<shared_ptr<PendingRequestBlocker>> m_arp_blockers;
};
}