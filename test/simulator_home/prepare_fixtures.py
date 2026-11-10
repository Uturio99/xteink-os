#!/usr/bin/env python3
"""Create isolated X4 Pro simulator SD roots for Home empty/populated review."""

import json
import shutil
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BOOKS = (
    ("test_reader_rendering_matrix.epub", "A Reader's Test", "NoveOS Test Suite"),
    ("test_tables.epub", "Tables and Text", "NoveOS Test Suite"),
    ("test_mixed_images.epub", "Images on Paper", "NoveOS Test Suite"),
)


def path_hashes(paths):
    source = """
#include <functional>
#include <iostream>
#include <string>
int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) std::cout << std::hash<std::string>{}(argv[i]) << '\\n';
}
"""
    with tempfile.TemporaryDirectory(dir=ROOT / "test" / "simulator_home") as temp:
        cpp = Path(temp) / "path_hash.cpp"
        exe = Path(temp) / "path_hash"
        cpp.write_text(source, encoding="utf-8")
        subprocess.run(["g++", str(cpp), "-o", str(exe)], check=True)
        result = subprocess.run([str(exe), *paths], check=True, text=True, capture_output=True)
    hashes = result.stdout.splitlines()
    if len(hashes) != len(paths):
        raise RuntimeError("std::hash helper returned an unexpected number of results")
    return hashes


def main():
    import argparse

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("destination", type=Path, help="new, non-existing fixture root")
    args = parser.parse_args()
    destination = args.destination.expanduser().resolve()
    if destination.exists():
        raise SystemExit(f"Refusing to overwrite existing fixture path: {destination}")

    paths = [f"/books/{filename}" for filename, _, _ in BOOKS]
    sources = [ROOT / "test" / "epubs" / filename for filename, _, _ in BOOKS]
    for source in sources:
        if not source.is_file():
            raise FileNotFoundError(source)
    hashes = path_hashes(paths)

    empty = destination / "empty"
    populated = destination / "populated"
    (empty / ".crosspoint").mkdir(parents=True)
    (populated / ".crosspoint").mkdir(parents=True)
    (populated / "books").mkdir()
    (empty / ".crosspoint" / "recent.json").write_text('{"books": []}\n', encoding="utf-8")

    entries = []
    for (filename, title, author), source, relative in zip(BOOKS, sources, paths):
        shutil.copy2(source, populated / relative.lstrip("/"))
        entries.append({"path": relative, "title": title, "author": author, "coverBmpPath": ""})
    (populated / ".crosspoint" / "recent.json").write_text(
        json.dumps({"books": entries}, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )

    for index, path_hash in enumerate(hashes):
        progress_dir = populated / ".crosspoint" / f"epub_{path_hash}"
        progress_dir.mkdir()
        # First EPUB shows partial progress; the other two remain unread.
        data = bytes((0, 0, 1, 0, 2, 0)) if index == 0 else bytes((0, 0, 0, 0, 0, 0))
        (progress_dir / "progress.bin").write_bytes(data)
    print(f"Created isolated fixtures under {destination}")


if __name__ == "__main__":
    main()
