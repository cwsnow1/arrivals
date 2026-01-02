#include "cta.h"

#define ID_OFFSET (30000)
#define INDEX(id) ((id) - ID_OFFSET)


led_index_t cta_get_led_index(size_t stop_id)
{
    // not implemented
    return (led_index_t) { 0, 0 };
}
