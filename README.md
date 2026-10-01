# Semantic Search for Study Notes

Search your own study notes by meaning, not exact words. The search index (HNSW: a layered
graph with heaps) is written from scratch in C. Python only reads PDFs and runs a small AI
model that turns text into numbers. A local web page shows the results.

DSA Experiential Learning, CS233AI, RVCE.

## Layout

| Folder | What lives there |
| --- | --- |
| `engine/` | C engine: every data structure (graded part) |
| `pipeline/` | Python: PDF reading, chunking, embeddings |
| `server/` | Python: runs the engine, serves the page |
| `web/` | Search page and visuals |
| `bench/` | Evaluation scripts and results |
| `data/` | Notes and saved index (kept out of git) |
| `docs/` | Engine protocol, report, slides |

## Setup checks

```
cd engine
make check                      # on Windows/MinGW: mingw32-make check

cd ..
pip install -r requirements.txt
python pipeline/check_setup.py  # checks Python version and packages
```

