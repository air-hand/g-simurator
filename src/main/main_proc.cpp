#include <boost/program_options.hpp>
#include <directxtk/Keyboard.h>

#include <google/protobuf/message_lite.h>
#include <google/protobuf/util/json_util.h>

#include "std/windows.hpp"

#include "controller/keyboard.hpp"
#include "route/route.hpp"

#include "main_proc.hpp"

import utils.logger;
import utils.window_inspector;
import utils.capture;

namespace options = boost::program_options;
namespace logging = sim::utils::logging;
namespace window = sim::utils::window;
using Keys = DirectX::Keyboard::Keys;

namespace sim
{
class MainProc::Impl
{
public:
    Impl(int argc, char** argv)
    {
        Init(argc, argv);
    }
    ~Impl()
    {
        Cleanup();
    }

    int Run()
    {
        if (!std::filesystem::exists(route_path_))
        {
            logging::log("Route file not found. {}", route_path_.string());
            return 1;
        }

#if false // test code
        const auto desktop = window::Window(GetDesktopWindow());
        desktop.Activate();
        desktop.Capture();
#endif

        const auto reader = route::RouteReader();
        const auto route = reader.ReadJSONFile(route_path_);

        const auto windowName = route.window_name();
        const window::WindowInspector inspector {};
        const auto window = inspector.Find(windowName);
        if (window == nullptr) {
            logging::log("Window not found: [{}]", windowName);
            return 1;
        }
        window->Activate();
        window->Capture();

        const auto& keyboard = sim::controller::Keyboard::Get();
        keyboard.KeyDown({'A', 'B', 'C', Keys::Enter});

        return 0;
    }

    void AddFinalizer(std::function<void()> finalizer)
    {
        finalizers_.emplace_back(std::move(finalizer));
    }

private:
    std::vector<std::function<void()>> finalizers_;
    std::filesystem::path route_path_;

    void Init(int argc, char** argv)
    {
        logging::init();
        AddFinalizer([] {
            logging::log("Good bye, World...");
        });
        AddFinalizer([] {
            logging::log("Shutting down protobuf library...");
            google::protobuf::ShutdownProtobufLibrary();
        });
        utils::Capture::Get().Init();
        AddFinalizer([] {
            utils::Capture::Get().Finalize();
        });
        options::options_description desc("Options");
        desc.add_options()
            ("route", options::value<std::filesystem::path>(&route_path_), "Route file")
        ;
        options::variables_map vm;
        options::store(options::parse_command_line(argc, argv, desc), vm);
        notify(vm);
    }

    void Cleanup() const
    {
        std::for_each(std::crbegin(finalizers_), std::crend(finalizers_), [](auto&& func) {
            func();
        });
    }
};

MainProc::MainProc(int argc, char** argv) noexcept
    : impl_(std::make_unique<Impl>(argc, argv))
{
}

MainProc::~MainProc() = default;

MainProc::MainProc(MainProc&&) noexcept = default;
MainProc& MainProc::operator=(MainProc&& rhs)
{
    if (this != &rhs)
    {
        impl_ = std::move(rhs.impl_);
    }
    return *this;
}

int MainProc::Run()
{
    return impl_->Run();
}

void MainProc::AddFinalizer(std::function<void()> finalizer)
{
    impl_->AddFinalizer(std::move(finalizer));
}

}
