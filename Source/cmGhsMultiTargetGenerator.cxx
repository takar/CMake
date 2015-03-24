/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2015 Geoffrey Viola <geoffrey.viola@asirobots.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmGhsMultiTargetGenerator.h"
#include "cmGlobalGhsMultiGenerator.h"
#include "cmLocalGhsMultiGenerator.h"
#include "cmMakefile.h"
#include "cmTarget.h"
#include "cmGeneratedFileStream.h"
#include "cmSourceFile.h"
#include <assert.h>
#include <cmAlgorithms.h>

std::string const cmGhsMultiTargetGenerator::DDOption("-dynamic");

cmGhsMultiTargetGenerator::cmGhsMultiTargetGenerator(cmTarget const *target)
    : Target(target), LocalGenerator(static_cast<cmLocalGhsMultiGenerator *>(
                          target->GetMakefile()->GetLocalGenerator())),
      Makefile(target->GetMakefile()), TargetGroup(DetermineIfTargetGroup()),
      DynamicDownload(false) {
  std::string BuildFileName;
  BuildFileName = this->Target->GetName();
  BuildFileName += cmGlobalGhsMultiGenerator::FILE_EXTENSION;

  char const *folderProp = this->Target->GetProperty("FOLDER");
  RelBuildFilePath = NULL == folderProp ? "" : folderProp;
  if (RelBuildFilePath.size() > 0 && '/' != RelBuildFilePath.back()) {
    RelBuildFilePath += "/";
  }
  RelBuildFilePath += Target->GetName() + "/";

  RelOutputFileName = RelBuildFilePath + Target->GetName() + ".a";

  RelBuildFileName = RelBuildFilePath;
  if (RelBuildFileName.size() > 0 && '/' != RelBuildFileName.back()) {
    RelBuildFileName += "/";
  }
  RelBuildFileName += BuildFileName;

  std::string absPathToRoot(this->Makefile->GetHomeOutputDirectory());
  if (absPathToRoot.size() > 0 && '/' != absPathToRoot.back()) {
    absPathToRoot += "/";
  }
  AbsBuildFilePath = absPathToRoot + RelBuildFilePath;
  AbsBuildFileName = absPathToRoot + RelBuildFileName;
  AbsOutputFileName = absPathToRoot + RelOutputFileName;
}

cmGhsMultiTargetGenerator::~cmGhsMultiTargetGenerator()
{
  cmDeleteAll(FolderBuildStreams);
}

void cmGhsMultiTargetGenerator::Generate() {
  std::vector<cmSourceFile *> objectSources = GetSources();
  if (objectSources.size() > 0 && IncludeThisTarget()) {
    if (!cmSystemTools::FileExists(AbsBuildFilePath.c_str())) {
      cmSystemTools::MakeDirectory(AbsBuildFilePath.c_str());
    }
    cmGlobalGhsMultiGenerator::Open(std::string(""), this->AbsBuildFileName,
                                    &FolderBuildStreams);
    cmGlobalGhsMultiGenerator::OpenBuildFileStream(GetFolderBuildStreams());
    std::string config = this->Makefile->GetSafeDefinition("CMAKE_BUILD_TYPE");
    if (0 == config.length()) {
      config = "RELEASE";
    }
    const std::string language(this->Target->GetLinkerLanguage(config));
    config = cmSystemTools::UpperCase(config);
    DynamicDownload = DetermineIfDynamicDownload(config, language);
    if (DynamicDownload) {
      *this->GetFolderBuildStreams() << "#component integrity_dynamic_download"
                                     << std::endl;
    }
    GhsMultiGpj::WriteGpjTag(GetGpjTag(), this->GetFolderBuildStreams());
    cmGlobalGhsMultiGenerator::WriteDisclaimer(GetFolderBuildStreams());

    bool const notKernel = IsNotKernel(config, language);
    WriteTypeSpecifics(config, notKernel);
    WriteDebugOptions(config, notKernel);
    WriteCompilerOptions(config, language);
    WriteCompilerFlags();
    WriteCompilerDefinitions(config, language);
    WriteIncludes(config, language);
    WriteTargetLinkLibraries();
    WriteCustomCommands();
    if (DynamicDownload) {
      *this->GetFolderBuildStreams() << "    " << DDOption << std::endl;
    }

    WriteSources(objectSources);
  }
}

