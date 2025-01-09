#include "NetworkReply.hpp"

NetworkReply::NetworkReply(std::string content, int statusCode, std::string error)
: mStatusCode{statusCode}
, mError{error}
, mContent{content}
{
}

bool NetworkReply::error() const
{
    return !mError.empty();
}

std::string NetworkReply::getErrorString() const
{
    return mError;
}

int NetworkReply::getStatusCode() const
{
    return mStatusCode;
}

std::string NetworkReply::read() const
{
    return mContent;
}
