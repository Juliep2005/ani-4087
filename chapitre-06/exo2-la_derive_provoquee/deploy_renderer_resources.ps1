param(
    [Parameter(Mandatory = $true)]
    [string]$TargetDirectory
)

$source = [System.IO.Path]::GetFullPath(
    (Join-Path $PSScriptRoot '..\..\..\projet\Kit\Resources\NKRenderer')
)
if (-not (Test-Path -LiteralPath $source -PathType Container)) {
    throw "Ressources NKRenderer du kit introuvables : $source"
}

$destination = Join-Path $TargetDirectory 'Resources\NKRenderer'
New-Item -ItemType Directory -Path $destination -Force | Out-Null
Get-ChildItem -LiteralPath $source -Force | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $destination -Recurse -Force
}
