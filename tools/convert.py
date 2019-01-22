#!/usr/bin/python

import sys, os
from PIL import Image


def con(inf, of):
   if not ".bmp" in inf:
      print("skipping", inf)
      return
   print(inf, "->", of)
   im = Image.open(inf)
   if im.mode == "RGB":
       pixelSize = 3
   elif im.mode == "RGBA":
       pixelSize = 4
   else:
     print ('not supported pixel mode: "%s"' % (im.mode))
     return

   pixels = im.tostring()
   pixels2 = ""
   for i in range(0, len(pixels) - 1, pixelSize):
       pixels2 += chr(ord(pixels[i + 2]) >> 3 | (ord(pixels[i + 1]) << 3 & 0xe0))
       pixels2 += chr(ord(pixels[i]) & 0xf8 | (ord(pixels[i + 1]) >> 5 & 0x07))
   out = open(of, "wb")
   out.write(pixels2)
   out.close()

if len(sys.argv) == 2:
    # print "\nReading: " + sys.argv[1]
    for file in os.listdir(sys.argv[1]):
       of = os.path.join("flash", file.replace(".bmp", ".raw"))
       con(os.path.join(sys.argv[1], file), of)
else:
    print "Usage: png2fb.py indir"
    sys.exit(1)

