#!/usr/bin/env python3
from collections import defaultdict
import os
import time
from pathlib import Path
import sys
import subprocess

THIS_DIR = Path(__file__).absolute().parent


def pebble(*args: str):
    subprocess.check_call(["pebble"] + list(args))

def reset() -> None:
    pebble("kill")
    pebble("wipe")

def sleep_until(t: float) -> None:
    now = time.monotonic()
    d = t - now
    if d > 0.0:
        time.sleep(d)

def run(platforms: list[str]) -> int:
    # run with DEBUG_TIME set to true
    reset()
    screenshot_fnames = defaultdict(list)
    pebble("build")
    for platform in platforms:
        pebble("install", "--emulator", platform)
        for frame in range(59):
            begin = time.monotonic()
            screenshot_fname = f"screenshot_{platform}_{frame}.png"
            pebble("screenshot", "--emulator", platform, screenshot_fname)
            screenshot_fnames[platform].append(screenshot_fname)
            sleep_until(begin + 1.0)
    reset()

    # Combine all the pngs into one gif with ImageMagick
    # https://joeldare.com/combining-multiple-images-into-an-animated-gif-with-imagemagick
    for platform, fnames in screenshot_fnames.items():
        subprocess.check_call(
            [
                "convert",
                "-delay",
                "10",
                "-loop",
                "0",
                "-dispose",
                "previous",
                *fnames,
                f"screenshots_{platform}.gif",
            ]
        )

    return 0


def main() -> int:
    return run(["emery", "gabbro"])


if __name__ == "__main__":
    sys.exit(main())
