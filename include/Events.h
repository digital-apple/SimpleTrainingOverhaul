#pragma once

class Events final : public RE::BSTEventSink<RE::MenuOpenCloseEvent>, public RE::BSTEventSink<RE::TESQuestStartStopEvent>
{
public:
    static auto GetSingleton() -> Events*;
    static void Register();
private:
    auto ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) -> RE::BSEventNotifyControl override;
    auto ProcessEvent(const RE::TESQuestStartStopEvent* a_event, RE::BSTEventSource<RE::TESQuestStartStopEvent>*) -> RE::BSEventNotifyControl override;
};