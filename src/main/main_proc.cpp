module;

#include <boost/program_options.hpp>
#include <directxtk/Keyboard.h>

#include <google/protobuf/message_lite.h>
#include <google/protobuf/util/json_util.h>

#include "utils/logger.hpp"
#include "route/route.hpp"

module sim;

import utils;
import :main;
import :keyboard;

namespace options = boost::program_options;
namespace logging = sim::utils::logging;
namespace window = sim::utils::window;
using Keys = DirectX::Keyboard::Keys;

namespace sim
{
class MainProc::Impl
{
public:
    Impl(uint32_t argc, char** argv)
    {
        initialized_ = Init(argc, argv);
    }
    ~Impl()
    {
        Finalize();
    }

    uint32_t Run()
    {
        DEBUG_LOG_SPAN(_);
        if (!initialized_)
        {
            logging::log("Failed to initialize.");
            return 1;
        }

        if (!std::filesystem::exists(route_path_))
        {
            logging::log("Route file not found. {}", route_path_.string());
            return 1;
        }

        const auto reader = route::RouteReader();
        const auto route = reader.ReadJSONFile(route_path_);
        if (route.routes().empty())
        {
            return 0;
        }

        const auto windowName = route.window_name();
        const window::WindowInspector inspector {};
        const auto window = inspector.Find(windowName);
        if (window == nullptr)
        {
            logging::log("Window not found: [{}]", windowName);
            return 1;
        }
        DEBUG_LOG_ARGS("Window name: [{}]", window->Name());
        window->Activate();

        auto capture = window->CreateCapture();
        capture.Start();

        auto& recognizer = utils::recognize::RecognizeText::Get();
        {
            // Main Loop
            do
            {
                if (cancel_requested_)
                {
                    logging::log("Canceled.");
                    break;
                }
                window->Activate();
                // FIXME: capture結果が空だとPopで待ち続けるので、cancelできなくなる
                const auto mat = capture.Pop();
                const auto results = recognizer.RecognizeImage(mat, 50.0f);
                const auto text = results.ToString();
                logging::log("Recognized: [{}]", text);
#ifdef DEBUG
                {
                    const auto filename = sim::utils::strings::fmt(L"./tmp/roi_applied_{:%Y%m%d%H%M%S}.png", std::chrono::system_clock::now());
                    sim::utils::image::saveImage(results.DrawRects(), sim::utils::unicode::to_utf8(filename));
                }
#endif
            }
            while (true);
//
//            std::size_t index = 0;
//            while (index < static_cast<std::size_t>(route.routes_size()))
//            {
//                const auto& r = route.routes(index);
//                const auto& roi = r.roi();
//                const auto& expected = r.expected();
//                while (true)
//                {
//                    const auto mat = capture.Pop();
//                    // FIXME: expectedが含まれる場合、captureにROIを渡して適用させるとかどうか？
//                    const auto roiMat = route::applyROI(mat, roi);
//                    const auto results = recognizer.RecognizeImage(roiMat, 50.0f);
//                    const auto text = results.ToString();
//                    logging::log("Recognized: [{}]", text);
//#ifdef DEBUG
//                    const auto filename = sim::utils::strings::fmt(L"./tmp/roi_applied_{:%Y%m%d%H%M%S}.png", std::chrono::system_clock::now());
//                    sim::utils::image::saveImage(results.DrawRects(), sim::utils::unicode::to_utf8(filename));
//#endif
//                    if (text.contains(expected))
//                    {
//                        logging::log("Expected [{}] contained.", expected);
//                        break;
//                    }
//                }
//                const auto& keys = r.keys();
//                for (const auto key : keys)
//                {
//                    keyboard.KeyDown(key.at(0));
//                    sim::utils::time::sleep(1000);
//                }
//                ++index;
//            }
        }
        return 0;
    }

    void Cancel()
    {
        logging::log("Cancel requested.");
        cancel_requested_ = true;
    }

    void AddFinalizer(std::function<void()> finalizer)
    {
        finalizers_.emplace_back(std::move(finalizer));
    }

private:
    std::vector<std::function<void()>> finalizers_;
    std::filesystem::path route_path_;
    bool cancel_requested_ = false;
    bool initialized_ = false;

    bool Init(uint32_t argc, char** argv)
    {
        {
            AddFinalizer([] {
                logging::log("Good bye, World...");
            });
        }
        {
            AddFinalizer([] {
                DEBUG_LOG("Shutting down protobuf library...");
                google::protobuf::ShutdownProtobufLibrary();
            });
        }
        {
            utils::CaptureContext::Get().Init();
            AddFinalizer([] {
                utils::CaptureContext::Get().Finalize();
            });
        }
        {
            if (!utils::recognize::RecognizeText::Get().Init())
            {
                return false;
            }
            AddFinalizer([] {
                utils::recognize::RecognizeText::Get().Finalize();
            });
        }
        options::options_description desc("Options");
        desc.add_options()
            ("route", options::value<std::filesystem::path>(&route_path_), "Route file")
        ;
        options::variables_map vm;
        options::store(options::parse_command_line(argc, argv, desc), vm);
        notify(vm);
        return true;
    }

    void Finalize() const
    {
        std::for_each(std::crbegin(finalizers_), std::crend(finalizers_), [](auto&& func) {
            func();
        });
    }
};

MainProc::MainProc(uint32_t argc, char** argv) noexcept
    : impl_(std::make_unique<Impl>(argc, argv))
{
}

MainProc::~MainProc()
{
    DEBUG_LOG_SPAN(_);
    impl_.reset();
}

MainProc::MainProc(MainProc&&) noexcept = default;
MainProc& MainProc::operator=(MainProc&& rhs)
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

uint32_t MainProc::Run()
{
    return impl_->Run();
}

void MainProc::Cancel()
{
    impl_->Cancel();
}

void MainProc::AddFinalizer(std::function<void()> finalizer)
{
    impl_->AddFinalizer(std::move(finalizer));
}

}