bool cmGhsMultiTargetGenerator::IncludeThisTarget() {
  bool output = true;
  char const *excludeFromAll = this->Target->GetProperty("EXCLUDE_FROM_ALL");
  if (NULL != excludeFromAll && '1' == excludeFromAll[0] &&
      '\0' == excludeFromAll[1]) {
    output = false;
  }
  return output;
}

std::vector<cmSourceFile *> cmGhsMultiTargetGenerator::GetSources() const {
  std::vector<cmSourceFile *> output;
  std::string config = this->Makefile->GetSafeDefinition("CMAKE_BUILD_TYPE");
  this->Target->GetSourceFiles(output, config);
  return output;
}

GhsMultiGpj::Types cmGhsMultiTargetGenerator::GetGpjTag() const {
  GhsMultiGpj::Types output;
  if (IsTargetGroup()) {
    output = GhsMultiGpj::Types::INTERGRITY_APPLICATION;
  } else {
    output = GhsMultiGpj::Types::PROGRAM;
  }
  return output;
}

cmGlobalGhsMultiGenerator *
cmGhsMultiTargetGenerator::GetGlobalGenerator() const {
  return static_cast<cmGlobalGhsMultiGenerator *>(
      LocalGenerator->GetGlobalGenerator());
}

void cmGhsMultiTargetGenerator::WriteTypeSpecifics(const std::string &config,
                                                   bool const notKernel) {
  std::string outputDir(GetOutputDirectory(config));
  std::string outputFilename(GetOutputFilename(config));

  if (Target->GetType() == cmTarget::TargetType::STATIC_LIBRARY) {
    *GetFolderBuildStreams() << "    -relobj" << std::endl;
    *GetFolderBuildStreams() << "    {optgroup=GhsCommonOptions} -o \""
                             << outputDir << outputFilename << ".a\""
                             << std::endl;
  } else if (Target->GetType() == cmTarget::TargetType::EXECUTABLE) {
    if (notKernel && !IsTargetGroup()) {
      *GetFolderBuildStreams() << "    -relprog" << std::endl;
    }
    if (IsTargetGroup()) {
      *GetFolderBuildStreams() << "    -non_shared" << std::endl;
      *GetFolderBuildStreams() << "    -o \"" << outputDir << outputFilename
                               << ".elf\"" << std::endl;
    } else {
      *GetFolderBuildStreams() << "    {optgroup=GhsCommonOptions} -o \""
                               << outputDir << outputFilename << ".as\""
                               << std::endl;
    }
  }
}

