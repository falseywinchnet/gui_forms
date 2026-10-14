# Starts Word 2010 as the observation did (/w /q /a), reports whether it opens
# maximized as it did before the observation, maximizes it if it does not,
# and closes it without saving.
param([string]$Out)
. (Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'common.ps1')
[RibbonObserveWin32]::UsePhysicalPixels()
Add-Type -Namespace RibbonObserve -Name Show -MemberDefinition '[DllImport("user32.dll")] public static extern bool ShowWindow(System.IntPtr h, int cmd);'
$p = Start-Process -FilePath 'C:\Program Files\Microsoft Office\Office14\WINWORD.EXE' -ArgumentList '/w','/q','/a' -PassThru
$h = Get-MainWindow $p.Id 'OpusApp' 40000
Start-Sleep -Seconds 4
$others = @(Get-OtherWindows $p.Id $h | Where-Object { $_.Class -ne 'MsoSplash' })
if ($h -eq [IntPtr]::Zero -or $others.Count -gt 0) { Stop-Process -Id $p.Id -Force; "prompt or no window; stopped"; exit 2 }
$pl = [RibbonObserveWin32]::Placement($h)
"at start: showCmd=$($pl.showCmd) window=$([RibbonObserveWin32]::WindowRect($h) -join ',')"
if ($pl.showCmd -ne 3) {
    [void][RibbonObserve.Show]::ShowWindow($h, 3)
    Start-Sleep -Milliseconds 800
    "maximized: window=$([RibbonObserveWin32]::WindowRect($h) -join ',')"
}
[RibbonObserveWin32]::Close($h)
if ($p.WaitForExit(15000)) { 'closed' } else { 'still running' }
