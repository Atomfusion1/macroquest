$location = Get-Location

function Copy-Latestfile {
	Param($File1, $File2, $Relative)
	#Write-Host "Copy $File1 to $File2 ..."

	if (![System.IO.File]::Exists($File1)) {
		return
	}
	$File1Date = Get-Item $File1 | ForEach-Object{ $_.LastWriteTimeUtc }

	# make sure dest directory exists
	$dir = Split-Path -Parent $File2
	$dirExists = Test-Path -Path $dir -PathType Container
	if (-Not $dirExists)
	{
		Write-Host "DirName: $dir"
		New-Item -ItemType Directory -Path $dir | Out-Null
	}

	if (![System.IO.File]::Exists($File2)) {
		try {
			New-Item -ItemType HardLink -Path $File2 -Value $File1 | Out-Null

			$File2 = Resolve-Path $File2
			Write-Host "Linking $File2..."
		}
		catch {
			Copy-Item -Path $File1 -Dest $File2 -Force

			$File2 = Resolve-Path $File2
			Write-Host "Copying $File2..."
		}
		return
	}

	$File2 = Resolve-Path $File2
	$File2Date = Get-Item $File2 | ForEach-Object{ $_.LastWriteTimeUtc }

	if ($File1Date -gt $File2Date)
	{
		Remove-Item -Path $File2
		try {
			New-Item -ItemType HardLink -Path $File2 -Value $File1

			$File2 = Resolve-Path $File2
			Write-Host "Linking $File2..."
		}
		catch {
			Copy-Item -Path $File1 -Dest $File2 -Force

			$File2 = Resolve-Path $File2
			Write-Host "Copying $File2..."
		}
	}
}

try {
	$Src = $args[0]
	$Dst = $args[1]
	$Arch = $args[2]
	#Write-Output $Src $Dst $Arch

	$ListFile = "$Src\BinCopy.txt"
	Set-Location $Src

	$lines = Get-Content $ListFile 
	foreach ($line in $lines)
	{
		$files = Get-ChildItem -Path "$Src\$line" | % { $_.FullName }
		foreach ($file in $files)
		{
			$relative = Resolve-Path -Relative $file
			Copy-LatestFile $file "$Dst\$relative" $relative
		}
	}

	# Copy crash reporter
	Copy-LatestFile "$Src\..\contrib\vcpkg\installed\$Arch-windows-static\tools\crashpad_handler.exe" "$Dst\crashpad_handler.exe" "crashpad_handler.exe"
	$DstLeaf = $Dst|Split-Path -leaf
	# Copy DX files
	if ($Dst -like '*debug*')
	{
		Copy-LatestFile "$Src\..\contrib\vcpkg\installed\$Arch-windows\debug\bin\D3DX9d_43.dll" "$Dst\D3DX9d_43.dll" "D3DX9d_43.dll"
	}
	else
	{
		Copy-LatestFile "$Src\..\contrib\vcpkg\installed\$Arch-windows\bin\D3DX9d_43.dll" "$Dst\D3DX9d_43.dll" "D3DX9d_43.dll"
	}
}
catch {
	Write-Output $_
	Set-Location $location

	exit 1
}

Set-Location $location