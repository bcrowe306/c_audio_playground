#pragma once

#ifndef EVENT_REGISTRY_H
#define EVENT_REGISTRY_H

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
// TODO: remove the use of this global singleton event registry. It is not thread safe and can cause issues in a multi-threaded environment.
// Consider using the observer module in the library instead.
class EventRegistry
{
public:
    // Singleton instance retrieval
    static EventRegistry &getInstance()
    {
        static EventRegistry instance;
        return instance;
    }

    // Subscribe an event handler to an event
    void subscribe(const std::string &eventName, std::function<void(void *)> handler)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        eventHandlers_[eventName].push_back(handler);
    }

    // Unsubscribe an event handler from an event
    void unsubscribe(const std::string &eventName, const std::function<void(void *)> &handler)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto &handlers = eventHandlers_[eventName];
        handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
                                      [&](const std::function<void(void *)> &h)
                                      {
                                          return h.target<void(void *)>() == handler.target<void(void *)>();
                                      }),
                       handlers.end());
    }

    // Publish an event
    void publish(const std::string &eventName, void *eventArgs = nullptr)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = eventHandlers_.find(eventName);
        if (it != eventHandlers_.end())
        {
            for (const auto &handler : it->second)
            {
                handler(eventArgs);
            }
        }
    }

    // Delete copy and move constructors and assignment operators
    EventRegistry(const EventRegistry &) = delete;
    EventRegistry &operator=(const EventRegistry &) = delete;
    EventRegistry(EventRegistry &&) = delete;
    EventRegistry &operator=(EventRegistry &&) = delete;

private:
    EventRegistry() {}
    std::map<std::string, std::vector<std::function<void(void *)>>> eventHandlers_;
    std::mutex mutex_;
};

#endif // !EVENT_REGISTRY_H

