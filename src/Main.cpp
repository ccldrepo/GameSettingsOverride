#include <PCH.h>

#include <spdlog/sinks/basic_file_sink.h>

#include "GameSettings.h"
#include "Util/Win.h"

namespace
{
    void InitLogger()
    {
        auto path = SKSE::log::log_directory();
        if (!path) {
            SKSE::stl::report_and_fail("Failed to find SKSE logging directory."sv);
        }
        *path /= SKSE::PluginDeclaration::GetSingleton()->GetName();
        *path += L".log";

        auto logger = spdlog::basic_logger_mt("Global", path->string(), true);
#ifndef _DEBUG
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);
#else
        logger->set_level(spdlog::level::debug);
        logger->flush_on(spdlog::level::debug);
#endif

        spdlog::set_default_logger(std::move(logger));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    }

    void OnMessage(SKSE::MessagingInterface::Message* a_message)
    {
        switch (a_message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            GameSettings::Load();
            break;
        default:
            break;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    InitLogger();

    auto os_ver = GetOsVersion();
    if (os_ver) {
        SKSE::log::info("OS Version: {}", os_ver->string("."sv));
    } else {
        SKSE::log::info("OS Version: Unknown");
    }

    SKSE::log::info("Game Version: {}", a_skse->RuntimeVersion().string("."sv));

    auto plugin = SKSE::PluginDeclaration::GetSingleton();
    SKSE::log::info("{} {} is loading...", plugin->GetName(), plugin->GetVersion().string("."sv));

    SKSE::Init(a_skse);

    auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener(OnMessage);

    SKSE::log::info("{} has finished loading.", plugin->GetName());
    return true;
}
