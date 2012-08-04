// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

//-------------------------------------------------------------------------------------------------

#include <efd/PathUtils.h>

using namespace efd;

/*
    The parameters for the test can easily be passed to the executable on the command-line
    using the configuration manager.  Simply add the following for Visual Studio projects:
        "Testing.Solution=$(SolutionName)" "Testing.Config=$(ConfigurationName)" 
        "Testing.Project=$(ProjectName)" "Testing.Script=<Name>" "Testing.ProcIndex=<ID>"
*/

/// The header in the configuration manager that contains the value for this service
static const char* kFilePrefix = "TestResults";
/// The header in the configuration manager that contains the value for this service
static const char* kConfigHeader = "Testing";
/// The key name for the solution name
static const char* kConfigSolution = "Solution";
/// The key name for the configuration name
static const char* kConfigConfiguration = "Config";
/// The key name for the project name
static const char* kConfigProject = "Project";
/// The key name for the script engine name
static const char* kConfigScript = "ScriptEngine";
/// The key name for the process id
static const char* kConfigProcID = "ProcIndex";
/// The key name of the extra test name given in the config file
static const char* kConfigExtraName = "TestName";
/// The string names for the test results
static const char* kResultNames[kResultMax+1] = {
    "Failed", 
    "Timed Out", 
    "Passed", 
    "Skipped", 
    "Needed", 
    "MAX-In"};

//-------------------------------------------------------------------------------------------------
const char* GetResultString(tTestResult result)
{
    return kResultNames[result];
}

//-------------------------------------------------------------------------------------------------
void WriteXMLString(File* pResultsFile, const char* text)
{
    int len = strcspn(text, "<&>");

    while (len > 0)
    {
        pResultsFile->Write(text, len);
        text += len;
        if (text[0] == '<')
        {
            pResultsFile->Write("&lt;", 4);
            text++;
        }
        else if (text[0] == '&')
        {
            pResultsFile->Write("&amp;", 4);
            text++;
        }
        else if (text[0] == '>')
        {
            pResultsFile->Write("&gt;", 4);
            text++;
        }
        len = strcspn(text, "<&>");
    }
}

//-------------------------------------------------------------------------------------------------
File* BaseCreateDataFile(
    efd::utf8string& solution, 
    efd::utf8string& config,
    efd::utf8string& project,
    efd::utf8string& scriptEngine,
    efd::utf8string& procID)
{
    efd::utf8string filename = kFilePrefix;

    if (!solution.empty())
        filename.append("-" + solution);
    if (!config.empty())
        filename.append("-" + config);
    if (!project.empty())
        filename.append("-" + project);
    if (!scriptEngine.empty())
        filename.append("-" + scriptEngine);
    if (!procID.empty())
        filename.append("-" + procID);
    filename.append(".xml");

    PathUtils::EnsureFilePathIsWriteable(filename);

    File* pResultsFile = File::GetFile(filename.c_str(), File::WRITE_ONLY);

    // Check that the file opened
    if (pResultsFile)
    {
        const char* pcHeader = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
        pResultsFile->Write(pcHeader, strlen(pcHeader));

        const char* pcResultsOpen = "<Results solution=\"";
        pResultsFile->Write(pcResultsOpen, strlen(pcResultsOpen));
        if (!solution.empty())
            WriteXMLString(pResultsFile, solution.c_str());
        const char* pcConfig = "\" configuration=\"";
        pResultsFile->Write(pcConfig, strlen(pcConfig));
        if (!config.empty())
            WriteXMLString(pResultsFile, config.c_str());
        const char* pcResultsClosed = "\">\n";
        pResultsFile->Write(pcResultsClosed, strlen(pcResultsClosed));
    }

    return pResultsFile;
}

