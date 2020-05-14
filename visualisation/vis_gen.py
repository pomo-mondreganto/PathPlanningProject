#!/usr/bin/env python3

import subprocess
import sys
from pathlib import Path
from tempfile import NamedTemporaryFile

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
from lxml import etree

BASE_DIR = Path(__file__).absolute().resolve().parents[1]
EXEC_PATH = BASE_DIR / 'Bin' / 'Debug' / 'PathPlanning'

PER_PIXEL = 9


def set_pixel(blocked, data, i, j, pix, sz=PER_PIXEL):
    if (i, j) in blocked:
        return
    coord = data[PER_PIXEL * i:PER_PIXEL * i + sz, PER_PIXEL * j:PER_PIXEL * j + sz]
    for di in range(sz):
        for dj in range(sz):
            coord[di, dj] = pix


def parse(test, stderr, dst_path):
    rows = list(map(lambda x: list(map(int, x.text.split(' '))),
                    test.find('map').find('grid').findall('row')))
    data = np.zeros((PER_PIXEL * len(rows), PER_PIXEL * len(rows[0]), 3), dtype=np.uint8)

    fig = plt.figure()
    images = []

    blocked = set()
    for i, row in enumerate(rows):
        for j, col in enumerate(row):
            if col:
                set_pixel(blocked, data, i, j, [0, 0, 0])
                blocked.add((i, j))
            else:
                set_pixel(blocked, data, i, j, [255, 255, 255])

    log = stderr.split('\n')[:-1]

    approx_length = 20 * 1000
    interval = 20
    total_length = len(log) * interval
    draw_interval = max(1, total_length // approx_length)

    for step, line in enumerate(log):
        i, j = list(map(int, line.split(' ')[-2:]))
        block = False
        if 'LOOK' in line:
            color = [0, 255, 0]
        elif 'OPEN' in line:
            color = [255, 0, 0]
        elif 'START' in line:
            block = True
            color = [0, 0, 255]
        elif 'TERM' in line:
            block = True
            color = [255, 0, 255]
        else:
            assert False

        set_pixel(blocked, data, i, j, color)
        if block:
            blocked.add((i, j))

        if step % draw_interval == 0 or step == len(log) - 1:
            images.append([plt.imshow(data, animated=True)])

    ani = animation.ArtistAnimation(fig, images, interval=interval, blit=True, repeat=False)
    ani.save(dst_path)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f'Usage: {sys.argv[0]} test_file')
        exit(1)

    test_path = (Path().cwd() / sys.argv[1]).resolve()
    test_data = test_path.read_text()
    root = etree.fromstring(test_data)
    root.find('options').find('loglevel').text = '1.5'

    for algorithm in ['astar', 'dijkstra', 'jp_search']:
        if algorithm == 'dijkstra':
            # dijkstra doesn't use metrics or breaking ties
            metrics = ['diagonal']
            breaking_ties = ['g-min']
        else:
            metrics = ['diagonal', 'manhattan', 'euclidean', 'chebyshev']
            breaking_ties = ['g-min', 'g-max']

        for metric in metrics:
            for bt in breaking_ties:
                for allow_diagonal in [0, 1]:
                    for cut_corners in [0, 1]:
                        if cut_corners and not allow_diagonal:
                            continue
                        for allow_squeeze in [0, 1]:
                            if allow_squeeze and (not cut_corners or not allow_diagonal):
                                continue

                            with NamedTemporaryFile(suffix='.xml', delete=False) as tmp_f:
                                algo_el = root.find('algorithm')
                                algo_el.find('searchtype').text = algorithm
                                algo_el.find('metrictype').text = metric
                                algo_el.find('breakingties').text = bt
                                algo_el.find('allowdiagonal').text = str(allow_diagonal)
                                algo_el.find('cutcorners').text = str(cut_corners)
                                algo_el.find('allowsqueeze').text = str(allow_squeeze)
                                etree.ElementTree(root).write(tmp_f)
                                tmp_f.close()

                                p = subprocess.Popen(
                                    [EXEC_PATH, tmp_f.name],
                                    cwd=BASE_DIR,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE,
                                )
                                out, err = p.communicate()

                            fname = f'ani_{algorithm}_{metric}_{bt}'
                            if allow_diagonal:
                                fname += '_allowdiagonal'
                            if cut_corners:
                                fname += '_cutcorners'
                            if allow_squeeze:
                                fname += '_allowsqueeze'

                            dst_path = (Path(__file__).resolve().parent / 'animations' / fname)
                            dst_path = dst_path.with_suffix('.mp4')

                            parse(root, err.decode(), dst_path)
                            print('Done', dst_path)
