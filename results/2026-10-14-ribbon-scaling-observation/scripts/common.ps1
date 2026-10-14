# Shared helpers for the ribbon scaling observation (PowerShell 5.1, built-in
# assemblies only). Acts only on windows of the process id it is given.
$ErrorActionPreference = 'Stop'
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
Add-Type -AssemblyName UIAutomationClient, UIAutomationTypes, System.Drawing
if (-not ('RibbonObserveWin32' -as [type])) {
    Add-Type -Path (Join-Path $here 'Win32Window.cs') -ReferencedAssemblies System.Drawing, Accessibility
}
$UIA = [System.Windows.Automation.AutomationElement]
$TS = [System.Windows.Automation.TreeScope]

function Get-MainWindow([int]$ProcessId, [string]$ClassName, [int]$TimeoutMs = 20000) {
    $deadline = (Get-Date).AddMilliseconds($TimeoutMs)
    while ((Get-Date) -lt $deadline) {
        foreach ($h in [RibbonObserveWin32]::VisibleTopLevelWindows($ProcessId)) {
            if ([RibbonObserveWin32]::ClassOf($h) -eq $ClassName) { return $h }
        }
        Start-Sleep -Milliseconds 200
    }
    return [IntPtr]::Zero
}

function Get-OtherWindows([int]$ProcessId, [IntPtr]$Main) {
    foreach ($h in [RibbonObserveWin32]::VisibleTopLevelWindows($ProcessId)) {
        if ($h -ne $Main) {
            [pscustomobject]@{ Handle = $h; Class = [RibbonObserveWin32]::ClassOf($h); Title = [RibbonObserveWin32]::TitleOf($h) }
        }
    }
}

function Wait-Settled([System.Diagnostics.Process]$Process) {
    try { [void]$Process.WaitForInputIdle(2000) } catch {}
    Start-Sleep -Milliseconds 250
}

# Every UI Automation element under `root`, flattened, with depth.
function Get-UiaTree($root, [int]$MaxDepth = 12) {
    $walker = [System.Windows.Automation.TreeWalker]::ControlViewWalker
    $rows = New-Object System.Collections.Generic.List[object]
    $stack = New-Object System.Collections.Generic.Stack[object]
    $stack.Push(@($root, 0))
    while ($stack.Count -gt 0) {
        $pair = $stack.Pop(); $e = $pair[0]; $d = $pair[1]
        $c = $e.Current
        $r = $c.BoundingRectangle
        $typeName = $c.ControlType.ProgrammaticName.Replace('ControlType.', '')
        $x = 0; $y = 0; $w = 0; $hh = 0
        if (-not $r.IsEmpty -and -not [double]::IsInfinity($r.X)) { $x = [int]$r.X; $y = [int]$r.Y; $w = [int]$r.Width; $hh = [int]$r.Height }
        $rows.Add([pscustomobject]@{
            Depth = $d; Name = $c.Name; Type = $typeName;
            ClassName = $c.ClassName; Id = $c.AutomationId; Offscreen = $c.IsOffscreen;
            X = $x; Y = $y; W = $w; H = $hh })
        if ($d -lt $MaxDepth) {
            $children = New-Object System.Collections.Generic.List[object]
            $child = $walker.GetFirstChild($e)
            while ($child -ne $null) { $children.Add($child); $child = $walker.GetNextSibling($child) }
            for ($i = $children.Count - 1; $i -ge 0; $i--) { $stack.Push(@($children[$i], ($d + 1))) }
        }
    }
    return $rows
}
