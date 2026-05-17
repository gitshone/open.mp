; open.mp client installer.
;
; Expected -D flags from CMake:
;   LOADER_EXE      Absolute path to openmp.exe
;   CLIENT_DLL      Absolute path to openmp-client.dll
;   RESOURCES_DIR   Absolute path to Client/Resources/
;
; Build through CMake's ClientInstaller target.

!ifndef LOADER_EXE
  !error "LOADER_EXE not defined; invoke through CMake's ClientInstaller target."
!endif

!define PRODUCT_NAME      "open.mp client"
!define PRODUCT_PUBLISHER "open.mp"
!define PRODUCT_WEB_SITE  "https://open.mp"
!define INSTALL_DIR       "$PROGRAMFILES\open.mp"

Name "${PRODUCT_NAME}"
OutFile "openmp-client-setup.exe"
InstallDir "${INSTALL_DIR}"
RequestExecutionLevel admin

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Client" SEC_CLIENT
  SetOutPath "$INSTDIR"
  File "${LOADER_EXE}"
  File "${CLIENT_DLL}"

  SetOutPath "$INSTDIR\resources"
  File /r "${RESOURCES_DIR}\*.*"

  ; omp:// URL protocol so join links can launch the client.
  WriteRegStr HKCR "omp" "" "URL:open.mp Protocol"
  WriteRegStr HKCR "omp" "URL Protocol" ""
  WriteRegStr HKCR "omp\DefaultIcon" "" "$INSTDIR\openmp.exe,0"
  WriteRegStr HKCR "omp\shell\open\command" "" '"$INSTDIR\openmp.exe" --omp-uri "%1"'

  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openmp-client" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openmp-client" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openmp-client" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openmp-client" "URLInfoAbout" "${PRODUCT_WEB_SITE}"

  CreateDirectory "$SMPROGRAMS\open.mp"
  CreateShortCut "$SMPROGRAMS\open.mp\open.mp client.lnk" "$INSTDIR\openmp.exe"
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\openmp.exe"
  Delete "$INSTDIR\openmp-client.dll"
  Delete "$INSTDIR\uninstall.exe"
  RMDir /r "$INSTDIR\resources"
  RMDir "$INSTDIR"

  DeleteRegKey HKCR "omp"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openmp-client"
  Delete "$SMPROGRAMS\open.mp\open.mp client.lnk"
  RMDir  "$SMPROGRAMS\open.mp"
SectionEnd
