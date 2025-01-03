#ifndef APP_URL_HPP_
#define APP_URL_HPP_

#include <string>
#include <regex>

class Url
{
public:
    Url(std::string url)
    : mUrl(url)
    {}
    virtual ~Url() = default;

    std::string toString() const
    {
        return mUrl;
    }

    bool operator<(const Url& other) const
    {
        return this->mUrl < other.mUrl;
    }

    bool isEmpty() const
    {
        return mUrl.empty();
    }

    static bool isUrl(std::string input)
    {
        // TODO
        return true;
    }

private:
    std::string mUrl;
};
#endif // APP_URL_HPP_