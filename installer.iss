#define MyApplicationName "Pitchenga"
#define MyApplicationPublisher "Pitchenga"
#define MyApplicationUrl "https://github.com/pitchenga/pitchenga"
#define MyApplicationExecutableName "Pitchenga.exe"

#ifndef MyApplicationVersion
  #define MyApplicationVersion "1.0.0"
#endif

#ifndef MyBuildDirectory
  #define MyBuildDirectory "cmake-build-release\Pitchenga_artefacts\Release"
#endif

[Setup]
AppId={{D3B3E8C1-8A7E-4B0E-B8A8-8F8C8A8C8A8C}
AppName={#MyApplicationName}
AppVersion={#MyApplicationVersion}
AppPublisher={#MyApplicationPublisher}
AppPublisherURL={#MyApplicationUrl}
AppSupportURL={#MyApplicationUrl}
AppUpdatesURL={#MyApplicationUrl}
DefaultDirName={autopf}\{#MyApplicationName}
DefaultGroupName={#MyApplicationName}
AllowNoIcons=yes
OutputDir=.
OutputBaseFilename=pitchenga-windows-installer
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MyBuildDirectory}\Standalone\Pitchenga.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyBuildDirectory}\VST3\Pitchenga.vst3\*"; DestDir: "{commoncf}\VST3\Pitchenga.vst3"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#MyApplicationName}"; Filename: "{app}\{#MyApplicationExecutableName}"
Name: "{autodesktop}\{#MyApplicationName}"; Filename: "{app}\{#MyApplicationExecutableName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyApplicationExecutableName}"; Description: "{cm:LaunchProgram,{#StringChange(MyApplicationName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent