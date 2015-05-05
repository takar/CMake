#!/usr/bin/env python

import os, sys, json

def collectString(s):
  s = s.strip()

  result = ""
  idx = s.find('"')
  nullptr = s.find('0')
  if (nullptr < idx):
    return (None, None)

  oldIdx = idx + 1
  i = 0
  while idx != -1:
    i = i + 1
    if (i > 10):
      break
    idx = s.find('"', oldIdx)
    result += s[oldIdx:idx]
    oldIdx = idx + 1
    nextQuote = s.find('"', oldIdx)
    nextComma = s.find(',', oldIdx)
    if nextQuote == -1 or (nextComma < nextQuote):
      break
    oldIdx = nextQuote + 1
    idx = oldIdx
  return result, oldIdx + 2

def cleanString(s):
  return s.replace("\\", "").strip()

with open(sys.argv[2]) as policyHeader:
  content = policyHeader.read()

  start = content.find("CM_FOR_EACH_POLICY_TABLE") + 18
  content = content[start:]
  start = content.find("SELECT(POLICY, ")

  policies = []

  while start != -1:
    content = content[start + 15:]
    policyId, content = content.split(",", 1)
    docString, docLength = collectString(content)
    content = content[docLength:]
    rstString, rstLength = collectString(content)
    if (rstString is None):
      rstString = docString
    else:
      content = content[rstLength:]
    versionMajor, versionMinor, versionPatch, content = content.split(",", 3)
    versionMajor = cleanString(versionMajor)
    versionMinor = cleanString(versionMinor)
    versionPatch = cleanString(versionPatch)
    version = versionMajor + "." + versionMinor + "." + versionPatch

    policies.append({"id": policyId, "doc": docString,
                     "rst": rstString, "version": version})

    start = content.find("SELECT(POLICY, ")

  with open(os.path.join(sys.argv[1], "PolicyList.json"), "w") as policiesList:

    policiesList.write(json.dumps(policies))
