#include "System.h"
#include "Serialization.h"

auto System::GetSingleton() -> System*
{
    static System singleton;
    return std::addressof(singleton);
}

void System::ParseTopics()
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (data) {
        std::ifstream path("Data/SKSE/Plugins/Simple Training Overhaul/Topics.json");
        json config = json::parse(path);
        const json& elements = config["Topics"];

        for (const auto& element : elements.array_range()) {
            const auto quest = data->LookupFormID(element["Quest"]["FormID"].as<RE::FormID>(), element["Quest"]["ModName"].as<std::string>());
            const auto topic = data->LookupFormID(element["Topic"]["FormID"].as<RE::FormID>(), element["Topic"]["ModName"].as<std::string>());
            const System::Topic instance {quest, topic};
            topics.emplace_back(instance);
        }
    }

    logs::info("System::ParseTopics :: Number of topics: '{}'", topics.size());
}

bool System::ContainsTopic(RE::FormID a_id)
{
    return std::find_if(topics.begin(), topics.end(), [a_id](System::Topic topic) { return topic.quest == a_id; }) != topics.end();
}

void System::PatchTopics()
{
    for (const auto& topic : topics) {
        System::PatchTopicInfos(topic.topic);
    }
}

void System::PatchTopics(RE::FormID a_id)
{
    for (const auto& topic : topics) {
        if (topic.quest == a_id) {
            System::PatchTopicInfos(topic.topic);
        }
    }
}

void System::PatchTopicInfos(RE::FormID a_id)
{
    const auto topic = RE::TESForm::LookupByID<RE::TESTopic>(a_id);
    
    logs::info("Current Topic: '0x{:x}'", a_id);

    if (topic) {
        logs::info("Topic: '0x{:x}' number of topics: '{}'", a_id, topic->numTopicInfos);
        for (std::size_t i = 0; i < topic->numTopicInfos; i++) {
            auto conditions = topic->topicInfos[i]->objConditions.head;

            while (conditions) {
                if (conditions->data.functionData.function == RE::FUNCTION_DATA::FunctionID::kGetBaseActorValue) {
                    VoidCaster<RE::ActorValue> value = conditions->data.functionData.params[0];
                    conditions->data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetGlobalValue;
                    conditions->data.functionData.params[0] = Serialization::GetSingleton()->GetGlobalValue(value);
                    logs::info("Patched topic info: '0x{:x}' from topic: '0x{:x}' from mod: '{}'", topic->topicInfos[i]->GetFormID(), topic->GetFormID(), topic->GetFile()->GetFilename());
                }
                conditions = conditions->next;
            }
        }
    }
}