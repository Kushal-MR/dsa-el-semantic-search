"""Setup check for the Python side (step 1).

Confirms the Python version and that the three packages we use are installed.
It does not download the AI model; that happens once, later, in step 5.
"""
import importlib
import sys

REQUIRED = [
    ("pymupdf", "pymupdf", "reads text from PDFs"),
    ("sentence_transformers", "sentence-transformers", "runs the AI model (text -> 384 numbers)"),
    ("flask", "flask", "local web server"),
]


def main() -> int:
    ok = True
    if sys.version_info < (3, 10):
        print(f"Python 3.10+ needed, found {sys.version.split()[0]}")
        ok = False
    else:
        print(f"Python {sys.version.split()[0]}: OK")

    for module, package, purpose in REQUIRED:
        try:
            importlib.import_module(module)
            print(f"{package}: OK ({purpose})")
        except ImportError:
            print(f"{package}: MISSING ({purpose}) -> pip install {package}")
            ok = False

    print("Python setup OK" if ok else "Python setup incomplete")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
