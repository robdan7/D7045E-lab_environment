#include "Event_system.h"
#include "Event_actor.h"
namespace Engine {
    std::unordered_map<std::type_index, I_Event_list*> Event_system::event_containers;

    void Event_system::Init() {
        /// TODO do something useful.
        event_containers.clear();
    }
}
