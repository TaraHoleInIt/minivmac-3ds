BaseOpts="-t 3dsa -api 3ds -lang eng -as 0 -bg 1 -ta 0 -speed 1 -n minivmac-3ds -an mnvm3ds"

setup/tool $BaseOpts $@ > setup.sh
sh setup.sh
rm Makefile

