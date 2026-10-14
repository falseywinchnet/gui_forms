# Ribbon scaling observation (GUI.Forms stage RB, phase 2a).
#
# Starts one program (WordPad, or Word 2010 with a new blank document and
# /a so it loads no add-ins and locks its settings files), and acts only on
# the windows of that process:
#   - sizes its main window's client area along width and height ladders,
#     bisecting every change of ribbon state to the exact pixel, shrinking and
#     growing;
#   - reads the ribbon through MSAA (the NetUI ribbon publishes its groups and
#     controls as IAccessible) at every probe;
#   - captures the top of the window (title bar and ribbon) once per distinct
#     ribbon state, with PrintWindow (the window only);
#   - selects the tabs it observes through their MSAA default action, then
#     selects Home again;
#   - restores the window placement and closes the program without saving.
# It never touches the Quick Access Toolbar or the ribbon's minimized state.
# All sizes are physical pixels (the script is per-monitor DPI aware).
param(
    [ValidateSet('wordpad', 'word')][string]$App,
    [string]$Out,
    [string[]]$Tabs = @('Home'),
    [int]$MaxWidth = 2800, [int]$MinWidth = 300, [int]$Step = 40,
    [int]$LadderHeight = 1200,
    [int[]]$HeightLadderWidths = @(1600, 800, 600),
    [int]$MaxHeight = 1400, [int]$MinHeight = 160, [int]$HeightStep = 20,
    # Physical x,y,width,height to leave the window at before closing, when the
    # placement found at start is not the one to keep (see the README).
    [string]$RestoreRect = '',
    [switch]$SkipWidth
)
. (Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'common.ps1')
[RibbonObserveWin32]::UsePhysicalPixels()
New-Item -ItemType Directory -Force $Out | Out-Null
$log = Join-Path $Out 'run.log'
function Log([string]$text) {
    $line = "{0:HH:mm:ss} {1}" -f (Get-Date), $text
    Write-Host $line
    try { Add-Content $log $line -ErrorAction Stop } catch { Write-Host "(log write failed: $_)" }
}

# -- the program --------------------------------------------------------------

if ($App -eq 'wordpad') {
    $exe = 'C:\Program Files\Windows NT\Accessories\wordpad.exe'; $arguments = @(); $mainClass = 'WordPadClass'
} else {
    $exe = 'C:\Program Files\Microsoft Office\Office14\WINWORD.EXE'; $arguments = @('/w', '/q', '/a'); $mainClass = 'OpusApp'
}
$version = (Get-Item $exe).VersionInfo.FileVersion
Log "start $exe $($arguments -join ' ') (version $version)"
if ($arguments.Count -gt 0) { $p = Start-Process -FilePath $exe -ArgumentList $arguments -PassThru } else { $p = Start-Process -FilePath $exe -PassThru }
$main = Get-MainWindow $p.Id $mainClass 40000
Start-Sleep -Seconds 4
$others = @(Get-OtherWindows $p.Id $main | Where-Object { $_.Class -ne 'MsoSplash' })
if ($main -eq [IntPtr]::Zero -or $others.Count -gt 0) {
    Log "PROMPT OR NO MAIN WINDOW: main=$main others=$(@($others | ForEach-Object { "$($_.Class) '$($_.Title)'" }) -join '; ')"
    $i = 0
    foreach ($o in $others) { $i++; try { [RibbonObserveWin32]::CaptureTop($o.Handle, (Join-Path $Out "prompt_$i.png"), 4000) } catch { Log "capture failed: $_" } }
    # Close what this script started without answering the prompt.
    Stop-Process -Id $p.Id -Force
    Log 'stopped the process without answering the prompt'
    exit 2
}
Wait-Settled $p
$placement = [RibbonObserveWin32]::Placement($main)
$dpi = [RibbonObserveWin32]::Dpi($main)
Log "main=$main dpi=$dpi window=$([RibbonObserveWin32]::WindowRect($main) -join ',') client=$([RibbonObserveWin32]::ClientRect($main) -join ',') showCmd=$($placement.showCmd)"

function Get-RibbonWindow {
    foreach ($hwnd in [RibbonObserveWin32]::FindDescendantWindows($main, 'NetUIHWND')) {
        $top = [RibbonObserveWin32]::AccTree($hwnd, 0)
        if ($top.Count -gt 0 -and $top[0].Name -eq 'Ribbon') { return $hwnd }
    }
    return [IntPtr]::Zero
}

# -- reading the ribbon ---------------------------------------------------------

$script:ribbon = Get-RibbonWindow
if ($script:ribbon -eq [IntPtr]::Zero) { Log 'no ribbon window found' }

function Read-Ribbon {
    $rows = [RibbonObserveWin32]::AccTree($script:ribbon, 14)
    $lower = -1
    for ($i = 0; $i -lt $rows.Count; $i++) { if ($rows[$i].Name -eq 'Lower Ribbon') { $lower = $i; break } }
    $state = [ordered]@{ hidden = $true; scrolling = $false; tab = ''; groups = @() }
    if ($rows.Count -gt 0) { $state.ribbon = '{0},{1},{2},{3}' -f $rows[0].X, $rows[0].Y, $rows[0].W, $rows[0].H }
    # The ribbon's dock window (a child of the main window) is hidden when the
    # framework hides the ribbon; MSAA keeps reporting the last layout then.
    $dock = [RibbonObserveWin32]::DockOf($main, $script:ribbon)
    $state.dock = '{0},{1},{2},{3},visible={4}' -f $dock[0], $dock[1], $dock[2], $dock[3], $dock[4]
    if ($dock[4] -eq 0) { return $state }
    if ($lower -lt 0) { return $state }
    $lr = $rows[$lower]
    $visible = (($lr.State -band 0x8000) -eq 0) -and $lr.W -gt 0 -and $lr.H -gt 0
    $state.lower = '{0},{1},{2},{3}' -f $lr.X, $lr.Y, $lr.W, $lr.H
    if (-not $visible) { return $state }
    $state.hidden = $false
    $d0 = $lr.Depth
    $groups = New-Object System.Collections.Generic.List[object]
    $current = $null
    for ($i = $lower + 1; $i -lt $rows.Count -and $rows[$i].Depth -gt $d0; $i++) {
        $r = $rows[$i]
        $isVisible = (($r.State -band 0x8000) -eq 0) -and $r.W -gt 0
        if (($r.Name -eq 'Column left' -or $r.Name -eq 'Column right') -and $isVisible) { $state.scrolling = $true }
        if ($r.Role -eq 38 -and $r.Depth -eq $d0 + 2) { $state.tab = $r.Name; continue }
        if ($r.Role -eq 22 -and $r.Depth -eq $d0 + 3) {
            $current = [ordered]@{ name = $r.Name; x = $r.X; w = $r.W; collapsed = $false; items = New-Object System.Collections.Generic.List[string] }
            $groups.Add($current); continue
        }
        if ($current -ne $null -and $r.Depth -eq $d0 + 4 -and $isVisible) {
            if ($r.Name -eq $current.name -and ($r.Role -eq 58 -or $r.Role -eq 56 -or $r.Role -eq 57)) { $current.collapsed = $true }
            else { $current.items.Add(('{0}:{1}x{2}' -f $r.Name, $r.W, $r.H)) }
        }
    }
    $state.groups = $groups
    return $state
}

function Get-Signature($state) {
    if ($state.hidden) { return 'HIDDEN' }
    $parts = foreach ($g in $state.groups) {
        if ($g.collapsed) { '{0}[collapsed]' -f $g.name } else { '{0}{{{1}}}' -f $g.name, ($g.items -join ';') }
    }
    $sig = $parts -join ' | '
    if ($state.scrolling) { $sig += ' | SCROLL' }
    return $sig
}

