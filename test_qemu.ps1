$out = 'C:\Users\Gower\AppData\Local\Temp\qemu_debug_out.txt'
$err = 'C:\Users\Gower\AppData\Local\Temp\qemu_debug_err.txt'
Remove-Item $out -ErrorAction SilentlyContinue
Remove-Item $err -ErrorAction SilentlyContinue
$proc = Start-Process -FilePath 'qemu-system-i386.exe' -ArgumentList @('-fda','C:\Users\Gower\Downloads\BojanOS\BojanOS.img','-serial','file:C:\Users\Gower\AppData\Local\Temp\qemu_serial.txt','-no-reboot','-d','int,cpu_reset') -PassThru -RedirectStandardOutput $out -RedirectStandardError $err -WindowStyle Hidden
Start-Sleep -Seconds 3
if (!$proc.HasExited) { $proc.Kill() }
Start-Sleep -Seconds 1
Write-Output '--- out ---'
if (Test-Path $out) { Get-Content $out -ErrorAction SilentlyContinue | Select-Object -First 30 } else { Write-Output '(missing)' }
Write-Output '--- err ---'
if (Test-Path $err) { Get-Content $err -ErrorAction SilentlyContinue | Select-Object -First 30 } else { Write-Output '(missing)' }
