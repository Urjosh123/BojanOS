param([string] = 'C:\Users\Gower\Downloads\BojanOS\BojanOS.img', [int] = 131072)
 = (Get-Item ).Length
 =  - 
if ( -gt 0) {
     = [System.IO.File]::Open(, [System.IO.FileMode]::Append)
     = New-Object byte[] 
    .Write(, 0, )
    .Close()
}
