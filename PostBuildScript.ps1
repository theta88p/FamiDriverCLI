param([parameter(Mandatory = $true)] [string] $solutionDir, [string] $targetDir, [string] $targetPath)

$version = (Get-Item $targetPath).VersionInfo.FileVersion
$version = $version.Substring(0, $version.Length - 2)
# バージョンの後ろの2文字を削る
$projectName = 'famidriver_cli'
$zipFileName = $targetDir + $projectName + "_v" + $version + ".zip"
$txtFileName = $solutionDir + "*.txt"
$mdFileName = $solutionDir + "*.md"
Write-Output $zipFileName

# 既存のzipファイルが存在する場合は削除
if (Test-Path $zipFileName) {
    Remove-Item $zipFileName
}

# 現在年月日時分秒
[String] $now = (Get-Date -Format "yyyyMMddHHmmss")

# 一時ファイル名
[String] $temp = "temp_" + $now + "_" + $path

# 除外ファイル
[Array] $exclude = @( "*.pdb", "*.exp", "*.lib", "Settings.xml", "runtimes", "Presets.bin")

# 一時フォルダ作成
Copy-Item $targetDir -Recurse $temp -Exclude $exclude
Copy-Item $txtFileName $temp
Copy-Item $mdFileName $temp

# 対象となるファイル
$files = Get-ChildItem -Path $temp -Exclude $exclude

# ファイル(directory)を圧縮
Compress-Archive -Path $files -DestinationPath $zipFileName

# 一時フォルダを削除
Remove-Item $temp -Recurse -Force
$outputDir = $solutionDir + "bin\"
Move-Item $zipFileName $outputDir -Force
