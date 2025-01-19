#include "hdr/infex.h"
#include "hdr/state.h"

void action_quit(void)
{
    state_set_quit(true);
}
