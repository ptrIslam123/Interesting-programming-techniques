#pragma once

#include <array>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>
#include <cstring>

class Subscriber;
class Publisher;

typedef char Byte;
typedef std::array<Byte, 1024> Buffer;

class Publisher {
public:
    enum class EventList {
        FirstEvent,
        SecondEvent
        /** ... **/
    };

    typedef EventList Event;
    typedef void (Subscriber::*Callback)(const Buffer &buffer);
    typedef std::pair<Subscriber*, Callback> CallbackInfo;

    void addSubscriber(Subscriber *subscriber, const Event event, Callback callback) {
        auto callbacksIter = subscribers_.find(event);
        if (callbacksIter == subscribers_.cend()) {
            Callbacks callbacks = {std::make_pair(subscriber, callback)};
            subscribers_.insert({event, callbacks});
        } else {
            callbacksIter->second.emplace_back(subscriber, callback);
        }
    }

    void removeSubscriber(Subscriber *subscriber, const Event event) {
        auto callbacksIter = subscribers_.find(event);
        if (callbacksIter == subscribers_.cend()) {
            throw std::runtime_error("can`t find this event type");
        }

        auto &callbacks = callbacksIter->second;
        auto subscriberIter = std::find_if(callbacks.begin(), callbacks.end(), [subscriber](CallbackInfo &callbackInfo){
            return subscriber == callbackInfo.first;
        });

        if (subscriberIter == callbacks.cend()) {
            throw std::runtime_error("can`t fine subscriber");
        }
        callbacks.erase(subscriberIter);
    }

    void notifyAbout(const Event event, const Buffer &data) const {
        const auto callbacksIter = subscribers_.find(event);
        if (callbacksIter == subscribers_.cend()) {
            throw std::runtime_error("can`t find this event type");
        }
        std::for_each(callbacksIter->second.cbegin(), callbacksIter->second.cend(),
                      [&data](const CallbackInfo &callbackInfo){
            Subscriber *subscriber = callbackInfo.first;
            Callback callback = callbackInfo.second;
            (subscriber->*callback)(data);
        });
    }
private:
    typedef std::vector<CallbackInfo> Callbacks;
    std::map<Event, Callbacks> subscribers_;
};


struct Subscriber {
    virtual ~Subscriber() = default;
};

class SomeWidget : public Subscriber {
public:
    SomeWidget() = default;

    void eventHandler(const Buffer &data) {
        deserialize(data);
        operator<<(std::cout);
    }

    std::ostream &operator<<(std::ostream &oss) const {
        oss << "change widget position: (" << x_ << ", " << y_ << ")" << std::endl;
        return oss;
    }

    void serialize(Buffer &buffer, const int x, const int y) {
        std::memcpy(buffer.data(), &x, sizeof(x));
        std::memcpy(buffer.data() + sizeof(x), &y, sizeof(y));
    }

    void deserialize(const Buffer &buffer) {
        std::memcpy(&x_, buffer.data(), sizeof(x_));
        std::memcpy(&y_, buffer.data() + sizeof(x_), sizeof(y_));
    }

private:
    int x_;
    int y_;
};

static std::unique_ptr<SomeWidget> makeAndAddSubscriber(Publisher &publisher,
                                                        const Publisher::Event event) {
    auto subscriber = std::make_unique<SomeWidget>();
    const auto callback = static_cast<Publisher::Callback>(&SomeWidget::eventHandler);
    publisher.addSubscriber(subscriber.get(), event, callback);
    return std::move(subscriber);
}

static std::ostream &operator<<(std::ostream &oss, const SomeWidget &widget) {
    return widget.operator<<(oss);
}

static void examplePublisherAndSubscriber() {
    Publisher publisher;
    auto subscriber1 = makeAndAddSubscriber(publisher, Publisher::Event::FirstEvent);
    auto subscriber2 = makeAndAddSubscriber(publisher, Publisher::Event::FirstEvent);
    auto subscriber3 = makeAndAddSubscriber(publisher, Publisher::Event::FirstEvent);
    auto subscriber4 = makeAndAddSubscriber(publisher, Publisher::Event::SecondEvent);

    publisher.removeSubscriber(subscriber1.get(), Publisher::Event::FirstEvent);

    Buffer buffer1 = {0};
    subscriber1->serialize(buffer1, 10, 14);
    publisher.notifyAbout(Publisher::Event::FirstEvent, buffer1);

    Buffer buffer2 = {0};
    subscriber2->serialize(buffer2, 23, 45);
    publisher.notifyAbout(Publisher::Event::SecondEvent, buffer2);
}