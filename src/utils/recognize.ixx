module;

#include "macro.hpp"

export module utils:recognize;

import std;

namespace sim::utils::recognize
{

export class RecognizeText
{
public:
    ~RecognizeText() = default;

    DELETE_COPY_AND_ASSIGN(RecognizeText);

#if 0
    static RecognizeText& Get();

    void Init();
    void Finalize();

    std::string ImageToText() const;
#endif

private:
    RecognizeText();
};

}
