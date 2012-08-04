' -------------------------------------------
' Tests STLport on Pocket PC or Emulator
'
' (C) 2004, 2005 Michael Fink
' -------------------------------------------
Option Explicit

Dim WshShell
Set WshShell = WScript.CreateObject("WScript.Shell")


Dim oPlatformManager, oPlatform, oDevice, oConnection

' opens connection to device on platform
Sub OpenConnection(szPlatformName, szDeviceName)
   Set oPlatformManager = CreateObject("PlatformManager.PlatformManager.1")
   Set oPlatform = oPlatformManager.GetPlatform(szPlatformName)
   Set oDevice = oPlatform.GetDevice(szDeviceName)
   Set oConnection = oDevice.Attach("cemgr", 10000)
End Sub

' closes connection and cleans up all stuff
Sub CloseConnection()
   Set oConnection = Nothing
   Set oDevice = Nothing
   Set oPlatform = Nothing
   Set oPlatformManager = Nothing
End Sub

' uploads file to device
Sub UploadFile(szLocalFilename, szRemoteFilename)
   oConnection.FileCopy szLocalFilename, szRemoteFilename, 1
End Sub

' downloads file from device
Sub DownloadFile(szRemoteFilename, szLocalFilename)
   Dim fso
   Set fso = CreateObject("Scripting.FileSystemObject")

   On Error Resume Next
   fso.DeleteFile(szLocalFilename)
   On Error GoTo 0

   oConnection.GetFile szRemoteFilename, szLocalFilename
End Sub

' deletes file on device
Sub DeleteRemoteFile(szRemoteFilename)
   On Error Resume Next
   oConnection.FileDelete szRemoteFilename
   On Error GoTo 0
End Sub

' executes process on device
Sub ExecRemoteProcess(szRemoteExecutable, szCmdLine)
   oConnection.Launch szRemoteExecutable, szCmdLine
End Sub

' lists all platforms and devices available through Platform Manager
Sub ListPlatformsAndDevices()

   Dim oPlatformManager, ePlatforms, eDevices, szText

   Set oPlatformManager = CreateObject("PlatformManager.PlatformManager.1")

   Set ePlatforms = oPlatformManager.EnumPlatforms()
   For Each oPlatform in ePlatforms
      szText = szText & oPlatform.Name & vbCrLf

      Set eDevices = oPlatform.EnumDevices()
      For Each oDevice in eDevices
         szText = szText & "   " & oDevice.Name & vbCrLf
      Next
   Next

   MsgBox szText

   oPlatformManager = Nothing

End Sub

' helps testing STLport by uploading and running tests and downloading test results
Sub TestHelper(szPlatform, szTarget, szDllName, szExeName)

   if (szDllName <> "") Then
      WScript.Echo "Uploading file bin\" & szPlatform & "\" & szDllName
      UploadFile WshShell.CurrentDirectory & "\..\bin\" & szPlatform & "\" & szDllName, "\" & szDllName
   End If

   WScript.Echo "Uploading file bin\" & szPlatform & "\" & szExeName
   UploadFile WshShell.CurrentDirectory & "\..\bin\" & szPlatform & "\" & szExeName, "\" & szExeName

   WScript.Echo "Executing \" & szExeName & " -f=\" & szPlatform & "_" & szTarget & ".txt"
   ExecRemoteProcess "\" & szExeName, "-f=\" & szPlatform & "_" & szTarget & ".txt"
   WScript.Sleep 120000

   WScript.Echo "Downloading \" & szPlatform & "_" & szTarget & ".txt"
   DownloadFile "\" & szPlatform & "_" & szTarget & ".txt", WshShell.CurrentDirectory & "\" & szPlatform & "_" & szTarget & ".txt"

   WScript.Echo "Cleaning up device..."
   if (szDllName <> "") Then
      DeleteRemoteFile "\" & szDllName
   End If

   DeleteRemoteFile "\" & szExeName
   DeleteRemoteFile "\" & szPlatform & "_" & szTarget & ".txt"

End Sub


' main script

If WScript.Arguments.Count <> 2 Then
   WScript.Quit 1

End If

Dim szPlatform, szTarget
szPlatform = WScript.Arguments(0)
szTarget = WScript.Arguments(1)

WScript.Echo "Opening Connection..."

' open proper connection
if szPlatform = "evc3-arm" Then
   OpenConnection "Pocket PC 2002", "Pocket PC 2002 (Default Device)"

ElseIf szPlatform = "evc3-x86" Then
   OpenConnection "Pocket PC 2002", "Pocket PC 2002 Emulation"

ElseIf szPlatform = "evc4-arm" Then
   OpenConnection "POCKET PC 2003", "POCKET PC 2003 Device"

ElseIf szPlatform = "evc4-x86" Then
   OpenConnection "POCKET PC 2003", "POCKET PC 2003 Emulator"

Else
   MsgBox "Wrong Target: " & WScript.Arguments(0)
   WScript.Quit 1

End If

' determine files to upload
Dim szLibFile, szTestFile
szLibFile = ""

If szTarget = "release-shared" Then
   szLibFile = "stlport_r50.dll"
   szTestFile = "stl_unit_test_r50.exe"

ElseIf szTarget = "dbg-shared" Or szTarget = "debug-shared" Then
   szLibFile = "stlport_d50.dll"
   szTestFile = "stl_unit_test_d50.exe"

ElseIf szTarget = "stldbg-shared" Or szTarget = "stldebug-shared" Then
   szLibFile = "stlport_stld50.dll"
   szTestFile = "stl_unit_test_stld50.exe"

ElseIf szTarget = "release-static" Then
   szLibFile = ""
   szTestFile = "stl_unit_test_static_r50.exe"

ElseIf szTarget = "dbg-static" Or szTarget = "debug-static"  Then
   szLibFile = ""
   szTestFile = "stl_unit_test_static_d50.exe"

ElseIf szTarget = "stldbg-static" Or szTarget = "stldebug-static"  Then
   szLibFile = ""
   szTestFile = "stl_unit_test_static_stld50.exe"

End If

' test STLport
TestHelper szPlatform, szTarget, szLibFile, szTestFile

WScript.Echo "Closing Connection..."

CloseConnection
