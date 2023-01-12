# Octopus Engine

This engine aims at handling RTS on large scale, the goal is to have a fast lock step engine with only logic and no graphical implementation


## Note on commands to produce movie from steps

make && ./bin/game

Script usage to convert .csv into .png (require python and matplotlib)

python ../../scripts/plot_all.py path_to_csv_files nb_of_files path_to_script

rm out.mp4 && ffmpeg -framerate 30 -pattern_type glob -i 'step/*.png' -c:v libx264 -pix_fmt yuv420p out.mp4  && cp out.mp4 ~/Data/Data/test/out.mp4
