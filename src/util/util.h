#pragma once

#include <string>

namespace Gloria
{
    class Util
    {
    public:
        // ¶ÁÈ¡ºó×ºÃû
        static std::wstring GetFileExrension(std::wstring path)
        {
            if (path.rfind('.') != std::wstring::npos && path.rfind('.') != (path.length() - 1))
            {
                return path.substr(path.rfind('.') + 1);
            }
            else
            {
                return L"";
            }
        }
    };
}