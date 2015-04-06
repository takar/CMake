
#include "cmConfiguration.h"

#include "cmake.h"
#include "cmCacheManager.h"

cmConfiguration::cmConfiguration(cmake* cm)
  : CMakeInstance(cm)
{
}

const char* cmCacheEntryTypes[] =
{ "BOOL",
  "PATH",
  "FILEPATH",
  "STRING",
  "INTERNAL",
  "STATIC",
  "UNINITIALIZED",
  0
};

const char*
cmConfiguration::CacheEntryTypeToString(cmConfiguration::CacheEntryType type)
{
  if ( type > 6 )
    {
    return cmCacheEntryTypes[6];
    }
  return cmCacheEntryTypes[type];
}

cmConfiguration::CacheEntryType
cmConfiguration::StringToCacheEntryType(const char* s)
{
  int i = 0;
  while(cmCacheEntryTypes[i])
    {
    if(strcmp(s, cmCacheEntryTypes[i]) == 0)
      {
      return static_cast<cmConfiguration::CacheEntryType>(i);
      }
    ++i;
    }
  return STRING;
}

bool cmConfiguration::IsCacheEntryType(std::string const& key)
{
  for(int i=0; cmCacheEntryTypes[i]; ++i)
    {
    if(strcmp(key.c_str(), cmCacheEntryTypes[i]) == 0)
      {
      return true;
      }
    }
  return false;
}

std::vector<std::string> cmConfiguration::GetCacheEntryKeys() const
{
  std::vector<std::string> definitions;
  definitions.reserve(this->CMakeInstance->GetCacheManager()->GetSize());
  cmCacheManager::CacheIterator cit =
    this->CMakeInstance->GetCacheManager()->GetCacheIterator();
  for ( cit.Begin(); !cit.IsAtEnd(); cit.Next() )
    {
    definitions.push_back(cit.GetName());
    }
  return definitions;
}

const char* cmConfiguration::GetCacheEntryValue(std::string const& key) const
{
  cmCacheManager::CacheEntry* e = this->CMakeInstance->GetCacheManager()
             ->GetCacheEntry(key);
  if (!e)
    {
    return 0;
    }
  return e->Value.c_str();
}

const char*
cmConfiguration::GetInitializedCacheValue(std::string const& key) const
{
  return this->CMakeInstance->GetCacheManager()->GetInitializedCacheValue(key);
}

cmConfiguration::CacheEntryType
cmConfiguration::GetCacheEntryType(std::string const& key) const
{
  cmCacheManager::CacheIterator it =
      this->CMakeInstance->GetCacheManager()->GetCacheIterator(key.c_str());
  return it.GetType();
}

void cmConfiguration::SetCacheEntryValue(std::string const& key,
                                         std::string const& value)
{
  this->CMakeInstance->GetCacheManager()->SetCacheEntryValue(key, value);
}

void cmConfiguration::SetCacheEntryProperty(std::string const& key,
                            std::string const& propertyName,
                            std::string const& value)
{
  cmCacheManager::CacheIterator it =
      this->CMakeInstance->GetCacheManager()->GetCacheIterator(key.c_str());
  it.SetProperty(propertyName, value.c_str());
}

void cmConfiguration::SetCacheEntryBoolProperty(std::string const& key,
                            std::string const& propertyName,
                            bool value)
{
  cmCacheManager::CacheIterator it =
      this->CMakeInstance->GetCacheManager()->GetCacheIterator(key.c_str());
  it.SetProperty(propertyName, value);
}

const char* cmConfiguration::GetCacheEntryProperty(std::string const& key,
                                              std::string const& propertyName)
{
  cmCacheManager::CacheIterator it = this->CMakeInstance->GetCacheManager()
             ->GetCacheIterator(key.c_str());
  if (!it.PropertyExists(propertyName))
    {
    return 0;
    }
  return it.GetProperty(propertyName);
}

bool cmConfiguration::GetCacheEntryPropertyAsBool(std::string const& key,
                                              std::string const& propertyName)
{
  return this->CMakeInstance->GetCacheManager()
             ->GetCacheIterator(key.c_str()).GetPropertyAsBool(propertyName);
}

void cmConfiguration::AddCacheEntry(const std::string& key, const char* value,
                                    const char* helpString,
                                    cmConfiguration::CacheEntryType type)
{
  this->CMakeInstance->GetCacheManager()->AddCacheEntry(key, value,
                                                        helpString, type);
}

void cmConfiguration::RemoveCacheEntry(std::string const& key)
{
  this->CMakeInstance->GetCacheManager()->RemoveCacheEntry(key);
}

void cmConfiguration::AppendCacheEntryProperty(const std::string& key,
                                               const std::string& property,
                                               const std::string& value,
                                               bool asString)
{
  this->CMakeInstance->GetCacheManager()
       ->GetCacheIterator(key.c_str()).AppendProperty(property,
                                                       value.c_str(),
                                                       asString);
}

void cmConfiguration::RemoveCacheEntryProperty(std::string const& key,
                                              std::string const& propertyName)
{
  this->CMakeInstance->GetCacheManager()
       ->GetCacheIterator(key.c_str()).SetProperty(propertyName, (void*)0);
}
