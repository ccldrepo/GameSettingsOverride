from __future__ import annotations

from pathlib import Path
from typing import Iterator


def glob(dirpaths: list[Path], patterns: list[str]) -> Iterator[Path]:
    for dirpath in dirpaths:
        for pattern in patterns:
            yield from dirpath.rglob(pattern)


def write_cmake_list(path: Path, name: str, strings: list[str]) -> None:
    with open(path, "w", encoding="utf-8") as f:
        f.write(f"set({name}\n")
        for string in strings:
            f.write(f"    {string}\n")
        f.write(")\n")


def main() -> None:
    root = Path(__file__).parents[1]
    cmake_dir = root / "cmake"
    src_dir = root / "src"
    vendor_dir = root / "vendor"

    headerlist = sorted(
        p.relative_to(root).as_posix()
        for p in glob([src_dir, vendor_dir], ["*.h", "*.hpp"])
    )
    write_cmake_list(cmake_dir / "headerlist.cmake", "headers", headerlist)

    sourcelist = sorted(
        p.relative_to(root).as_posix()
        for p in glob([src_dir, vendor_dir], ["*.c", "*.cpp"])
    )
    write_cmake_list(cmake_dir / "sourcelist.cmake", "sources", sourcelist)


if __name__ == "__main__":
    main()
