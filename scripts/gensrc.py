from collections.abc import Iterable, Iterator
from pathlib import Path


def glob(dirpaths: list[Path], patterns: list[str]) -> Iterator[Path]:
    for dirpath in dirpaths:
        for pattern in patterns:
            yield from dirpath.rglob(pattern)


def norm(root: Path, paths: Iterable[Path]) -> list[str]:
    return sorted(p.relative_to(root).as_posix() for p in paths)


def write_cmake_list(path: Path, name: str, strings: list[str]) -> None:
    with open(path, "w", encoding="utf-8") as f:
        f.write(f"set({name}\n")
        for string in strings:
            f.write(f'    "{string}"\n')
        f.write(")\n")


def main() -> None:
    root = Path(__file__).parents[1]
    cmake_dir = root / "cmake"
    src_dir = root / "src"
    vendor_dir = root / "vendor"

    headerlist = norm(root, glob([src_dir, vendor_dir], ["*.h", "*.hpp"]))
    write_cmake_list(cmake_dir / "headerlist.cmake", "PROJECT_HEADERS", headerlist)

    sourcelist = norm(root, glob([src_dir, vendor_dir], ["*.c", "*.cpp"]))
    write_cmake_list(cmake_dir / "sourcelist.cmake", "PROJECT_SOURCES", sourcelist)


if __name__ == "__main__":
    main()
