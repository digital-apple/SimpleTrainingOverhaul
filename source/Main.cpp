#include "Events.h"
#include "Hooks.h"
#include "Serialization.h"

void InitLogging()
{
    auto path = logs::log_directory();
    if (!path)
        return;

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    *path /= fmt::format("{}.log", plugin->GetName());

    std::vector<spdlog::sink_ptr> sinks{ 
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true), 
        std::make_shared<spdlog::sinks::msvc_sink_mt>() 
    };

    auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::info);

    spdlog::set_default_logger(std::move(logger));
    spdlog::set_pattern("[%^%L%$] %v");
}

void InitMessaging()
{
    logs::trace("Initializing messaging listener.");
    const auto interface = SKSE::GetMessagingInterface();
    if (!interface->RegisterListener([](SKSE::MessagingInterface::Message* a_message) {
        if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
            Addresses::Hook();
            Events::GetSingleton()->Register();
        }
        })) {
        stl::report_and_fail("Failed to initialize message listener.");
    }
}

void InitSerialization()
{
    logs::trace("Initializing cosave serialization...");

    auto* interface = SKSE::GetSerializationInterface();
    interface->SetUniqueID(Serialization::kModID);
    interface->SetSaveCallback(Serialization::OnGameSaved);
    interface->SetRevertCallback(Serialization::OnRevert);
    interface->SetLoadCallback(Serialization::OnGameLoaded);

    logs::trace("Cosave serialization initialized.");
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    InitLogging();

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    logs::info("{} v{} is loading...", plugin->GetName(), plugin->GetVersion());

    SKSE::Init(a_skse);
    InitMessaging();
    InitSerialization();

    logs::info("{} loaded.", plugin->GetName());

    return true;
}
