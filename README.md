# BMP Editor
A program written in C to represent the progress of reading, writing and editing a bitmap image file

Target file type specification:
- 24 bits per pixel (8/8/8)
- DIB header (bitmap information header) type: BITMAPINFOHEADER (40 bytes)
- Pixel array with padding

Function:
- Read/Save from/to an existing bitmap
- GUI made in Qt
- CLI made taking advantage of the funtion getopt (Linux)

Author: Nguyen Quang Huy

Date: 02/2018
