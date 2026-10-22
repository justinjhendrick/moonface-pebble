import sys
import subprocess
from pathlib import Path

def convert(*args: str) -> None:
    subprocess.check_call(["convert"] + list(args))

def main(gif: Path) -> None:
    convert("-coalesce", str(gif), "frame%03d.png")
    for frame in Path(".").glob("frame*.png"):
        convert("-shave", "21x22", str(frame), str(frame))
        convert("-resize", "200x200", str(frame), str(frame))

if __name__ == "__main__":
    main(Path(sys.argv[1]).resolve())
