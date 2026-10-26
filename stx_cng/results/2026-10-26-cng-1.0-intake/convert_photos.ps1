# Rebuilds the photo_full and photo_crop inputs of the 2026-10-22 intake corpus
# from the Windows wallpapers: System.Drawing decodes each JPEG and saves it as
# PNG; crops are the centre 1920 x 1200 (offsets rounded up) of that PNG,
# decoded again (a crop of the JPEG-backed bitmap gives other bytes). Hashes
# are checked by find_corpus.py.
param([string]$Inputs, [string]$Out)
Add-Type -AssemblyName System.Drawing
$web = 'C:\Windows\Web'
New-Item -ItemType Directory -Force "$Out\photo_full", "$Out\photo_crop" | Out-Null
foreach ($line in Get-Content $Inputs) {
    $fields = $line -split "`t"
    if ($fields.Count -lt 4) { continue }
    $category = $fields[0]
    $name = $fields[1]
    if ($category -ne 'photo_full' -and $category -ne 'photo_crop') { continue }
    $base = $name -replace '^crop_', '' -replace '\.png$', ''
    $folder, $stem = $base -split '_', 2
    if ($folder -eq 'Screen' -or $folder -eq 'touchkeyboard') {
        $source = Join-Path $web "$folder\$stem.jpg"
    } elseif ($folder -eq 'Windows' -and $stem -like '*_1920x1200') {
        $source = Join-Path $web "4K\Wallpaper\Windows\$stem.jpg"
    } else {
        $source = Join-Path $web "Wallpaper\$folder\$stem.jpg"
    }
    if (-not (Test-Path $source)) { Write-Output "no source for $name ($source)"; continue }
    if ($category -eq 'photo_crop') {
        $source = "$Out\photo_full\$base.png"
        if (-not (Test-Path $source)) { Write-Output "convert $base.png first"; continue }
    }
    $bitmap = [System.Drawing.Bitmap]::FromFile($source)
    try {
        if ($category -eq 'photo_full') {
            $bitmap.Save("$Out\photo_full\$name", [System.Drawing.Imaging.ImageFormat]::Png)
        } else {
            $x = [int][Math]::Ceiling(($bitmap.Width - 1920) / 2)
            $y = [int][Math]::Ceiling(($bitmap.Height - 1200) / 2)
            $rect = New-Object System.Drawing.Rectangle $x, $y, 1920, 1200
            $crop = $bitmap.Clone($rect, $bitmap.PixelFormat)
            $crop.Save("$Out\photo_crop\$name", [System.Drawing.Imaging.ImageFormat]::Png)
            $crop.Dispose()
        }
    } finally {
        $bitmap.Dispose()
    }
}
