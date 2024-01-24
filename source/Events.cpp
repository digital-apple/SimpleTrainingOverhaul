#include "Events.h"

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