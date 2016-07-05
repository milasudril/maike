##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=maike
ConfigurationName      :=Debug
WorkspacePath          := "/home/torbjorr/Dokument/maike"
ProjectPath            := "/home/torbjorr/Dokument/maike"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Torbjörn Rathsman
Date                   :=05/07/16
CodeLitePath           :="/home/torbjorr/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="maike.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)pthread $(LibrarySwitch)jansson 
ArLibs                 :=  "pthread" "jansson" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -std=c++11 -Wall -Woverflow -Wconversion -Werror $(Preprocessors)
CFLAGS   :=  -g -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(ObjectSuffix) $(IntermediateDirectory)/dependencygraphdefault.cpp$(ObjectSuffix) $(IntermediateDirectory)/errormessage.cpp$(ObjectSuffix) $(IntermediateDirectory)/datasinkstd-posix.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetdirectoryloader.cpp$(ObjectSuffix) $(IntermediateDirectory)/strerror-posix.cpp$(ObjectSuffix) $(IntermediateDirectory)/maike.cpp$(ObjectSuffix) $(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(ObjectSuffix) $(IntermediateDirectory)/spiderdefault.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetcxxcompiler.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/maike-main.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetdirectory.cpp$(ObjectSuffix) $(IntermediateDirectory)/command.cpp$(ObjectSuffix) $(IntermediateDirectory)/exceptionhandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/dependency.cpp$(ObjectSuffix) $(IntermediateDirectory)/stringformat.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetbase.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetcxxloader.cpp$(ObjectSuffix) $(IntermediateDirectory)/filein-linux.cpp$(ObjectSuffix) $(IntermediateDirectory)/sysvars-linux.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/versionnumber.cpp$(ObjectSuffix) $(IntermediateDirectory)/expressionevaluatordefault.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetcxx.cpp$(ObjectSuffix) $(IntermediateDirectory)/resourceobject.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetcxxoptions.cpp$(ObjectSuffix) $(IntermediateDirectory)/targetcxxpptokenizer.cpp$(ObjectSuffix) $(IntermediateDirectory)/pipe-posix.cpp$(ObjectSuffix) $(IntermediateDirectory)/thread-posix.cpp$(ObjectSuffix) $(IntermediateDirectory)/fileutils-linuxgnu.cpp$(ObjectSuffix) $(IntermediateDirectory)/systemtargetinfo.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/parametersetdefault.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(ObjectSuffix): directorylister-linuxgnu.cpp $(IntermediateDirectory)/directorylister-linuxgnu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/directorylister-linuxgnu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(DependSuffix): directorylister-linuxgnu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(DependSuffix) -MM "directorylister-linuxgnu.cpp"

$(IntermediateDirectory)/directorylister-linuxgnu.cpp$(PreprocessSuffix): directorylister-linuxgnu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/directorylister-linuxgnu.cpp$(PreprocessSuffix) "directorylister-linuxgnu.cpp"

$(IntermediateDirectory)/dependencygraphdefault.cpp$(ObjectSuffix): dependencygraphdefault.cpp $(IntermediateDirectory)/dependencygraphdefault.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/dependencygraphdefault.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dependencygraphdefault.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dependencygraphdefault.cpp$(DependSuffix): dependencygraphdefault.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dependencygraphdefault.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dependencygraphdefault.cpp$(DependSuffix) -MM "dependencygraphdefault.cpp"

$(IntermediateDirectory)/dependencygraphdefault.cpp$(PreprocessSuffix): dependencygraphdefault.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dependencygraphdefault.cpp$(PreprocessSuffix) "dependencygraphdefault.cpp"

$(IntermediateDirectory)/errormessage.cpp$(ObjectSuffix): errormessage.cpp $(IntermediateDirectory)/errormessage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/errormessage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/errormessage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/errormessage.cpp$(DependSuffix): errormessage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/errormessage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/errormessage.cpp$(DependSuffix) -MM "errormessage.cpp"

$(IntermediateDirectory)/errormessage.cpp$(PreprocessSuffix): errormessage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/errormessage.cpp$(PreprocessSuffix) "errormessage.cpp"

$(IntermediateDirectory)/datasinkstd-posix.cpp$(ObjectSuffix): datasinkstd-posix.cpp $(IntermediateDirectory)/datasinkstd-posix.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/datasinkstd-posix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/datasinkstd-posix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/datasinkstd-posix.cpp$(DependSuffix): datasinkstd-posix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/datasinkstd-posix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/datasinkstd-posix.cpp$(DependSuffix) -MM "datasinkstd-posix.cpp"

$(IntermediateDirectory)/datasinkstd-posix.cpp$(PreprocessSuffix): datasinkstd-posix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/datasinkstd-posix.cpp$(PreprocessSuffix) "datasinkstd-posix.cpp"

$(IntermediateDirectory)/targetdirectoryloader.cpp$(ObjectSuffix): targetdirectoryloader.cpp $(IntermediateDirectory)/targetdirectoryloader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetdirectoryloader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetdirectoryloader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetdirectoryloader.cpp$(DependSuffix): targetdirectoryloader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetdirectoryloader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetdirectoryloader.cpp$(DependSuffix) -MM "targetdirectoryloader.cpp"

$(IntermediateDirectory)/targetdirectoryloader.cpp$(PreprocessSuffix): targetdirectoryloader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetdirectoryloader.cpp$(PreprocessSuffix) "targetdirectoryloader.cpp"

$(IntermediateDirectory)/strerror-posix.cpp$(ObjectSuffix): strerror-posix.cpp $(IntermediateDirectory)/strerror-posix.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/strerror-posix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/strerror-posix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/strerror-posix.cpp$(DependSuffix): strerror-posix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/strerror-posix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/strerror-posix.cpp$(DependSuffix) -MM "strerror-posix.cpp"

$(IntermediateDirectory)/strerror-posix.cpp$(PreprocessSuffix): strerror-posix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/strerror-posix.cpp$(PreprocessSuffix) "strerror-posix.cpp"

$(IntermediateDirectory)/maike.cpp$(ObjectSuffix): maike.cpp $(IntermediateDirectory)/maike.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/maike.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/maike.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/maike.cpp$(DependSuffix): maike.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/maike.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/maike.cpp$(DependSuffix) -MM "maike.cpp"

$(IntermediateDirectory)/maike.cpp$(PreprocessSuffix): maike.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/maike.cpp$(PreprocessSuffix) "maike.cpp"

$(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(ObjectSuffix): fileinfo-linuxgnu.cpp $(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/fileinfo-linuxgnu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(DependSuffix): fileinfo-linuxgnu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(DependSuffix) -MM "fileinfo-linuxgnu.cpp"

$(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(PreprocessSuffix): fileinfo-linuxgnu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fileinfo-linuxgnu.cpp$(PreprocessSuffix) "fileinfo-linuxgnu.cpp"

$(IntermediateDirectory)/spiderdefault.cpp$(ObjectSuffix): spiderdefault.cpp $(IntermediateDirectory)/spiderdefault.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/spiderdefault.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/spiderdefault.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/spiderdefault.cpp$(DependSuffix): spiderdefault.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/spiderdefault.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/spiderdefault.cpp$(DependSuffix) -MM "spiderdefault.cpp"

$(IntermediateDirectory)/spiderdefault.cpp$(PreprocessSuffix): spiderdefault.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/spiderdefault.cpp$(PreprocessSuffix) "spiderdefault.cpp"

$(IntermediateDirectory)/targetcxxcompiler.cpp$(ObjectSuffix): targetcxxcompiler.cpp $(IntermediateDirectory)/targetcxxcompiler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetcxxcompiler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetcxxcompiler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetcxxcompiler.cpp$(DependSuffix): targetcxxcompiler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetcxxcompiler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetcxxcompiler.cpp$(DependSuffix) -MM "targetcxxcompiler.cpp"

$(IntermediateDirectory)/targetcxxcompiler.cpp$(PreprocessSuffix): targetcxxcompiler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetcxxcompiler.cpp$(PreprocessSuffix) "targetcxxcompiler.cpp"

$(IntermediateDirectory)/maike-main.cpp$(ObjectSuffix): maike-main.cpp $(IntermediateDirectory)/maike-main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/maike-main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/maike-main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/maike-main.cpp$(DependSuffix): maike-main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/maike-main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/maike-main.cpp$(DependSuffix) -MM "maike-main.cpp"

$(IntermediateDirectory)/maike-main.cpp$(PreprocessSuffix): maike-main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/maike-main.cpp$(PreprocessSuffix) "maike-main.cpp"

$(IntermediateDirectory)/targetdirectory.cpp$(ObjectSuffix): targetdirectory.cpp $(IntermediateDirectory)/targetdirectory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetdirectory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetdirectory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetdirectory.cpp$(DependSuffix): targetdirectory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetdirectory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetdirectory.cpp$(DependSuffix) -MM "targetdirectory.cpp"

$(IntermediateDirectory)/targetdirectory.cpp$(PreprocessSuffix): targetdirectory.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetdirectory.cpp$(PreprocessSuffix) "targetdirectory.cpp"

$(IntermediateDirectory)/command.cpp$(ObjectSuffix): command.cpp $(IntermediateDirectory)/command.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/command.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/command.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/command.cpp$(DependSuffix): command.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/command.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/command.cpp$(DependSuffix) -MM "command.cpp"

$(IntermediateDirectory)/command.cpp$(PreprocessSuffix): command.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/command.cpp$(PreprocessSuffix) "command.cpp"

$(IntermediateDirectory)/exceptionhandler.cpp$(ObjectSuffix): exceptionhandler.cpp $(IntermediateDirectory)/exceptionhandler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/exceptionhandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/exceptionhandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/exceptionhandler.cpp$(DependSuffix): exceptionhandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/exceptionhandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/exceptionhandler.cpp$(DependSuffix) -MM "exceptionhandler.cpp"

$(IntermediateDirectory)/exceptionhandler.cpp$(PreprocessSuffix): exceptionhandler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/exceptionhandler.cpp$(PreprocessSuffix) "exceptionhandler.cpp"

$(IntermediateDirectory)/dependency.cpp$(ObjectSuffix): dependency.cpp $(IntermediateDirectory)/dependency.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/dependency.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dependency.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dependency.cpp$(DependSuffix): dependency.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dependency.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dependency.cpp$(DependSuffix) -MM "dependency.cpp"

$(IntermediateDirectory)/dependency.cpp$(PreprocessSuffix): dependency.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dependency.cpp$(PreprocessSuffix) "dependency.cpp"

$(IntermediateDirectory)/stringformat.cpp$(ObjectSuffix): stringformat.cpp $(IntermediateDirectory)/stringformat.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/stringformat.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/stringformat.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/stringformat.cpp$(DependSuffix): stringformat.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/stringformat.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/stringformat.cpp$(DependSuffix) -MM "stringformat.cpp"

$(IntermediateDirectory)/stringformat.cpp$(PreprocessSuffix): stringformat.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/stringformat.cpp$(PreprocessSuffix) "stringformat.cpp"

$(IntermediateDirectory)/targetbase.cpp$(ObjectSuffix): targetbase.cpp $(IntermediateDirectory)/targetbase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetbase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetbase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetbase.cpp$(DependSuffix): targetbase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetbase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetbase.cpp$(DependSuffix) -MM "targetbase.cpp"

$(IntermediateDirectory)/targetbase.cpp$(PreprocessSuffix): targetbase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetbase.cpp$(PreprocessSuffix) "targetbase.cpp"

$(IntermediateDirectory)/targetcxxloader.cpp$(ObjectSuffix): targetcxxloader.cpp $(IntermediateDirectory)/targetcxxloader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetcxxloader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetcxxloader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetcxxloader.cpp$(DependSuffix): targetcxxloader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetcxxloader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetcxxloader.cpp$(DependSuffix) -MM "targetcxxloader.cpp"

$(IntermediateDirectory)/targetcxxloader.cpp$(PreprocessSuffix): targetcxxloader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetcxxloader.cpp$(PreprocessSuffix) "targetcxxloader.cpp"

$(IntermediateDirectory)/filein-linux.cpp$(ObjectSuffix): filein-linux.cpp $(IntermediateDirectory)/filein-linux.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/filein-linux.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/filein-linux.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/filein-linux.cpp$(DependSuffix): filein-linux.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/filein-linux.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/filein-linux.cpp$(DependSuffix) -MM "filein-linux.cpp"

$(IntermediateDirectory)/filein-linux.cpp$(PreprocessSuffix): filein-linux.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/filein-linux.cpp$(PreprocessSuffix) "filein-linux.cpp"

$(IntermediateDirectory)/sysvars-linux.cpp$(ObjectSuffix): sysvars-linux.cpp $(IntermediateDirectory)/sysvars-linux.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/sysvars-linux.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sysvars-linux.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sysvars-linux.cpp$(DependSuffix): sysvars-linux.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sysvars-linux.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sysvars-linux.cpp$(DependSuffix) -MM "sysvars-linux.cpp"

$(IntermediateDirectory)/sysvars-linux.cpp$(PreprocessSuffix): sysvars-linux.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sysvars-linux.cpp$(PreprocessSuffix) "sysvars-linux.cpp"

$(IntermediateDirectory)/versionnumber.cpp$(ObjectSuffix): versionnumber.cpp $(IntermediateDirectory)/versionnumber.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/versionnumber.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/versionnumber.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/versionnumber.cpp$(DependSuffix): versionnumber.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/versionnumber.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/versionnumber.cpp$(DependSuffix) -MM "versionnumber.cpp"

$(IntermediateDirectory)/versionnumber.cpp$(PreprocessSuffix): versionnumber.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/versionnumber.cpp$(PreprocessSuffix) "versionnumber.cpp"

$(IntermediateDirectory)/expressionevaluatordefault.cpp$(ObjectSuffix): expressionevaluatordefault.cpp $(IntermediateDirectory)/expressionevaluatordefault.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/expressionevaluatordefault.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/expressionevaluatordefault.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/expressionevaluatordefault.cpp$(DependSuffix): expressionevaluatordefault.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/expressionevaluatordefault.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/expressionevaluatordefault.cpp$(DependSuffix) -MM "expressionevaluatordefault.cpp"

$(IntermediateDirectory)/expressionevaluatordefault.cpp$(PreprocessSuffix): expressionevaluatordefault.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/expressionevaluatordefault.cpp$(PreprocessSuffix) "expressionevaluatordefault.cpp"

$(IntermediateDirectory)/targetcxx.cpp$(ObjectSuffix): targetcxx.cpp $(IntermediateDirectory)/targetcxx.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetcxx.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetcxx.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetcxx.cpp$(DependSuffix): targetcxx.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetcxx.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetcxx.cpp$(DependSuffix) -MM "targetcxx.cpp"

$(IntermediateDirectory)/targetcxx.cpp$(PreprocessSuffix): targetcxx.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetcxx.cpp$(PreprocessSuffix) "targetcxx.cpp"

$(IntermediateDirectory)/resourceobject.cpp$(ObjectSuffix): resourceobject.cpp $(IntermediateDirectory)/resourceobject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/resourceobject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/resourceobject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/resourceobject.cpp$(DependSuffix): resourceobject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/resourceobject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/resourceobject.cpp$(DependSuffix) -MM "resourceobject.cpp"

$(IntermediateDirectory)/resourceobject.cpp$(PreprocessSuffix): resourceobject.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/resourceobject.cpp$(PreprocessSuffix) "resourceobject.cpp"

$(IntermediateDirectory)/targetcxxoptions.cpp$(ObjectSuffix): targetcxxoptions.cpp $(IntermediateDirectory)/targetcxxoptions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetcxxoptions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetcxxoptions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetcxxoptions.cpp$(DependSuffix): targetcxxoptions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetcxxoptions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetcxxoptions.cpp$(DependSuffix) -MM "targetcxxoptions.cpp"

$(IntermediateDirectory)/targetcxxoptions.cpp$(PreprocessSuffix): targetcxxoptions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetcxxoptions.cpp$(PreprocessSuffix) "targetcxxoptions.cpp"

$(IntermediateDirectory)/targetcxxpptokenizer.cpp$(ObjectSuffix): targetcxxpptokenizer.cpp $(IntermediateDirectory)/targetcxxpptokenizer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/targetcxxpptokenizer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/targetcxxpptokenizer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/targetcxxpptokenizer.cpp$(DependSuffix): targetcxxpptokenizer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/targetcxxpptokenizer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/targetcxxpptokenizer.cpp$(DependSuffix) -MM "targetcxxpptokenizer.cpp"

$(IntermediateDirectory)/targetcxxpptokenizer.cpp$(PreprocessSuffix): targetcxxpptokenizer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/targetcxxpptokenizer.cpp$(PreprocessSuffix) "targetcxxpptokenizer.cpp"

$(IntermediateDirectory)/pipe-posix.cpp$(ObjectSuffix): pipe-posix.cpp $(IntermediateDirectory)/pipe-posix.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/pipe-posix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/pipe-posix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/pipe-posix.cpp$(DependSuffix): pipe-posix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/pipe-posix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/pipe-posix.cpp$(DependSuffix) -MM "pipe-posix.cpp"

$(IntermediateDirectory)/pipe-posix.cpp$(PreprocessSuffix): pipe-posix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/pipe-posix.cpp$(PreprocessSuffix) "pipe-posix.cpp"

$(IntermediateDirectory)/thread-posix.cpp$(ObjectSuffix): thread-posix.cpp $(IntermediateDirectory)/thread-posix.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/thread-posix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/thread-posix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/thread-posix.cpp$(DependSuffix): thread-posix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/thread-posix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/thread-posix.cpp$(DependSuffix) -MM "thread-posix.cpp"

$(IntermediateDirectory)/thread-posix.cpp$(PreprocessSuffix): thread-posix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/thread-posix.cpp$(PreprocessSuffix) "thread-posix.cpp"

$(IntermediateDirectory)/fileutils-linuxgnu.cpp$(ObjectSuffix): fileutils-linuxgnu.cpp $(IntermediateDirectory)/fileutils-linuxgnu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/fileutils-linuxgnu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fileutils-linuxgnu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fileutils-linuxgnu.cpp$(DependSuffix): fileutils-linuxgnu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fileutils-linuxgnu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/fileutils-linuxgnu.cpp$(DependSuffix) -MM "fileutils-linuxgnu.cpp"

$(IntermediateDirectory)/fileutils-linuxgnu.cpp$(PreprocessSuffix): fileutils-linuxgnu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fileutils-linuxgnu.cpp$(PreprocessSuffix) "fileutils-linuxgnu.cpp"

$(IntermediateDirectory)/systemtargetinfo.cpp$(ObjectSuffix): systemtargetinfo.cpp $(IntermediateDirectory)/systemtargetinfo.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/systemtargetinfo.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/systemtargetinfo.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/systemtargetinfo.cpp$(DependSuffix): systemtargetinfo.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/systemtargetinfo.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/systemtargetinfo.cpp$(DependSuffix) -MM "systemtargetinfo.cpp"

$(IntermediateDirectory)/systemtargetinfo.cpp$(PreprocessSuffix): systemtargetinfo.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/systemtargetinfo.cpp$(PreprocessSuffix) "systemtargetinfo.cpp"

$(IntermediateDirectory)/parametersetdefault.cpp$(ObjectSuffix): parametersetdefault.cpp $(IntermediateDirectory)/parametersetdefault.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/torbjorr/Dokument/maike/parametersetdefault.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/parametersetdefault.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/parametersetdefault.cpp$(DependSuffix): parametersetdefault.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/parametersetdefault.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/parametersetdefault.cpp$(DependSuffix) -MM "parametersetdefault.cpp"

$(IntermediateDirectory)/parametersetdefault.cpp$(PreprocessSuffix): parametersetdefault.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/parametersetdefault.cpp$(PreprocessSuffix) "parametersetdefault.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


