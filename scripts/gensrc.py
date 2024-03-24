from __future__ import annotations

from pathlib import Path


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

    headerlist = [p.relative_to(root).as_posix() for p in src_dir.rglob("*.h")]
    headerlist.sort()
    write_cmake_list(cmake_dir / "headerlist.cmake", "headers", headerlist)

    sourcelist = [p.relative_to(root).as_posix() for p in src_dir.rglob("*.cpp")]
    sourcelist.sort()
    write_cmake_list(cmake_dir / "sourcelist.cmake", "sources", sourcelist)


if __name__ == "__main__":
    main()