//-------------------------------------------------------------------------------------------------
void BaseWriteResultData(        
    File* pResultsFile,
    tTestResult result,
    const char* pFile,
    efd::UInt32 line,
    const char* pName,
    const char* pDescription,
    const char* pFunctionArea,
    efd::utf8string& project,
    efd::utf8string& scriptEngine,
    efd::utf8string& procID,
    efd::TimeType startTime,
    efd::TimeType endTime,
    const char* pMessage)
{
    // Make sure the results file is still open
    if (!pResultsFile)
        return;

    EE_ASSERT(pName);
    if (!pName || (strlen(pName) == 0))
        pName = "Unknown";

    // Write the opening tag for the result
    // <Result name="Test Name" value="pass">
    const char* pResultOpen = "\t<Result ";
    pResultsFile->Write(pResultOpen, strlen(pResultOpen));

    const char* pResultName = "name=\"";
    pResultsFile->Write(pResultName, strlen(pResultName));
    WriteXMLString(pResultsFile, pName);
    const char* pResultValue = "\" value=\"";
    pResultsFile->Write(pResultValue, strlen(pResultValue));
    pResultsFile->Write(kResultNames[result], strlen(kResultNames[result]));
    const char* pResultProject = "\" project=\"";
    pResultsFile->Write(pResultProject, strlen(pResultProject));
    if (!project.empty())
        WriteXMLString(pResultsFile, project.c_str());
    const char* pResultScript = "\" scriptengine=\"";
    pResultsFile->Write(pResultScript, strlen(pResultScript));
    if (!scriptEngine.empty())
        WriteXMLString(pResultsFile, scriptEngine.c_str());
    const char* pResultProc = "\" procid=\"";
    pResultsFile->Write(pResultProc, strlen(pResultProc));
    if (!procID.empty())
        WriteXMLString(pResultsFile, procID.c_str());
    const char* pResultEnd = "\">\n";
    pResultsFile->Write(pResultEnd, strlen(pResultEnd));

    if (pDescription && (strlen(pDescription) > 0))
    {
        // Write the description for the test
        // <description>A description of the test</description>
        const char* pDescripStart = "\t\t<description>";
        pResultsFile->Write(pDescripStart, strlen(pDescripStart));
        WriteXMLString(pResultsFile, pDescription);
        const char* pDescripEnd = "</description>\n";
        pResultsFile->Write(pDescripEnd, strlen(pDescripEnd));
    }

    if (pFunctionArea && (strlen(pFunctionArea) > 0))
    {
        // Write the functional area that this test covered
        // <functionalarea>Behavior API</functionalarea>
        const char* pFunctionAreaStart = "\t\t<functionalarea>";
        pResultsFile->Write(pFunctionAreaStart, strlen(pFunctionAreaStart));
        WriteXMLString(pResultsFile, pFunctionArea);
        const char* pFunctionAreaEnd = "</functionalarea>\n";
        pResultsFile->Write(pFunctionAreaEnd, strlen(pFunctionAreaEnd));
    }

    if (pMessage && (strlen(pMessage) > 0))
    {
        // Write the message that was passed along with the result
        // <message>Some message about the result</message>
        const char* pMessageStart = "\t\t<message>";
        pResultsFile->Write(pMessageStart, strlen(pMessageStart));
        WriteXMLString(pResultsFile, pMessage);
        const char* pMessageEnd = "</message>\n";
        pResultsFile->Write(pMessageEnd, strlen(pMessageEnd));
    }

    // Write the message that was passed along with the result
    // <message>Some message about the result</message>
    const char* pTimeStart = "\t\t<time start=\"";
    pResultsFile->Write(pTimeStart, strlen(pTimeStart));
    efd::Char acBuff[64];
    efd::SInt32 numPrinted = efd::Sprintf(acBuff, EE_ARRAYSIZEOF(acBuff), "%f", startTime);
    pResultsFile->Write(acBuff, numPrinted);
    const char* pTimeMiddle = "\" end=\"";
    pResultsFile->Write(pTimeMiddle, strlen(pTimeMiddle));
    numPrinted = efd::Sprintf(acBuff, EE_ARRAYSIZEOF(acBuff), "%f", endTime);
    pResultsFile->Write(acBuff, numPrinted);
    const char* pTimeEnd = "\"/>\n"; 
    pResultsFile->Write(pTimeEnd, strlen(pTimeEnd));

    if (pFile && (strlen(pFile) > 0))
    {
        // Write the file tag and the file and line number
        // <file>
        //  <name>Source.cpp</name>
        //  <line>56</line>
        // </file>
        const char* pFileStart = "\t\t<file>\n\t\t\t<name>";
        pResultsFile->Write(pFileStart, strlen(pFileStart));
        WriteXMLString(pResultsFile, pFile);
        const char* pFileLine = "</name>\n\t\t\t<line>";
        pResultsFile->Write(pFileLine, strlen(pFileLine));
        numPrinted = efd::Sprintf(acBuff, EE_ARRAYSIZEOF(acBuff), "%i", line);
        pResultsFile->Write(acBuff, numPrinted);
        const char* pFileEnd = "</line>\n\t\t</file>\n";
        pResultsFile->Write(pFileEnd, strlen(pFileEnd));
    }

    // Close the result tag
    const char* pcResultClose = "\t</Result>\n";
    pResultsFile->Write(pcResultClose, strlen(pcResultClose));
}

//-------------------------------------------------------------------------------------------------
void BaseCloseDataFile(File* pResultsFile)
{
    if (pResultsFile != NULL)
    {
        // Write the tail of the XML, closing the open tags
        const char* pcResultsClose = "</Results>\n";
        pResultsFile->Write(pcResultsClose, strlen(pcResultsClose));

        // Flush the file
        pResultsFile->Flush();

        // Delete the file handle to close the file
        EE_DELETE pResultsFile;
        pResultsFile = NULL;
    }
}
