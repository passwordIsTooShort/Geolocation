#ifndef APP_NETWORKREPLY_HPP_
#define APP_NETWORKREPLY_HPP_

#include <string>

class NetworkReply
{
public:
    inline constexpr static int OK_STATUS_CODE = 200;

    NetworkReply(std::string content, int statusCode, std::string error);

    bool error() const;
    std::string getErrorString() const;
    int getStatusCode() const;
    std::string read() const;

private:
    int mStatusCode;
    std::string mError;
    std::string mContent;
};

#endif // APP_NETWORKREPLY_HPP_