#pragma once

#include <functional>
#include <atomic>
#include <mutex>
#include "Event_system.h"
#include <iostream>

namespace Engine {

    /**
     * Each event actor has its own inbox that takes care of sorting events and
     * eventually executing them.
     * @tparam Events
     */
    template<typename... Events>
    class Inbox {
    private:
        /**
        * Container for one type of events in an inbox.
        * @tparam E
        */
        template<typename Event>
        class Event_container {
        public:

            /** Typedef for the callback type **/
            typedef std::function<void(Event event)> My_callback;

            Event_container() {}

            /**
             * This is the method that actually calls the callback given by an
             * actor.
             */
            void check_inbox_container() {
                std::unique_lock<decltype(this->event_m)> lock(this->event_m);
                if (!this->incoming_events.size()) return;
                /// Iterate through the list and call the function with all events.

                for (auto event : this->incoming_events) {
//                    Event_system::fetch_event<Event>(event);

                    /** This fetches a copy of the event from the event system
                     *  and dispatches the callback function.
                     **/
//                     if (Event_system::has_event<Event>(event)) {
                    this->callback(Event_system::fetch_event<Event>(event));
//                     } else {
//                         CORE_LOG_ERROR("Event system did not have event {0} of type {1}", event, typeid(Event).name());
//                     }
                }
                this->incoming_events.clear();
            }

            /**
             * Add an event ID to the list of new events.
             * @param event_ID
             */
            void notify(uint16_t event_ID) {
                std::unique_lock<decltype(this->event_m)> lock(this->event_m);
                this->incoming_events.push_back(event_ID);
            }

            template<typename Function>
            void set_callback(Function function) {
                this->callback = function;
            }

            void unsubscribe() {
                //Boggle::Event_system::unsu
            }

            /*void lock_mutex() {
                this->event_m.lock();
            }

            void unlock_mutex() {
                this->event_m.unlock();
            }
*/
        public:
            My_callback callback = [](Event i) {};
            std::vector<uint16_t> incoming_events;
            std::mutex event_m;
        };

    public:
        Inbox() {
            ((std::get<Event_container<Events>*>(this->m_event_containers) = new Event_container<Events>()),...);
        }

        /**
         * Check all events in the inbox once. It looks weird, I know.
         * Think of Events as a list of variable types, and check_container is called
         * for every type in Events.
         */
        void check_inbox() {
            (this->check_container<Events>(),...);
        }

        /**
         * Just notify the event list that some event has arrived.
         * @tparam Event - The event type.
         * @param event_ID - Global event ID used by the event system.
         */
        template<typename Event>
        void notify(unsigned int event_ID) {
            std::get<Event_container<Event>*>(this->m_event_containers)->notify(event_ID);
        }

        /**
         * Set the callback function for one type of subscribed events.
         * @tparam Event
         * @tparam Function
         * @param function
         */
        template<typename Event, typename Function>
        void set_callback(const Function function) {
            std::get<Event_container<Event>*>(m_event_containers)->set_callback(std::move(function));
        }

        template<typename Event>
        bool has_subscription() {
            return ((typeid(Event) == typeid(Events)) || ...);
        }

        bool has_subscription(std::type_info type) {
            return ((type == typeid(Events)) || ...);
        }

    private:
        /**
         * This locks a container in the inbox and checks its events.
         * @tparam Event - The event type to check.
         */
        template<typename Event>
        void check_container() {
            std::get<Event_container<Event>*>(this->m_event_containers)->check_inbox_container();
            //container->unlock_mutex();
        }
    private:
        std::tuple<Event_container<Events>*...> m_event_containers;
    };


    class I_Event_actor {
    public:
        //virtual bool has_subscription(std::type_info type) = 0;
    };

    /**
     * This is the generic class for event actors. Both single threaded and
     * multi threaded actors inherit from this class.
     * @tparam Events
     */
    template<typename... Events>
    class Event_actor : public I_Event_actor {
    public:

        Event_actor() {
            this->inbox = new Inbox<Events...>();
        }

        ~Event_actor() {
            delete this->inbox;
        }



        /**
         * Check the actor inbox immediately.
         */
        void check_inbox() {
            this->inbox->check_inbox();
        }

        template<typename Event>
        bool has_subscription() {
            return this->inbox->template has_subscription<Event>();
        }

        /*bool has_subscription(std::type_info type) override {
            return this->inbox-> template has_subscription(type);
        }*/



    protected:
        /**
 * This is the only method the user need to care about. It sets
 * a callback for an event type, so whenever that event is generated
 * the callback is executed. Remember that you can capture the actor object inside
 * a lambda function.
 * @tparam Event - Any kind of object, actually. This is the event that was posted.
 * @param function - Presumably a lambda function that takes the event as argument.
 */
        template<typename Event>
        void set_callback(const std::function<void(Event)> function) {
            this->inbox->template set_callback<Event, std::function<void(Event)>>(function);
        }
        /**
         * Subscriptions are not added when the actor is created. Instead,
         * the user must decide if and when to initialize all subscriptions.
         *
         * This is useful for multi threaded event actors. It allows the actor to start a thread
         * before subscribing to any events.
         */
        virtual void init_subscriptions() {
            /*((this->m_tokens[typeid(Events)] = Event_system::subscribe_to<Events>([this](auto ID){
                this->notify<Events>(ID);
            })),...);*/
            ((this->set_subscription<Events>(Event_system::subscribe_to<Events>([this](auto ID){
                this->notify<Events>(ID);
            }))),...);
        }

        template<typename Event>
        void set_subscription(Subscriber_token token) {
            this->m_tokens[typeid(Event)] = token;
        }

        /**
         * Internal function for unsubscribing from events. Undefined behaviour if
         * a non valid event type is used.
         * @tparam Event
         * @return
         */
        template<typename Event>
        bool unsubscribe() {
            return Event_system::unsubscribe<Event>(this->m_tokens[typeid(Event)]);
        }

        void unsubscribe_all() {
            ((this->unsubscribe<Events>()),...);
        }

        /**
         * Internal manual notify method. Undefined behaviour if subscriptions are
         * initialized.
         * @tparam Event
         * @param ID
         */
        template<typename Event>
        void notify(unsigned int ID) {
            this->inbox->template notify<Event>(ID);
        }

    private:
        Inbox<Events...> *inbox;
        std::unordered_map<std::type_index,Subscriber_token> m_tokens;
    };
}
