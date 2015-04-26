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

#include <assert.h>

//----------------------------------------------------------------------------
cmDefinitions::cmDefinitions(cmDefinitions* parent)
  : Up(parent)
{
}

//----------------------------------------------------------------------------
std::pair<const char*, bool> cmDefinitions::Get(const std::string& key)
{
  MapType::const_iterator i = this->Map.find(key);
  std::pair<const char*, bool> result(0, false);
  if(i != this->Map.end())
    {
    result = std::make_pair(i->second.Exists ? i->second.c_str() : 0, true);
    }
  return result;
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
std::set<std::string> cmDefinitions::LocalKeys() const
{
  std::set<std::string> keys;
  // Consider local definitions.
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    if (mi->second.Exists)
      {
      keys.insert(mi->first);
      }
    }
  return keys;
}

//----------------------------------------------------------------------------
cmDefinitions cmDefinitions::Closure() const
{
  return cmDefinitions(ClosureTag(), this);
}

//----------------------------------------------------------------------------
cmDefinitions::cmDefinitions(ClosureTag const&, cmDefinitions const* root):
  Up(0)
{
  std::set<std::string> undefined;
  this->ClosureImpl(undefined, root);
}

//----------------------------------------------------------------------------
void cmDefinitions::ClosureImpl(std::set<std::string>& undefined,
                                cmDefinitions const* defs)
{
  // Consider local definitions.
  for(MapType::const_iterator mi = defs->Map.begin();
      mi != defs->Map.end(); ++mi)
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

  // Traverse parents.
  if(cmDefinitions const* up = defs->Up)
    {
    this->ClosureImpl(undefined, up);
    }
}

//----------------------------------------------------------------------------
std::set<std::string> cmDefinitions::ClosureKeys() const
{
  std::set<std::string> defined;
  std::set<std::string> undefined;
  this->ClosureKeys(defined, undefined);
  return defined;
}

//----------------------------------------------------------------------------
void cmDefinitions::ClosureKeys(std::set<std::string>& defined,
                                std::set<std::string>& undefined) const
{
  cmDefinitions const* up = this;

  while (up)
    {
    // Consider local definitions.
    for(MapType::const_iterator mi = up->Map.begin();
        mi != up->Map.end(); ++mi)
      {
      // Use this key if it is not already set or unset.
      if(defined.find(mi->first) == defined.end() &&
         undefined.find(mi->first) == undefined.end())
        {
        std::set<std::string>& m = mi->second.Exists? defined : undefined;
        m.insert(mi->first);
        }
      }
    up = this->Up;
    }
}
