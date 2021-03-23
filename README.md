# rdr2_photos_converter
Converts unreadable camera photos saved locally to regular jpeg. Normally those images are only viewable inside the game.

Usage:
```
./converter input_photo output_file
```
\
To convert multiple pictures you can put them inside a folder (e.g.  'original') and use the following command:\
```
mkdir converted; i=0; for image in original/*; do ./converter $image ./converted/$i.jpg; ((i++)); done
```
