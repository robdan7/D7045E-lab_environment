#pragma once

#include <cstdint>
#include <vector>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <Utils/Sparse_set.h>
#include <functional>
#include "System_events.h"
#include <iostream>
namespace Engine {
    using Subscriber_token = uint32_t;
    using Event_ID_length = uint16_t;

    class I_Event_actor;

    class I_Event_list {

    };


    template<typename Event_type>
    class Event_list : public I_Event_list{
    public:
        Event_list(uint32_t index_per_page) : sparse_set(index_per_page){

        }
//        bool has_event(Event_ID_length event_ID) {
//            if (this->sparse_set.has_ID(event_ID)) {
//                int index = this->sparse_set.index(event_ID);
//                if (index < this->events.size()) {
//                    return true;
//                }
//                return false;
//            } else {
//                return false;
//            }
////            return this->sparse_set.has_ID(event_ID) && this->sparse_set.index(event_ID) < this->events.size();
//        }

        /**
         * Return a copy of an event. Not a reference.
         * @param event_ID
         */
        Event_type fetch_event(Event_ID_length event_ID) {

            this->event_m.lock();
            const auto index = this->sparse_set.index(event_ID);
            auto event = this->events.at(index).event;

            if (!(--this->events.at(index).ref_counter)) {
                this->events.at(index) = this->events.back();
                this->events.pop_back();
                this->sparse_set.remove(event_ID);
                this->deleted.push_back(event_ID);
            }
            this->event_m.unlock();
            return event;


        }

        template<typename... Args>
        void send_event(Args&&... args) {
            this->event_m.lock();
            Event_ID_length event_ID;
            if (deleted.size()) {
                event_ID = this->deleted.back();
                this->deleted.pop_back();
            } else {
                event_ID = this->events.size();
            }
            this->sparse_set.put(event_ID);

            this->actor_m.lock();
            if constexpr (std::is_aggregate_v<Event_type>) {
                this->events.push_back(Event_container{this->m_callback_container.size(), Event_type{args...}});
            } else {
                this->events.push_back(Event_container{this->m_callback_container.size(), Event_type(args...)});
            }
            this->event_m.unlock();
            for (auto container : this->m_callback_container) {
                container.m_function_callback(event_ID);
            }
            this->actor_m.unlock();
        }

        template<typename Callback>
        Subscriber_token subscribe(Callback callback) {
            this->actor_m.lock();
            //this->function_callbacks.push_back(callback);
            Subscriber_token token = -1;
            if(this->m_available_sub_tokens.empty()) {
                token = this->m_next_token++;
            } else {
                token = this->m_available_sub_tokens.back();
                this->m_available_sub_tokens.pop_back();
            }
            Callback_container container = {token, callback};
            this->m_callback_container.emplace_back(token,callback);
            this->actor_m.unlock();
            return token;
        }

        /**
         * Attempt to unsubscribe from an event. This function will permanently remove
         * the subscribed callback and replace it the last added subscription.
         * @tparam Event
         * @param token
         * @return
         */
        template<typename Event>
        bool unsubscribe(Subscriber_token token) {
            uint32_t i = 0;
            bool flag = false;
            for (const auto& container: this->m_callback_container) {
                if (container.m_token == token) {
                    flag = true;
                    break;
                }
                i++;
            }
            if (flag) {
                auto last_callback = this->m_callback_container.back();
                this->m_callback_container[i] = last_callback;
                this->m_callback_container.pop_back();
            }
            return flag;
        }

        /**
         * Let an actor unsubscribe from an event. Deleting an actor without unsubscribing
         * leads to undefined behaviour.
         * @param actor
         */
        // TODO Implement this the proper way, ya dingus fungus.
//        void unsubscribe(I_Event_actor* actor) {
//            this->actor_m.lock();
//            auto iterator = std::find(this->actors.begin(), this->actors.end(), actor);
//            if (iterator != this->actors.end()) {
//                *iterator = this->actors.back();
//                this->actors.pop_back();
//            }
//            this->actor_m.unlock();
//        }

        bool has_listeners() {
            return !this->m_callback_container.empty();
        }

    private:
        struct Event_container {
            uint64_t ref_counter;
            Event_type event;
        };
        struct Callback_container {
            Subscriber_token m_token;
            std::function<void(uint16_t)> m_function_callback;
            Callback_container(Subscriber_token m_token, std::function<void(uint16_t)> m_function_callback): m_token(m_token), m_function_callback(m_function_callback) {}
        };
        std::vector<Subscriber_token> m_available_sub_tokens;
        Subscriber_token m_next_token;

        std::mutex event_m, actor_m;
        std::vector<Event_container> events;    /// Queued up events
        Sparse_set<uint32_t> sparse_set;
        std::vector<uint16_t> deleted;

        std::vector<Callback_container> m_callback_container;   /// All subscribed callbacks.
    };

    class Event_system {
    private:
        static const uint8_t PAGE_SIZE = 8;
    public:
    public:
        /*
        template<typename Event>
        static bool has_event(Event_ID_length event_ID) {
            return static_cast<Event_list <Event>*>(event_containers.at(typeid(Event)))->has_event(event_ID);
        }
*/
        template<typename Event>
        static Event fetch_event(Event_ID_length event_ID) {
            return static_cast<Event_list <Event>*>(event_containers.at(typeid(Event)))->fetch_event(event_ID);
        }


        template<typename Event>
        static void set_container() {
            if (!event_containers.count(typeid(Event))) {
                event_containers[typeid(Event)] = static_cast<I_Event_list*>(new Event_list<Event>(PAGE_SIZE));
            }
        }

        /**
         * Subscribe to one type of event.
         * @tparam Event
         * @tparam Callback - The notify function
         * @param callback
         */
        template<typename Event>
        static Subscriber_token subscribe_to(std::function<void(Event_ID_length)> callback) {
            set_container<Event>();
            return static_cast<Event_list<Event>*>(event_containers.at(typeid(Event)))->subscribe(callback);
        }

        template<typename Event>
        static bool unsubscribe(Subscriber_token token) {
            return static_cast<Event_list<Event>*>(event_containers.at(typeid(Event)))->template unsubscribe<Event>(token);
        }

        template<typename Event, typename... Args>
        static void post(Args&&... args) {
            if (event_containers.count(typeid(Event))>0 && static_cast<Event_list<Event>*>(event_containers.at(typeid(Event)))->has_listeners()) {
                //CORE_LOG_TRACE("Posting event of type {0}", typeid(Event).name());
                static_cast<Event_list<Event>*>(event_containers.at(typeid(Event)))->template send_event<Args...>(std::forward<Args>(args)...);
            } else {
//                CORE_LOG_WARN("No actor is listening to event {0}", typeid(Event).name());
            }
        }

        static void Init();

    private:


    private:
        static std::unordered_map<std::type_index, I_Event_list*> event_containers;
    };
}