function Set-Client([int]$w, [int]$h) {
    $c = [RibbonObserveWin32]::SetClientSize($main, $w, $h)
    Wait-Settled $p
    Start-Sleep -Milliseconds 150
    return [RibbonObserveWin32]::ClientRect($main)
}

$script:probes = New-Object System.Collections.Generic.List[object]
$script:states = [ordered]@{}
$script:stateIndex = @{}

function Probe([int]$w, [int]$h, [string]$ladder, [string]$direction, [int]$approachFrom = 0) {
    if ($approachFrom -gt 0) {
        if ($ladder -eq 'height') { [void](Set-Client $w $approachFrom) } else { [void](Set-Client $approachFrom $h) }
    }
    $c = Set-Client $w $h
    $state = Read-Ribbon
    $sig = Get-Signature $state
    if (-not $script:stateIndex.ContainsKey($sig)) {
        $n = $script:stateIndex.Count + 1
        $script:stateIndex[$sig] = $n
        $script:states["S$n"] = [ordered]@{ signature = $sig; first_seen = "$ladder $direction client ${w}x$h"; detail = $state }
        $wr = [RibbonObserveWin32]::WindowRect($main)
        $crop = 400
        if (-not $state.hidden -and $state.lower) { $lb = $state.lower.Split(','); $crop = [int]$lb[1] + [int]$lb[3] - $wr[1] + 12 }
        elseif ($state.ribbon) { $rb = $state.ribbon.Split(','); $crop = [int]$rb[1] + [int]$rb[3] - $wr[1] + 12 }
        [RibbonObserveWin32]::CaptureTop($main, (Join-Path $Out ("{0}_S{1:00}.png" -f $script:tabName, $n)), [Math]::Max(80, $crop))
    }
    $id = $script:stateIndex[$sig]
    $row = [pscustomobject]@{ tab = $script:tabName; ladder = $ladder; direction = $direction; req_w = $w; req_h = $h; client_w = $c[2]; client_h = $c[3]; state = "S$id"; hidden = $state.hidden; scrolling = $state.scrolling }
    $script:probes.Add($row)
    return $row
}

$script:transitions = New-Object System.Collections.Generic.List[object]

# Finds every state boundary between a probe `a` and a probe `b` further
# along the ladder (smaller in shrinking, larger in growing), bisecting with
# each probe approached from `approach` (the side the sweep comes from).
function Resolve-Boundaries($a, $b, [string]$ladder, [string]$direction, [int]$fixed, [int]$approach) {
    $cur = $a
    while ($cur.state -ne $b.state) {
        if ($ladder -eq 'height') { $x0 = $cur.req_h; $x1 = $b.req_h } else { $x0 = $cur.req_w; $x1 = $b.req_w }
        $near = $x0; $far = $x1
        while ([Math]::Abs($far - $near) -gt 1) {
            $mid = [int][Math]::Floor(($near + $far) / 2)
            if ($ladder -eq 'height') { $m = Probe $fixed $mid $ladder $direction $approach } else { $m = Probe $mid $fixed $ladder $direction $approach }
            if ($m.state -eq $cur.state) { $near = $mid } else { $far = $mid }
        }
        if ($ladder -eq 'height') { $next = Probe $fixed $far $ladder $direction $approach } else { $next = Probe $far $fixed $ladder $direction $approach }
        $script:transitions.Add([pscustomobject]@{ tab = $script:tabName; ladder = $ladder; direction = $direction; fixed = $fixed; last_px_of_from = $near; first_px_of_to = $far; from = $cur.state; to = $next.state })
        Log ("{0} {1} {2}: {3} at {4} -> {5} at {6}" -f $script:tabName, $ladder, $direction, $cur.state, $near, $next.state, $far)
        if ($next.state -eq $cur.state) { break }
        $cur = $next
    }
}

