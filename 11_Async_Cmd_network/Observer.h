#pragma once

#include <string>
#include <vector>

enum class update_events { event_print, event_save, event_print_save };

class Observer {
   public:
    virtual ~Observer() = default;

    virtual void update(update_events event, std::vector<std::string> cmds) = 0;
};

class Observable {
   public:
    virtual ~Observable() = default;

    virtual void subscribe(Observer* obs) = 0;
    virtual void notify(update_events event, std::vector<std::string> cmds) = 0;
};