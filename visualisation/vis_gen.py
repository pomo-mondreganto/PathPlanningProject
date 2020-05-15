#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path
from tempfile import NamedTemporaryFile

import matplotlib
import numpy as np
from PIL import Image
from lxml import etree
from matplotlib import animation
from matplotlib import pyplot as plt

from custom_writer import FasterFFMpegWriter

matplotlib.use('agg')

BASE_DIR = Path(__file__).absolute().resolve().parents[1]
EXEC_PATH = BASE_DIR / 'Bin' / 'Debug' / 'PathPlanning'

PER_PIXEL = 1


def set_pixel(blocked, data, i, j, pix, sz=PER_PIXEL):
    if (i, j) in blocked:
        return
    coord = data[PER_PIXEL * i:PER_PIXEL * i + sz, PER_PIXEL * j:PER_PIXEL * j + sz]
    for di in range(sz):
        for dj in range(sz):
            coord[di, dj] = pix


def parse(test, stderr, dst_path, gen_image=False):
    rows = list(map(lambda x: list(map(int, x.text.split(' '))),
                    test.find('map').find('grid').findall('row')))
    data = np.zeros((PER_PIXEL * len(rows), PER_PIXEL * len(rows[0]), 3), dtype=np.uint8)

    fig = plt.figure(figsize=(25, 25))
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
            block = gen_image
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

        if not gen_image and step % draw_interval == 0 or step == len(log) - 1:
            images.append([plt.imshow(data, animated=True)])

        if step % 1000 == 0 or step == len(log) - 1:
            print(f'Done {step + 1} steps of {len(log)}')

    if gen_image:
        img = Image.fromarray(data)
        img = img.resize((512, 512))
        img.save(dst_path)
    else:
        print('Creating animation (can take a while)...')
        writer = FasterFFMpegWriter()
        ani = animation.ArtistAnimation(fig, images, interval=interval, blit=True, repeat=False)
        ani.save(dst_path, writer=writer)


def generate(root, algorithm, metric, hweight, bt, allow_diagonal, cut_corners, allow_squeeze,
             gen_image=False):
    with NamedTemporaryFile(suffix='.xml', delete=False) as tmp_f:
        algo_el = root.find('algorithm')
        algo_el.find('searchtype').text = algorithm
        algo_el.find('hweight').text = str(hweight)
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

    if gen_image:
        res_dir = 'images_temp'
        suffix = '.png'
    else:
        res_dir = 'animations_temp'
        suffix = '.mp4'

    dst_path = (Path(__file__).resolve().parent / res_dir / fname)
    dst_path = dst_path.with_suffix(suffix)

    parse(root, err.decode(), dst_path, gen_image)
    print('Done', dst_path)


def run_all(args):
    test_path = (Path().cwd() / args.file).resolve()
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

                            generate(root, algorithm, metric, 1.0, bt, allow_diagonal, cut_corners,
                                     allow_squeeze)


def run_custom(args):
    test_path = (Path().cwd() / args.file).resolve()
    test_data = test_path.read_text()
    root = etree.fromstring(test_data)
    root.find('options').find('loglevel').text = '1.5'

    generate(root, algorithm=args.algorithm, metric=args.metric, hweight=args.hweight,
             bt=args.breakingties, allow_diagonal=args.allowdiagonal, cut_corners=args.cutcorners,
             allow_squeeze=args.allowsqueeze, gen_image=args.image)


def run_image(args):
    test_path = (Path().cwd() / args.file).resolve()
    test_data = test_path.read_text()
    root = etree.fromstring(test_data)
    root.find('options').find('loglevel').text = '1.5'

    generate(root, algorithm=args.algorithm, metric=args.metric, hweight=args.hweight,
             bt=args.breakingties, allow_diagonal=args.allowdiagonal, cut_corners=args.cutcorners,
             allow_squeeze=args.allowsqueeze)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create visualisation for a test')
    parser.add_argument('--file', type=str, metavar='PATH', help='Path to test file', required=True)

    subparsers = parser.add_subparsers()

    all_parser = subparsers.add_parser('all', help='Bruteforce all options')
    all_parser.set_defaults(func=run_all)

    custom_parser = subparsers.add_parser('custom', help='Pass custom options')
    custom_parser.set_defaults(func=run_custom)

    custom_parser.add_argument('--algorithm', type=str, help='Algorithm to run',
                               default='jp_search')
    custom_parser.add_argument('--metric', type=str, help='Heuristic metric to use',
                               default='manhattan')
    custom_parser.add_argument('--hweight', type=float, help='Heuristic weight to use',
                               default=1.0)
    custom_parser.add_argument('--breakingties', type=str, help='Tie-breaking strategy',
                               default='g-min')
    custom_parser.add_argument('--allowdiagonal', type=int, help='Allow diagonal moves (0/1)',
                               default='1')
    custom_parser.add_argument('--cutcorners', type=int, help='Allow cutting corners (0/1)',
                               default='1')
    custom_parser.add_argument('--allowsqueeze', type=int, help='Allow squeezing (0/1)',
                               default='1')
    custom_parser.add_argument('--image', action='store_true', help='Generate the final image only')

    parsed = parser.parse_args()
    parsed.func(parsed)
