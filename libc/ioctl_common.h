#pragma once

#include "types.h"

namespace IOCTL {

    namespace VgaText {
        enum class Code : u32{
            PUT_CHAR = 1,
            MOVE_CURSOR = 2,
            GET_CHAR = 3,
            CLEAR = 4,
            GET_ENTRY=5,
            UPDATE_CURSOR=6,
        };

        struct [[gnu::packed]] Data {
            u8 row {0};
            u8 col {0};
            u16 value {0};
        };

        enum class Errs {
            E_OUT_OF_BOUNDS = 1,
        };

    };

    namespace VGA {
        enum class Code : u32{
            GET_DIMENSIONS,
        };

        struct [[gnu::packed]] Data {
            u16 width {0};
            u16 height {0};
            u16 pitch {0};
        };

        enum class Errs {
        };

    };

};
