#!/usr/bin/env python3
"""Run static analyzers"""
import subprocess
import os


def get_root():
    """Get project root"""
    return os.path.dirname(os.path.realpath(__file__)) + "/.."


def main():
    """Run commands"""
    commands = {
        "pylint":
            "pylint --max-line-length=240 -s n scripts/format.py",
        "yapf":
            "yapf --style google -i scripts/format.py",
        "cppcheck":
            "cppcheck --quiet --enable=all --inconclusive --library=posix --suppress=missingIncludeSystem -I valiant/ valiant/*.hpp examples/*.cpp tests/*.cpp",
        "clang-format":
            "clang-format -i valiant/*.hpp examples/*.cpp tests/*.cpp",
        "cmake-format":
            "cmake-format -i CMakeLists.txt",
        "cmake-lint":
            "cmake-lint --suppress-decorations CMakeLists.txt",
        "rg":
            "rg -t cpp --vimgrep TODO"
    }
    current = 1
    for executable, command in commands.items():
        progress = int((float(current) / len(commands)) * 100)
        print("[" + (" " * (3 - len(str(progress)))) + str(progress) + "%" +
              "] Running " + str(executable))
        subprocess.call(command, shell=True, cwd=get_root())
        current += 1


if __name__ == "__main__":
    main()
