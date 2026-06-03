#include "utils/logger.hpp"

import check;
import utils;

namespace
{

enum class CheckMode
{
    TrainingSpan,
};

}

int main()
{
    sim::utils::logging::init();

    switch (CheckMode::TrainingSpan)
    {
    case CheckMode::TrainingSpan:
        return sim::check::RunTrainingSpanCheck();
    default:
        return 1;
    }
}