function Sweep([string]$ladder, [string]$direction, [int]$from, [int]$to, [int]$stepSize, [int]$fixed) {
    $sign = if ($to -lt $from) { -1 } else { 1 }
    if ($ladder -eq 'height') { $prev = Probe $fixed $from $ladder $direction } else { $prev = Probe $from $fixed $ladder $direction }
    for ($x = $from + $sign * $stepSize; ($sign -lt 0 -and $x -ge $to) -or ($sign -gt 0 -and $x -le $to); $x += $sign * $stepSize) {
        if ($ladder -eq 'height') { $cur = Probe $fixed $x $ladder $direction } else { $cur = Probe $x $fixed $ladder $direction }
        if ($cur.state -ne $prev.state) {
            $approach = if ($ladder -eq 'height') { $prev.req_h } else { $prev.req_w }
            Resolve-Boundaries $prev $cur $ladder $direction $fixed $approach
            if ($ladder -eq 'height') { $cur = Probe $fixed $x $ladder $direction } else { $cur = Probe $x $fixed $ladder $direction }
        }
        $prev = $cur
        $actual = if ($ladder -eq 'height') { $cur.client_h } else { $cur.client_w }
        if ([Math]::Abs($actual - $x) -gt 2 -and $sign -lt 0) { Log "window stops at $actual (requested $x); sweep ends"; break }
    }
}

# -- the run --------------------------------------------------------------------

try {
    foreach ($tab in $Tabs) {
        $script:tabName = $tab
        if (-not [RibbonObserveWin32]::DoDefaultAction($script:ribbon, $tab, 37)) { Log "tab $tab not found"; continue }
        Wait-Settled $p
        Log "tab ${tab}: width ladder at client height $LadderHeight"
        if (-not $SkipWidth) {
            Sweep 'width' 'shrinking' $MaxWidth $MinWidth $Step $LadderHeight
            Sweep 'width' 'growing' $MinWidth $MaxWidth $Step $LadderHeight
        }
        if ($tab -eq 'Home') {
            foreach ($w in $HeightLadderWidths) {
                Log "tab ${tab}: height ladder at client width $w"
                Sweep 'height' 'shrinking' $MaxHeight $MinHeight $HeightStep $w
                Sweep 'height' 'growing' $MinHeight $MaxHeight $HeightStep $w
            }
        }
    }
} finally {
    # Close the program first; records and logging come after, so that a
    # failure while saving them cannot leave the program open.
    try { if ($script:ribbon -ne [IntPtr]::Zero) { [void][RibbonObserveWin32]::DoDefaultAction($script:ribbon, 'Home', 37) } } catch { Write-Host "home: $_" }
    try {
        [RibbonObserveWin32]::Restore($main, $placement)
        if ($RestoreRect -ne '') {
            $r = $RestoreRect.Split(',') | ForEach-Object { [int]$_ }
            [RibbonObserveWin32]::SetWindowRect($main, $r[0], $r[1], $r[2], $r[3])
        }
        Start-Sleep -Milliseconds 500
        $restored = [RibbonObserveWin32]::WindowRect($main) -join ','
    } catch { Write-Host "restore: $_" }
    [RibbonObserveWin32]::Close($main)
    $closed = $p.WaitForExit(15000)
    $script:probes | Export-Csv -NoTypeInformation (Join-Path $Out 'probes.csv')
    $script:transitions | Export-Csv -NoTypeInformation (Join-Path $Out 'transitions.csv')
    $script:states | ConvertTo-Json -Depth 8 | Set-Content (Join-Path $Out 'states.json')
    Log "restored window=$restored"
    if ($closed) { Log 'program closed' } else {
        Log ("program still running; windows: " + (@(Get-OtherWindows $p.Id ([IntPtr]::Zero) | ForEach-Object { "$($_.Class) '$($_.Title)'" }) -join '; '))
    }
}
