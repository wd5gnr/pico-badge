If using an Image from GIMP make sure there is no transparency (remove Alpha channel).

Color 565 From Gimp:
===
1. Load picture as normal
2. Resize to 240x240 (for this case. MUST BE 240x240)
3. File | Export As...
4. Unselect Select File Type (By Extension)
5. Pick Windows BMP image and press Export
6. From dialog, click Advanced Options
7. Pick 16-bits, R5 G6 B5
8. Complete export



C code From gimp
===
1. Load file in correct format
2. Export to C or H file
3. Uncheck Use Glib types
4. Check Use macros instead of struct
5. Check Save as RGB565 (unless using monochrome)
6. Others unchecked
7. Set prefix name as you like
8. Press Export
9. Modify file so it looks like head2gimp.h (remove most of it. Change to const char * PROGMEM string. Remove parenthesis.)
    

Note: sometimes a Trigraph will appear and generate a warning. You can go break the two question marks with a quote. E.g"

".... ??/.... "  => ".... ?" "?/....." 

This is not necessary, but will avoid the warnings. (see ncc1701.h for an example)


Font list:
https://github.com/olikraus/u8g2/wiki/fntlistall

