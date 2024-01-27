#include "Events.h"
#include "System.h"

auto Events::GetSingleton() -> Events*
{
    static Events singleton;
    return std::addressof(singleton);
}

void Events::Register()
{
    if (const auto UI = RE::UI::GetSingleton()) {
        UI->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());
        logs::info("Events :: Registered for MenuOpenCloseEvent.");
    }

    if (const auto events = RE::ScriptEventSourceHolder::GetSingleton()) {
        events->GetEventSource<RE::TESQuestStartStopEvent>()->AddEventSink(GetSingleton());
        logs::info("Events :: Registered for TESQuestStartStopEvent.");
    }
}

auto Events::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) -> RE::BSEventNotifyControl
{
    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (a_event->menuName == RE::TrainingMenu::MENU_NAME) {
        if (a_event->opening) {
            const auto ui = RE::UI::GetSingleton();

            if (ui) {
                const auto menu = ui->GetMenu(RE::TrainingMenu::MENU_NAME);

                if (menu && menu->uiMovie) {
                    RE::GFxValue zero{ 0.0F };
                    menu->uiMovie->SetVariable("TrainingMenuObj.TrainingCard.SkillMeter._alpha", zero);
                }
            }
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}

auto Events::ProcessEvent(const RE::TESQuestStartStopEvent* a_event, RE::BSTEventSource<RE::TESQuestStartStopEvent>*) -> RE::BSEventNotifyControl
{
    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (!System::GetSingleton()->ContainsTopic(a_event->formID)) {
        return RE::BSEventNotifyControl::kContinue;
    }

    System::GetSingleton()->PatchTopics(a_event->formID);

    return RE::BSEventNotifyControl::kContinue;
}