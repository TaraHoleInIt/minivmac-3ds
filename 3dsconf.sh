BaseOpts="-t 3dsa -api 3ds -lang eng -as 0 -bg 1 -ta 0 -speed 1 -n minivmac-3ds -an mnvm3ds"

Plus="${BaseOpts} -m Plus"
Plus_Sound="${BaseOpts} -m Plus -sound 1"

II_512x384x1="${BaseOpts} -m II -depth 0 -hres 512 -vres 384"
II_512x384x2="${BaseOpts} -m II -depth 1 -hres 512 -vres 384"
II_512x384x4="${BaseOpts} -m II -depth 2 -hres 512 -vres 384"
II_512x384=8="${BaseOpts} -m II -depth 3 -hres 512 -vres 384"

II_512x384x1_Sound="${BaseOpts} -m II -depth 0 -hres 512 -vres 384 -sound 1"
II_512x384x2_Sound="${BaseOpts} -m II -depth 1 -hres 512 -vres 384 -sound 1"
II_512x384x4_Sound="${BaseOpts} -m II -depth 2 -hres 512 -vres 384 -sound 1"
II_512x384=8-Sound="${BaseOpts} -m II -depth 3 -hres 512 -vres 384 -sound 1"

II_640x480x1="${BaseOpts} -m II -depth 0 -hres 640 -vres 480"
II_640x480x2="${BaseOpts} -m II -depth 1 -hres 640 -vres 480"
II_640x480x4="${BaseOpts} -m II -depth 2 -hres 640 -vres 480"
II_640x480=8="${BaseOpts} -m II -depth 3 -hres 640 -vres 480"

II_640x480x1_Sound="${BaseOpts} -m II -depth 0 -hres 640 -vres 480"
II_640x480x2_Sound="${BaseOpts} -m II -depth 1 -hres 640 -vres 480"
II_640x480x4_Sound="${BaseOpts} -m II -depth 2 -hres 650 -vres 480"
II_640x480x8_Sound="${BaseOpts} -m II -depth 3 -hres 640 -vres 480"

setup/tool $Plus | sh
make -f Makefile.Plus clean
make -f Makefile.Plus

setup/tool $BaseOpts $@ > setup.sh
sh setup.sh
rm Makefile

