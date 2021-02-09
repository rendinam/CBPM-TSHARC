'======================================================
' POST-COMPILE Script                                 |
'                                                     |
' This VBScript backs up the makefile for the project |
' that was just built so that the .ldr filename       |
' finder grabs the correct information.               |
'======================================================


' Get the name of the currently-active makefile for the project.
'----------------------------------------------------------------
Set makFSO = CreateObject("Scripting.FileSystemObject")
set folder = makFSO.getfolder(".")
For each file in folder.files
   If makFSO.GetExtensionName(file) = "mak" Then
      Makefile = file     
   End If
Next
'Set makFile = makFSO.OpenTextFile( Makefile )



set rencmd = CreateObject("Scripting.FileSystemObject")
if rencmd.FileExists(Makefile) Then
   rencmd.CopyFile Makefile, Makefile & ".BACKUP"
   rencmd.DeleteFile Makefile
End If