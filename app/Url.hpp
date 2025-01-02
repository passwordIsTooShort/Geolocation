#ifndef APP_URL_HPP_
#define APP_URL_HPP_

#include <string>

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

    bool isEmpty() const
    {
        return mUrl.empty();
    }

private:
    std::string mUrl;
};
#endif // APP_URL_HPP_