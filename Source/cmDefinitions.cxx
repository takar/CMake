/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmDefinitions.h"

//----------------------------------------------------------------------------
cmDefinitions::cmDefinitions(cmDefinitions* parent)
  : Up(parent)
{
}

//----------------------------------------------------------------------------
std::pair<const char*, bool> cmDefinitions::GetInternal(const std::string& key)
{
  MapType::const_iterator i = this->Map.find(key);
  std::pair<const char*, bool> result((const char*)0, false);
  if(i != this->Map.end())
    {
    result = std::make_pair(i->second.Exists ? i->second.c_str() : 0, true);
    }
  return result;
}

//----------------------------------------------------------------------------
const char* cmDefinitions::Get(const std::string& key)
{
  std::list<cmDefinitions*> ups;
  cmDefinitions* up = this;
  std::pair<const char*, bool> result((const char*)0, false);
  while (up)
    {
    result = up->GetInternal(key);
    if(result.second)
      {
      break;
      }
    ups.push_back(up);
    up = up->Up;
    }
  // Store the result in intermediate scopes.
  for (std::list<cmDefinitions*>::const_iterator it = ups.begin();
       it != ups.end(); ++it)
    {
    (*it)->Set(key, result.first);
    }
  return result.first;
}

//----------------------------------------------------------------------------
void cmDefinitions::Set(const std::string& key, const char* value)
{
  Def def(value);
  this->Map[key] = def;
}

void cmDefinitions::Erase(const std::string& key)
{
  this->Map.erase(key);
}

//----------------------------------------------------------------------------
std::vector<std::string> cmDefinitions::LocalKeys() const
{
  std::vector<std::string> keys;
  keys.reserve(this->Map.size());
  // Consider local definitions.
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    if (mi->second.Exists)
      {
      keys.push_back(mi->first);
      }
    }
  return keys;
}

//----------------------------------------------------------------------------
cmDefinitions cmDefinitions::MakeClosure(
    std::list<cmDefinitions>::const_reverse_iterator rbegin,
    std::list<cmDefinitions>::const_reverse_iterator rend)
{
  std::set<std::string> undefined;
  cmDefinitions closure;
  closure.MakeClosure(undefined, rbegin, rend);
  return closure;
}

//----------------------------------------------------------------------------
void
cmDefinitions::MakeClosure(std::set<std::string>& undefined,
    std::list<cmDefinitions>::const_reverse_iterator rbegin,
    std::list<cmDefinitions>::const_reverse_iterator rend)
{
  for (std::list<cmDefinitions>::const_reverse_iterator it = rbegin;
       it != rend; ++it)
    {
    // Consider local definitions.
    for(MapType::const_iterator mi = it->Map.begin();
        mi != it->Map.end(); ++mi)
      {
      // Use this key if it is not already set or unset.
      if(this->Map.find(mi->first) == this->Map.end() &&
         undefined.find(mi->first) == undefined.end())
        {
        if(mi->second.Exists)
          {
          this->Map.insert(*mi);
          }
        else
          {
          undefined.insert(mi->first);
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
std::vector<std::string>
cmDefinitions::ClosureKeys(std::set<std::string>& bound) const
{
  std::vector<std::string> defined;
  defined.reserve(this->Map.size());
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    // Use this key if it is not already set or unset.
    if(bound.insert(mi->first).second && mi->second.Exists)
      {
      defined.push_back(mi->first);
      }
    }
  return defined;
}
