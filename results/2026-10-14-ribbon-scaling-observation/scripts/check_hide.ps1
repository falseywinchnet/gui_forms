# Checks how WordPad's ribbon looks at small client sizes: the dock window's
# rectangle and visibility, the MSAA lower ribbon, and a capture of the window.
param([string]$Out)
. (Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'common.ps1')
[RibbonObserveWin32]::UsePhysicalPixels()
New-Item -ItemType Directory -Force $Out | Out-Null
$p = Start-Process -FilePath 'C:\Program Files\Windows NT\Accessories\wordpad.exe' -PassThru
$h = Get-MainWindow $p.Id 'WordPadClass'
if ($h -eq [IntPtr]::Zero) { throw 'no main window' }
Wait-Settled $p
$placement = [RibbonObserveWin32]::Placement($h)
try {
    $netui = [RibbonObserveWin32]::FindDescendantWindow($h, 'NetUIHWND')
    foreach ($size in @(@(1600,1200), @(1600,400), @(1600,300), @(1600,240), @(1600,200), @(600,300), @(300,1200), @(300,300))) {
        $c = [RibbonObserveWin32]::SetClientSize($h, $size[0], $size[1]); Wait-Settled $p; Start-Sleep -Milliseconds 300
        $c = [RibbonObserveWin32]::ClientRect($h)
        $dock = [RibbonObserveWin32]::DockOf($h, $netui)
        $rows = [RibbonObserveWin32]::AccTree($netui, 2)
        $lower = $rows | Where-Object { $_.Name -eq 'Lower Ribbon' } | Select-Object -First 1
        "{0}x{1}: client={2} dock={3} lower=state 0x{4:X} {5},{6},{7},{8}" -f $size[0], $size[1], ($c -join ','), ($dock -join ','), $lower.State, $lower.X, $lower.Y, $lower.W, $lower.H | Add-Content (Join-Path $Out 'hide_check.txt')
        [RibbonObserveWin32]::CaptureTop($h, (Join-Path $Out ("hide_{0}x{1}.png" -f $size[0], $size[1])), 2000)
    }
} finally {
    [RibbonObserveWin32]::Restore($h, $placement); Start-Sleep -Milliseconds 300
    [RibbonObserveWin32]::Close($h); [void]$p.WaitForExit(15000)
}
