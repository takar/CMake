
#ifndef cmConfiguration_h
#define cmConfiguration_h

#include "cmStandardIncludes.h"

class cmake;

class cmConfiguration
{
public:
  cmConfiguration(cmake* cm);

  enum CacheEntryType{ BOOL=0, PATH, FILEPATH, STRING, INTERNAL,STATIC,
                       UNINITIALIZED };
  static CacheEntryType StringToCacheEntryType(const char*);
  static const char* CacheEntryTypeToString(CacheEntryType);
  static bool IsCacheEntryType(std::string const& key);

  std::vector<std::string> GetCacheEntryKeys() const;
  const char* GetCacheEntryValue(std::string const& key) const;
  const char* GetInitializedCacheValue(std::string const& key) const;
  CacheEntryType GetCacheEntryType(std::string const& key) const;
  void SetCacheEntryValue(std::string const& key, std::string const& value);
  void SetCacheValue(std::string const& key, std::string const& value);

  void AddCacheEntry(const std::string& key, const char* value,
                     const char* helpString, CacheEntryType type);
  void RemoveCacheEntry(std::string const& key);

  void SetCacheEntryProperty(std::string const& key,
                             std::string const& propertyName,
                             std::string const& value);
  void SetCacheEntryBoolProperty(std::string const& key,
                                 std::string const& propertyName,
                                 bool value);
  const char* GetCacheEntryProperty(std::string const& key,
                                    std::string const& propertyName);
  bool GetCacheEntryPropertyAsBool(std::string const& key,
                                   std::string const& propertyName);
  void AppendCacheEntryProperty(std::string const& key,
                                const std::string& property,
                                const std::string& value,
                                bool asString = false);
  void RemoveCacheEntryProperty(std::string const& key,
                                std::string const& propertyName);

private:
  cmake* CMakeInstance;
};

#endif
