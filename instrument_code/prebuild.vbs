'======================================================
' This VBScript polls the system and various files    |
' within the project to construct a unique firmware   |
' ID stamp.                                           |
'======================================================
' You must have the Visual DSP installation directory
' as part of your %PATH% environment variable definition
' before this will run correctly.  Specifically, the
' path where the VDSP compiler of choice lives needs to
' be in your path. 
'  (For TigerSHARC processors, this is
'   'ccts.exe')
'======================================================


dim LineArray
dim Position
dim Examine
dim D
Dim arrFIleLines() ' Array to hold all lines of file to be parsed
Examine = 0
D = "#define"      ' Text insertion string literal

Nowval = Now
'--------------------------------------------------------------------

'-------------------------------------------
' Set hooks into the header file to modify
'-------------------------------------------
Set headerFSO  = CreateObject("Scripting.FileSystemObject")
Set headerFile = headerFSO.OpenTextFile("../include/cbpm_versions.h", 1) ' For reading


'-------------------------------------------
' Set hooks into makefile to get LDR name
'-------------------------------------------
Set makFSO = CreateObject("Scripting.FileSystemObject")
set folder = makFSO.getfolder(".")
For each file in folder.files
   If makFSO.GetExtensionName(file) = "mak" Then
      Makefile = file     
   End If
Next
Set makFile = makFSO.OpenTextFile( Makefile )


'-------------------------------------------
' Set hooks into OUTPUT file
'-------------------------------------------
set outputFSO  = CreateObject("Scripting.FileSystemObject")
strFileName    = outputFSO.GetTempName
Set objoutFile = outputFSO.CreateTextFile("OUT.txt",2) ' For writing


'--------------------------------------------------------------------


'----------------------------------------------------
'Read the lines from the file into a list in memory	
'----------------------------------------------------
i = 0
DO Until headerFile.AtEndOfStream
	Redim Preserve arrFileLines(i)
	arrFileLines(i) = headerFile.ReadLine
	i = i + 1
Loop
headerFile.Close


'----------------------------
' Get the loader file name
'----------------------------
i = 0
DO Until makFile.AtEndOfStream
	'Redim Preserve arrFileLines(i)
	'arrFileLines(i) = headerFile.ReadLine
	makline = makFile.Readline
	If Instr(makline, ".ldr") > 0 Then
	   Exit Do   
	End If
	i = i + 1
Loop
LineArray = Split(makline, "/")
LDR_NAME = LineArray( UBound(LineArray) )
LDR_NAME = Trim( LDR_NAME )
LDR_NAME = Chr(34) & LDR_NAME & Chr(34)
makFile.Close


'----------------------------
' Get compiler version info
'----------------------------
Set objShell      = CreateObject("WScript.Shell")
Set objScriptExec = objShell.Exec("c:\Program Files\Analog Devices\VisualDSP 5.0\ccts -v")
output            = objScriptExec.StdOut.ReadAll
WScript.Echo output


COMPILER_VERSION = 1

'----------------------------
' Get Build Time stamp
'----------------------------
YRVAL = Year(Nowval)-2000
If YRVAL < 10 Then
	YRVAL = Year(Nowval)-2000
End If
 
MOVAL = Month(Nowval) 
If MOVAL < 10 Then
	MOVAL = 0 & Month(Nowval)
End If

DAYVAL = Day(Nowval)
If DAYVAL < 10 Then
	DAYVAL = 0 & Day(Nowval)
End If

HRVAL = Hour(Nowval)
If HRVAL < 10 Then
	HRVAL = 0 & Hour(Nowval)
End If
	
MINVAL = Minute(Nowval)	
If MINVAL < 10 Then
	MINVAL = 0 & Minute(Nowval)
End If

BUILD_TIMESTAMP  = YRVAL & MOVAL & DAYVAL & HRVAL & MINVAL



'----------------------------
' Set DSP code type
'----------------------------
INST_CODE_TYPE    = 2



'-----------------------------------------------------------------------
' Process lines of file
'-----------------------------------------------------------------------
Examine = 0
' Loop over the lines read from the file and replace text as necessary
for line = LBound(arrFileLines) to UBound(arrFileLines) Step 1
	If Examine > 0 Then ' If this is not the first line in file
	   LineArray = Split(arrFileLines(line))	      
	   If Instr(arrFileLines(line), "#define") Then 
	      if Instr(arrFileLines(line), "LDR_NAME") > 0 Then
	         'Wscript.Echo D, LineArray(1), LDR_NAME
	         objoutFile.WriteLine(D &" "& LineArray(1) &" "& LDR_NAME)
	      ElseIf Instr(arrFileLines(line), "COMPILER_VERSION") > 0 Then
             'Wscript.Echo D, LineArray(1), COMPILER_VERSION	
             objoutFile.WriteLine(D &" "& LineArray(1) &" "& COMPILER_VERSION)
	      Elseif Instr(arrFileLines(line), "BUILD_TIMESTAMP") > 0 Then
	         'Wscript.Echo D, LineArray(1), BUILD_TIMESTAMP
	         objoutFile.WriteLine(D &" "& LineArray(1) &" "& BUILD_TIMESTAMP)
	      ElseIf Instr(arrFileLines(line), "INST_CODE_TYPE") > 0 Then
	         'Wscript.Echo D, LineArray(1), INST_CODE_TYPE
	         objoutFile.WriteLine(D &" "& LineArray(1) &" "& INST_CODE_TYPE)
	      Else
	         'Wscript.Echo arrFileLines(line) 
	         objoutFile.WriteLine( arrFileLines(line) )
	      End If  
	   Else
	      'Wscript.Echo arrFileLines(line)
	      objoutFile.WriteLine( arrFileLines(line) )
	   End If
	Else ' Ignore processing of first line of file, just pass it on.
	   'Wscript.Echo arrFileLines(line) ' To print FIRST line
	   objoutFile.WriteLine( arrFileLines(line) )
	   Examine = 1
	End If
Next


set copycmd = CreateObject("Scripting.FileSystemObject")
if copycmd.FileExists("OUT.txt") Then
   copycmd.CopyFile "..\include\cbpm_versions.h", "..\include\cbpm_versions.h.BACKUP"
   copycmd.DeleteFile "..\include\cbpm_versions.h"
   copycmd.CopyFile ".\OUT.txt", "..\include\cbpm_versions.h"
End If




