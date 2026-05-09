import sys, os
img = sys.argv[1] if len(sys.argv) > 1 else r'C:\Users\Gower\Downloads\BojanOS\BojanOS.img'
target = int(sys.argv[2]) if len(sys.argv) > 2 else 131072
s = os.path.getsize(img)
pad = target - s
if pad > 0:
    with open(img, 'ab') as f:
        f.write(b'\x00' * pad)
