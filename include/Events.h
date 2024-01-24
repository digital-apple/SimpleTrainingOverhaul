#pragma once

class Events final : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
    static auto GetSingleton() -> Events*;
    static void Register();
private:
    auto ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) -> RE::BSEventNotifyControl override;
};