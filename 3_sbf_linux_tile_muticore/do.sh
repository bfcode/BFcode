#!/bin/bash
tile-monitor --mkdir /opt/test --cd /opt/test --upload sbf sbf --mount-same /home/bloom/ --resume --root --tile 6x6 -- sbf rand_20b_100M.tr 20 restilek10.txt 35 10 >> restilek10_2.txt
sleep 3
tile-monitor --mkdir /opt/test --cd /opt/test --upload sbf sbf --mount-same /home/bloom/ --resume --root --tile 6x6 -- sbf rand_20b_100M.tr 20 restilek8.txt 35 8 >> restilek8_2.txt
sleep 3
tile-monitor --mkdir /opt/test --cd /opt/test --upload sbf sbf --mount-same /home/bloom/ --resume --root --tile 6x6 -- sbf rand_20b_100M.tr 20 restilek6.txt 35 6 >> restilek6_2.txt
sleep 3
tile-monitor --mkdir /opt/test --cd /opt/test --upload sbf sbf --mount-same /home/bloom/ --resume --root --tile 6x6 -- sbf rand_20b_100M.tr 20 restilek4.txt 35 4 >> restilek4_2.txt