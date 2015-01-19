;;;;;;;;;;;;;;;;;;;;;;;
;File:		DSAM_SDK.nsi
;Purpose:	Create the DSAM SDK required to compile DSAM programs
;Comments:	This script is based on NSIS example2.nsi
;			The "SetOutPath" is needed for the writing of the uninstaller and
;			and also the DLL install - even though the full directory is given
;			in the instruction line.
;Author:	L. P. O'Mard
;Created:	9 Mar 2009
;Updated:
;Copyright:	(c) 2010 Lowel P. O'Mard
;
;;;;;;;;;;;;;;;;;;;;;;;
;
;  This file is part of DSAM.
;
;  DSAM is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  DSAM is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
;
;;;;;;;;;;;;;;;;;;;;;;;

!addincludedir "NSIS"
;!include "LogicLib.nsh"
!include "Library.nsh"

!include "MUI2.nsh"		; Use Modern UI
!define	ALL_USERS		; For WriteEnvStr.nsh 
!include "WriteEnvStr.nsh"
!include "DSAMGenUtils.nsh"

!define PACKAGE_NAME	"SDK"
!define VERSION		"2.8.44"
!define INST_VERSION	"1"
!define DSAMDIR_ENV	"DSAMSDKDIR"

;--------------------------------

; The name of the installer
Name "DSAM ${PACKAGE_NAME} ${VERSION}"

; The file to write
OutFile "DSAM_${PACKAGE_NAME}_${VERSION}_${INST_VERSION}.exe"

; The default installation directory
; Currently the SDK has to be installed to the c drive to avoid
; file names with spaces for the "mex" compile command.
InstallDir "C:\DSAM\SDK"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM ${DSAM_SDK_RKEY} ${INSTALL_DIR}

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------

; Pages

; License page - replaces: Page license
PageEx license
	LicenseText "DSAM ${PACKAGE_NAME} License"
	LicenseData LICENSE
PageExEnd

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

; The stuff to install
Section "DSAM ${PACKAGE_NAME} ${VERSION}"

  SectionIn RO

  DetailPrint "Platform architecture is $platformArch-bit"
  ; Sets the registry view for x64 systems.
  ${iF} $platformArch = 64
  	SetRegView 64
  ${EndIf}

  ; Write the installation path into the registry
  WriteRegStr HKLM "{DSAM_SDK_RKEY}" "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_${PACKAGE_NAME}" "DisplayName" "DSAM ${PACKAGE_NAME} ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_${PACKAGE_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_${PACKAGE_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_${PACKAGE_NAME}" "NoRepair" 1
  ; Set output path to the install directory.
  SetOutPath $INSTDIR
  WriteUninstaller "uninstall.exe"

  ; Set Environment variable
  Push "${DSAMDIR_ENV}"
  Push "$INSTDIR"
  Call WriteEnvStr

SectionEnd

Section "DSAM Library ${VERSION}"

  SectionIn RO
  
  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\dsam

  File Include\*.h
  File Include\*.rc
  File DSAMSetup.h

  ; Set output path to the include dsam bitmap directory.
  SetOutPath $INSTDIR\include\dsam\Bitmaps

  File  /x *.am /x *.in "src\GrLib\Bitmaps\*.*"

  ; Set output path to the lib directory and place respective files.
  ${If} $platformArch = 32
    SetOutPath $INSTDIR\lib\x86
    File lib\*x86.lib
  ${Else}
    SetOutPath $INSTDIR\lib\x64
    File lib\*x64.lib
  ${EndIf}

  Call InstallDSAMDLLs
  
SectionEnd

Section "WXWIN Library ${WX_VERSION}"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\wxwin

  ; Put file there
  File /r ${WXWINDIR}\Include\*.*
  File /r ${WXWINDIR}\contrib\Include\*.h
  File /r ${WXWINDIR}\lib\vc_dll\mswu\*.h

  ; Set output path to the lib directory and place respective files.
  ${If} $platformArch = 32
    SetOutPath $INSTDIR\lib\x86
    File ${WXWINDIR}\lib\vc_dll\*.lib
  ${Else}
    SetOutPath $INSTDIR\lib\x64
    File ${WXWINDIR}\lib\vc_x64_dll\*.lib
  ${EndIf}
  
  Call InstallWxWinDLLs

SectionEnd

Section "LIBSNDFILE Library ${LSF_VERSION}"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\libsndfile

  ; Put file there
  File /r ${LIBSNDFILEDIR}\src\*.h

  ; Set output path to the lib directory and place respective files.
  ${If} $platformArch = 32
    SetOutPath $INSTDIR\lib\x86
    File ${LIBSNDFILEDIR}\MSVC\Release\libsndfile_x86.lib
  ${Else}
    SetOutPath $INSTDIR\lib\x64
    File ${LIBSNDFILEDIR}\MSVC\x64\Release\libsndfile_x64.lib
  ${EndIf}

  Call InstallLibSndFileDLL
  
SectionEnd

Section "PORTAUDIO Library ${PA_VERSION}"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\portaudio

  ; Put file there
  File /r ${PORTAUDIODIR}\include\*.h

  ; Set output path to the lib directory and place respective files.
  ${If} $platformArch = 32
    SetOutPath $INSTDIR\lib\x86
    File ${PORTAUDIODIR}\build\msvc\Release\*.lib
  ${Else}
    SetOutPath $INSTDIR\lib\x64
    File ${PORTAUDIODIR}\build\msvc\x64\Release\*.lib
  ${EndIf}

  Call InstallPortAudioDLL

SectionEnd

Section "FFTW Library ${FFTW_VERSION}"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\fftw

  ; Put file there
  File /r ${FFTWDIR}\x86\*.h

  ; Set output path to the lib directory and place respective files.
  ${If} $platformArch = 32
    SetOutPath $INSTDIR\lib\x86
    File ${FFTWDIR}\x86\libfftw3-3.lib
  ${Else}
    SetOutPath $INSTDIR\lib\x64
    File ${FFTWDIR}\x64\libfftw3-3.lib
  ${EndIf}

  Call InstallLibFFTWDLL

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  GetVersion::WindowsPlatformArchitecture
  Pop $platformArch 
  ; Sets the registry view for x64 systems.
  ${If} $platformArch = 64
  	SetRegView 64
  ${EndIf}

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_${PACKAGE_NAME}"
  DeleteRegKey HKLM ${DSAM_SDK_RKEY}

  ; Remove environment variables
  # remove the variable
  Push "${DSAMDIR_ENV}"
  Call un.DeleteEnvStr

  ; Remove files and uninstaller
  RMDIR /r /REBOOTOK $INSTDIR\include
  RMDIR /r /REBOOTOK $INSTDIR\lib\x86
  RMDIR /r /REBOOTOK $INSTDIR\lib\x64
  Delete $INSTDIR\uninstall.exe

  ; Remove DLLs
  Call un.SetDLLUnInstall

  ; Remove directories used
  RMDir /REBOOTOK "$INSTDIR\LIB" ; - remove if empty.
  RMDir /REBOOTOK "$INSTDIR" ; - remove if empty.

SectionEnd

;--------------------------------

;Descriptions

  ;Language strings
#  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
#  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
#    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
#  !insertmacro MUI_FUNCTION_DESCRIPTION_END