void cmGhsMultiTargetGenerator::WriteDebugOptions(std::string const &config,
                                                  bool const notKernel) {
  if ("DEBUG" == config) {
    if (notKernel) {
      *GetFolderBuildStreams() << "    -G" << std::endl;
      *GetFolderBuildStreams() << "    -Onone" << std::endl;
    } else {
      *GetFolderBuildStreams() << "    -ldebug" << std::endl;
    }
  } else if (notKernel) {
    *GetFolderBuildStreams() << "    -O" << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteCompilerOptions(
    std::string const &config, const std::string &language) {
  std::vector<std::string> options;
  Target->GetCompileOptions(options, config, language);
  bool hasStartfileDirProp = false;
  std::string const startFilePropName("-startfile_dir=");
  for (std::vector<std::string>::const_iterator options_i = options.begin();
       options_i != options.end(); ++options_i) {
    std::string option = *options_i;
    if (DDOption != option) {
      if (option.length() >= startFilePropName.length() &&
          startFilePropName == option.substr(0, startFilePropName.length())) {
        hasStartfileDirProp = true;
      }
      cmSystemTools::ConvertToUnixSlashes(option);
      *this->GetFolderBuildStreams() << "    " << option << std::endl;
    }
  }

  // If this property is relative, make it relative to the root lists file
  if (!hasStartfileDirProp && GetGlobalGenerator()->IsOSDirRelative()) {
    *this->GetFolderBuildStreams() << "    " << startFilePropName << "\""
                                   << this->Makefile->GetHomeOutputDirectory()
                                   << "/$(__LIBS_DIR_BASE)/$(__BSP_NAME)\""
                                   << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteCompilerFlags() {
  char const *const compileFlags = Target->GetProperty("COMPILE_FLAGS");
  if (NULL != compileFlags) {
    *this->GetFolderBuildStreams() << "    " << compileFlags << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteCompilerDefinitions(
    const std::string &config, const std::string &language) {
  std::vector<std::string> compileDefinitions;
  this->Target->GetCompileDefinitions(compileDefinitions, config, language);
  for (std::vector<std::string>::const_iterator cdI =
           compileDefinitions.begin();
       cdI != compileDefinitions.end(); ++cdI) {
    *this->GetFolderBuildStreams() << "    -D" << (*cdI) << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteIncludes(const std::string &config,
                                              const std::string &language) {
  std::vector<std::string> includes =
      Target->GetIncludeDirectories(config, language);
  for (std::vector<std::string>::const_iterator includes_i = includes.begin();
       includes_i != includes.end(); ++includes_i) {
    *this->GetFolderBuildStreams() << "    -I\"" << *includes_i << "\""
                                   << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteTargetLinkLibraries() {
  // library directories
  cmTargetDependSet tds =
      GetGlobalGenerator()->GetTargetDirectDepends(*this->Target);
  for (cmTargetDependSet::iterator tdsI = tds.begin(); tdsI != tds.end();
       ++tdsI) {
    cmTarget const *tg(*tdsI);
    cmGhsMultiTargetGenerator gmtg(tg);
    *this->GetFolderBuildStreams() << "    -L\"" << gmtg.GetAbsBuildFilePath()
                                   << "\"" << std::endl;
  }
  // library targets
  cmTarget::LinkLibraryVectorType llv =
      this->Target->GetOriginalLinkLibraries();
  for (cmTarget::LinkLibraryVectorType::const_iterator llvI = llv.begin();
       llvI != llv.end(); ++llvI) {
    std::string libName = llvI->first;
    // if it is a user defined target get the full path to the lib
    cmTarget *tg(GetGlobalGenerator()->FindTarget(libName));
    if (NULL != tg) {
      cmGhsMultiTargetGenerator gmtg(tg);
      libName = tg->GetName() + ".a";
    }
    *this->GetFolderBuildStreams() << "    -l\"" << libName << "\""
                                   << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteCustomCommands() {
  WriteCustomCommandsHelper(this->Target->GetPreBuildCommands(),
                            cmTarget::CustomCommandType::PRE_BUILD);
  WriteCustomCommandsHelper(this->Target->GetPostBuildCommands(),
                            cmTarget::CustomCommandType::POST_BUILD);
}

void cmGhsMultiTargetGenerator::WriteCustomCommandsHelper(
    std::vector<cmCustomCommand> const &commandsSet,
    cmTarget::CustomCommandType const commandType) {
  for (std::vector<cmCustomCommand>::const_iterator commandsSetI =
           commandsSet.begin();
       commandsSetI != commandsSet.end(); ++commandsSetI) {
    cmCustomCommandLines const &commands = commandsSetI->GetCommandLines();
    for (cmCustomCommandLines::const_iterator commandI = commands.begin();
         commandI != commands.end(); ++commandI) {
      switch (commandType) {
      case cmTarget::CustomCommandType::PRE_BUILD:
        *GetFolderBuildStreams() << "    :preexecShellSafe=";
        break;
      case cmTarget::CustomCommandType::POST_BUILD:
        *GetFolderBuildStreams() << "    :postexecShellSafe=";
        break;
      default:
        assert("Only pre and post are supported");
      }
      cmCustomCommandLine const &command = *commandI;
      for (cmCustomCommandLine::const_iterator commandLineI = command.begin();
           commandLineI != command.end(); ++commandLineI) {
        if (command.size() > 0) {
          *GetFolderBuildStreams()
              << (command.begin() == commandLineI ? "'" : " ");
        }
        *GetFolderBuildStreams() << *commandLineI;
      }
      if (command.size() > 0) {
        *GetFolderBuildStreams() << "'" << std::endl;
      }
    }
  }
}

void cmGhsMultiTargetGenerator::WriteSources(
    std::vector<cmSourceFile *> const &objectSources) {
  for (std::vector<cmSourceFile *>::const_iterator si = objectSources.begin();
       si != objectSources.end(); ++si) {
    std::vector<cmSourceGroup> sourceGroups(this->Makefile->GetSourceGroups());
    char const *sourceFullPath = (*si)->GetFullPath().c_str();
    cmSourceGroup *sourceGroup =
        this->Makefile->FindSourceGroup(sourceFullPath, sourceGroups);
    std::string sgPath(sourceGroup->GetFullName());
    cmSystemTools::ConvertToUnixSlashes(sgPath);
    cmGlobalGhsMultiGenerator::AddFilesUpToPath(
        GetFolderBuildStreams(), &FolderBuildStreams,
        this->Makefile->GetHomeOutputDirectory(), sgPath,
        GhsMultiGpj::Types::SUBPROJECT, RelBuildFilePath);

    if ((*si)->GetExtension() == ".int") {
      *this->FolderBuildStreams[sgPath] << "\"" << (*si)->GetFullPath() << "\""
                                        << std::endl;
    } else {
      *this->FolderBuildStreams[sgPath] << (*si)->GetFullPath() << std::endl;
    }

    if ("ld" != (*si)->GetExtension() && "int" != (*si)->GetExtension() &&
        "bsp" != (*si)->GetExtension()) {
      WriteObjectLangOverride(this->FolderBuildStreams[sgPath], (*si));

      WriteObjectDir(this->FolderBuildStreams[sgPath],
                     AbsBuildFilePath + sgPath);
    }
  }
}

void cmGhsMultiTargetGenerator::WriteObjectLangOverride(
    cmGeneratedFileStream *fileStream, cmSourceFile *sourceFile) {
  const char *rawLangProp = sourceFile->GetProperty("LANGUAGE");
  if (NULL != rawLangProp) {
    std::string sourceLangProp(rawLangProp);
    std::string extension(sourceFile->GetExtension());
    if ("CXX" == sourceLangProp && ("c" == extension || "C" == extension)) {
      *fileStream << "    -dotciscxx" << std::endl;
    }
  }
}

void cmGhsMultiTargetGenerator::WriteObjectDir(
    cmGeneratedFileStream *fileStream, std::string const &dir) {
  std::string workingDir(dir);
  if (workingDir.size() > 0 && '/' != workingDir.back()) {
    workingDir += "/";
  }
  workingDir += "Objs";
  *fileStream << "    -object_dir=\"" << workingDir << "\"" << std::endl;
}

std::string
cmGhsMultiTargetGenerator
::GetOutputDirectory(const std::string &config) const {
  std::string outputDir(AbsBuildFilePath);

  const char *runtimeOutputProp =
      Target->GetProperty("RUNTIME_OUTPUT_DIRECTORY");
  if (NULL != runtimeOutputProp) {
    outputDir = runtimeOutputProp;
  }

  std::string configCapped(cmSystemTools::UpperCase(config));
  const char *runtimeOutputSProp =
      Target->GetProperty("RUNTIME_OUTPUT_DIRECTORY_" + configCapped);
  if (NULL != runtimeOutputSProp) {
    outputDir = runtimeOutputSProp;
  }
  cmSystemTools::ConvertToUnixSlashes(outputDir);

  if (outputDir.length() > 0 && ('/' != (*outputDir.rbegin()))) {
    outputDir += "/";
  }

  return outputDir;
}

std::string
cmGhsMultiTargetGenerator::GetOutputFilename(const std::string &config) const {
  std::string outputFilename(Target->GetName());

  const char *outputNameProp = Target->GetProperty("OUTPUT_NAME");
  if (NULL != outputNameProp) {
    outputFilename = outputNameProp;
  }

  std::string configCapped(cmSystemTools::UpperCase(config));
  const char *outputNameSProp =
      Target->GetProperty(configCapped + "_OUTPUT_NAME");
  if (NULL != outputNameSProp) {
    outputFilename = outputNameSProp;
  }

  return outputFilename;
}

bool cmGhsMultiTargetGenerator::IsNotKernel(std::string const &config,
                                            const std::string &language) {
  bool output;
  std::vector<std::string> options;
  Target->GetCompileOptions(options, config, language);
  output =
      options.end() == std::find(options.begin(), options.end(), "-kernel");
  return output;
}

bool cmGhsMultiTargetGenerator::DetermineIfTargetGroup() {
  bool output = false;
  std::vector<cmSourceFile *> sources = GetSources();
  for (std::vector<cmSourceFile *>::const_iterator sources_i = sources.begin();
       sources.end() != sources_i; ++sources_i) {
    if ("int" == (*sources_i)->GetExtension()) {
      output = true;
    }
  }
  return output;
}

bool cmGhsMultiTargetGenerator::DetermineIfDynamicDownload(
    std::string const &config, const std::string &language) {
  std::vector<std::string> options;
  bool output = false;
  Target->GetCompileOptions(options, config, language);
  for (std::vector<std::string>::const_iterator options_i = options.begin();
       options_i != options.end(); ++options_i) {
    std::string option = *options_i;
    if (DDOption == option) {
      output = true;
    }
  }
  return output;
}
