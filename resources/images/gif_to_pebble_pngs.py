import sys
import subprocess
from pathlib import Path

def convert(*args: str) -> None:
    subprocess.check_call(["convert"] + list(args))

def main(gif: Path) -> None:
    convert(
        str(gif),
        "-coalesce",
        "-shave", "21x22",
        "-resize", "200x200",
        "-colorspace", "gray",
        "-level", "0,70%",
        "-posterize", "4",
        "frame%03d.png",
    )

if __name__ == "__main__":
    main(Path(sys.argv[1]).resolve())
