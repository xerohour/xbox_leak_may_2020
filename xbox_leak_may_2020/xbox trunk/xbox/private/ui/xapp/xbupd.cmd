@echo off

setlocal
set XDASH_PARTITION=xy:\

SET __XBCPCOMMAND=/d
if "%1"=="forceall" SET __XBCPCOMMAND=

:update
echo Updating media files...
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Default.xip"             %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Keyboard.xip"            %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\JKeyboard.xip"            %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Message.xip"             %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\DVD.xip"                 %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\MainMenu5.xip"           %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Memory2.xip"             %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Memory_Files2.xip"       %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Music2.xip"              %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Music_PlayEdit2.xip"     %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Music_Copy3.xip"         %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings3.xip"           %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_Clock.xip"      %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_Timezone.xip"   %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_List.xip"       %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_Video.xip"      %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_Language.xip"   %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_Panel.xip"      %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\Settings_Parental.xip"   %XDASH_PARTITION%

xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\XBox Book.xtf"           %XDASH_PARTITION%
xbcp %__XBCPCOMMAND% /f "%_NTBINDIR%\private\ui\dash\XBox.xtf"                %XDASH_PARTITION%

xbcp %__XBCPCOMMAND% /t /f "%_NTBINDIR%\private\ui\dash\Audio\AmbientAudioADPCM\*.wav"     %XDASH_PARTITION%Audio\AmbientAudio
xbcp %__XBCPCOMMAND% /t /f "%_NTBINDIR%\private\ui\dash\Audio\TransitionAudioADPCM\*.wav"  %XDASH_PARTITION%Audio\TransitionAudio
xbcp %__XBCPCOMMAND% /t /f "%_NTBINDIR%\private\ui\dash\Audio\MusicAudioADPCM\*.wav"       %XDASH_PARTITION%Audio\MusicAudio
xbcp %__XBCPCOMMAND% /t /f "%_NTBINDIR%\private\ui\dash\Audio\MainAudioADPCM\*.wav"        %XDASH_PARTITION%Audio\MainAudio
xbcp %__XBCPCOMMAND% /t /f "%_NTBINDIR%\private\ui\dash\Audio\MemoryAudioADPCM\*.wav"      %XDASH_PARTITION%Audio\MemoryAudio
xbcp %__XBCPCOMMAND% /t /f "%_NTBINDIR%\private\ui\dash\Audio\SettingsAudioADPCM\*.wav"    %XDASH_PARTITION%Audio\SettingsAudio

echo Updating XApp...
xbcp /f %__XBCPCOMMAND% /t %_NT386TREE%\dump\xboxdash.xbe xy:\xboxdash.xbe
xbcp /f %__XBCPCOMMAND% /t %_NT386TREE%\dump\dvdkey1d.bin xy:\dvdkeyd.bin
if %_NTUSER% == schanbai xbcp /f %__XBCPCOMMAND% /t %_NT386TREE%\dump\xboxdash.pdb xy:\xboxdash.pdb

endlocal
