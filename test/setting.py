import os


global dir
global fw_bin
dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'src', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')
