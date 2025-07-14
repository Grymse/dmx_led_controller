#include "binary_store.h"

BinaryStore::BinaryStore(const String &ns, const String &keyName)
{
    namespaceName = ns;
    key = keyName;
}

bool BinaryStore::saveData(const uint8_t *data, uint32_t length)
{
    if (!data || length == 0)
        return false;

    if (preferences.begin(namespaceName.c_str(), false))
    {
        size_t written = preferences.putBytes(key.c_str(), data, length);
        preferences.end();
        return written == length;
    }
    return false;
}

uint32_t BinaryStore::loadData(uint8_t *buffer, uint32_t maxLength)
{
    if (!buffer || maxLength == 0)
        return 0;

    uint32_t bytesRead = 0;
    if (preferences.begin(namespaceName.c_str(), true))
    {
        bytesRead = preferences.getBytes(key.c_str(), buffer, maxLength);
        preferences.end();
    }
    return bytesRead;
}

bool BinaryStore::clearData()
{
    if (preferences.begin(namespaceName.c_str(), false))
    {
        preferences.remove(key.c_str());
        preferences.end();
        return true;
    }
    return false;
}
