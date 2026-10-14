# Closes a WordPad process this observation started: puts its window at the
# given physical rectangle, selects Home, and sends WM_CLOSE (no document was
# edited, so WordPad closes without asking).
param([int]$ProcessId, [string]$Rect)
. (Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'common.ps1')
[RibbonObserveWin32]::UsePhysicalPixels()
$p = Get-Process -Id $ProcessId
$h = Get-MainWindow $p.Id 'WordPadClass' 5000
if ($h -eq [IntPtr]::Zero) { throw 'main window not found' }
$netui = [RibbonObserveWin32]::FindDescendantWindow($h, 'NetUIHWND')
if ($netui -ne [IntPtr]::Zero) { [void][RibbonObserveWin32]::DoDefaultAction($netui, 'Home', 37) }
$r = $Rect.Split(',') | ForEach-Object { [int]$_ }
[RibbonObserveWin32]::SetWindowRect($h, $r[0], $r[1], $r[2], $r[3])
Start-Sleep -Milliseconds 500
"window before close: $([RibbonObserveWin32]::WindowRect($h) -join ',')"
[RibbonObserveWin32]::Close($h)
if ($p.WaitForExit(15000)) { 'closed' } else { 'still running' }
