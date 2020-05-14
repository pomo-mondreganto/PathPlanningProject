#!/usr/bin/env python3

import secrets
import subprocess
import sys
from pathlib import Path
from tempfile import NamedTemporaryFile

import numpy as np
from PIL import Image
from lxml import etree

BASE_DIR = Path(__file__).absolute().resolve().parents[1]
EXEC_PATH = BASE_DIR / 'Bin' / 'Debug' / 'PathPlanning'
TESTS_DIR = BASE_DIR / 'TestData'


def parse(test, stderr):
    rows = list(map(lambda x: list(map(int, x.text.split(' '))),
                    test.find('map').find('grid').findall('row')))
    data = np.zeros((512, 512, 3), dtype=np.uint8)

    for i, row in enumerate(rows):
        for j, col in enumerate(row):
            if col:
                data[i, j] = [0, 0, 0]
            else:
                data[i, j] = [255, 255, 255]

    log = sorted(stderr.split('\n')[:-1])
    for line in log:
        i, j = list(map(int, line.split(' ')[-2:]))
        big = False
        if 'LOOK' in line:
            color = [0, 255, 0]
        elif 'OPEN' in line:
            color = [255, 0, 0]
        elif 'START' in line:
            big = True
            color = [0, 0, 255]
        elif 'TERM' in line:
            big = True
            color = [255, 0, 255]
        else:
            assert False

        if not big:
            data[i, j] = color
            continue

        for di in range(-5, 6):
            for dj in range(-5, 6):
                data[i + di, j + dj] = color

    image = Image.fromarray(data)
    image.save(f'/tmp/vis_{secrets.token_hex(10)}.png')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f'Usage: {sys.argv[0]} test_file')
        exit(1)

    test_path = (TESTS_DIR / sys.argv[1]).resolve()
    test_data = test_path.read_text()
    root = etree.fromstring(test_data)

    root.find('options').find('loglevel').text = '1.5'
    tmp_f = NamedTemporaryFile(suffix='.xml', delete=False)

    etree.ElementTree(root).write(tmp_f)

    tmp_f.close()

    p = subprocess.Popen(
        [EXEC_PATH, tmp_f.name],
        cwd=BASE_DIR,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    out, err = p.communicate()

    parse(root, err.decode())
