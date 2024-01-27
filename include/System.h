#pragma once

class System
{
public:
    struct Topic
    {
        RE::FormID quest;
        RE::FormID topic;
    };

    template<class A, class B>
    union ConversionCaster
    {
        A _a;
        B _b;

        ConversionCaster() = default;
        ConversionCaster(A a) : _a(a) {}
        ConversionCaster(B b) : _b(b) {}

        ConversionCaster& operator=(A a) { _a = a; return *this; }
        ConversionCaster& operator=(B b) { _b = b; return *this; }

        operator A() { return _a; }
        operator B() { return _b; }
    };
    
    template <class B>
    using VoidCaster = ConversionCaster<void*, B>;

    static auto GetSingleton() -> System*;

    bool ContainsTopic(RE::FormID a_id);
    void ParseTopics();
    void PatchTopicInfos(RE::FormID a_id);
    void PatchTopics();
    void PatchTopics(RE::FormID a_id);
private:
    System() = default;
    System(const System&) = delete;
    System(System&&) = delete;

    ~System() = default;

    System& operator=(const System&) = delete;
    System& operator=(System&&) = delete;

    std::vector<System::Topic> topics;
};