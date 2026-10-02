# Engine protocol (the contract)

Status: **agreed 2026-10-02** (step 1). The C engine, the Python pipeline and the server
must all follow this file. Any change here is made in the same commit as the code change.

## 1. Distance

- The engine uses **squared Euclidean distance**: `d(a, b) = sum over i of (a[i] - b[i])^2`.
- No square root: if `a < b` then `a^2 < b^2`, so the closest note is the same either way,
  and skipping `sqrt` saves time on every comparison.
- The `dist` values the engine reports are squared Euclidean distances (smaller = closer).
- Note vectors are **unit length** (length 1). The Python pipeline asks the model for
  normalised vectors. On unit-length vectors, Euclidean distance ranks results exactly like
  cosine similarity, so we get the best search quality from the AI model.
- 2D test points and benchmark data (SIFT) are used as they are, not normalised.

## 2. Talking to the engine

The engine is a separate program (`engine/build/engine`, `engine.exe` on Windows).
The server starts it and talks to it through standard input and output:

- **Commands**: one plain-text line per command, sent to the engine's stdin.
  Words and numbers are separated by single spaces. Commands are uppercase.
- **Replies**: exactly one line of JSON per command on stdout, flushed immediately.
- **Logs and debug messages** go to stderr only, never stdout.
- Numbers are plain decimals (Python sends them with `%.8g`).
- Maximum command line length: 65,536 bytes. Longer lines are rejected with an error.
- Commands are handled one at a time, in order.

Why this split: plain text is easy for C to read, and JSON is easy for C to write (`printf`)
and for Python to read (`json.loads`). C never has to parse JSON.

### Replies

Every reply has `"ok"`:

```
{"ok":true, ...command-specific fields...}
{"ok":false,"error":"expected 384 numbers, got 12"}
```

An error never stops the engine; it waits for the next command.

## 3. Commands

`<x...>` means exactly `dim` numbers (the vector). `<path>` is the rest of the line, so
paths may contain spaces.

| Command | Reply fields | Notes |
| --- | --- | --- |
| `NEW <dim>` | `dim` | Start an empty index. 1 <= dim <= 4096. Replaces any current index. Optional HNSW settings are added in step 3. |
| `LOAD_VECTORS <path>` | `added`, `count` | Bulk-add every vector in a vectors file (section 4). File dim must match. |
| `LOAD_NOTES <path>` | `notes` | Load note records (section 5). Number of notes must equal the number of vectors. |
| `ADD <x...>` | `id` | Add one vector; replies with its new ID. Used for 2D demos and tests. |
| `SEARCH <k> <ef> <x...>` | `results`, `compared`, `time_us` | HNSW search for the k closest. `ef` = how wide the search looks (k <= ef). |
| `BRUTE <k> <x...>` | `results`, `compared`, `time_us` | Exact search by checking every vector (the referee). |
| `TRACE <k> <ef> <x...>` | as SEARCH, plus `trace` | SEARCH that also reports the path it took, for the replay. |
| `NEIGHBORS <id>` | `id`, `neighbors` | The note's links on layer 0 (most similar notes). Optional `<layer>` after the ID. |
| `GET <id>` | `id`, `level`, `vector`, plus the note record if loaded | Used by the visuals to place points. |
| `STATS` | `count`, `dim`, `max_level`, `layer_sizes`, settings | Index summary. |
| `SAVE <path>` | `saved` | Write the whole index to an index file (section 6). |
| `LOAD <path>` | `count`, `dim` | Replace the current index with a saved one. |
| `QUIT` | `{"ok":true}` | Exit cleanly. End of input also exits. |

### Result format (SEARCH, BRUTE, TRACE)

```
{"ok":true,
 "results":[{"id":12,"dist":0.31,"subject":"DSA","file":"DSA/Unit2_Stacks.pdf","page":7,"snippet":"Browser history ..."}],
 "compared":312,
 "time_us":850}
```

- Results are sorted closest first. If no notes are loaded, each result has only `id` and `dist`.
- `compared` = how many distances were computed (the fair measure of work done).
- `time_us` = search time in microseconds, measured inside the engine.

### Trace format (TRACE)

```
"trace":[{"layer":2,"path":[5,9],"evaluated":[[5,0.91],[9,0.42],[3,0.77]]},
         {"layer":1,"path":[9,14], "evaluated":[...]},
         {"layer":0,"path":[14,21],"evaluated":[...]}]
```

- One entry per layer, top layer first. `path` = the notes the search moved through on that
  layer; `evaluated` = every note whose distance was computed there, with its distance.
- May be refined in step 7 (visuals); any change is made here first.

## 4. Vectors file (Python writes, C reads)

Binary, little-endian:

| Offset | Size | Field |
| --- | --- | --- |
| 0 | 4 bytes | Tag `DSAV` |
| 4 | uint32 | Version = 1 |
| 8 | uint32 | `count` (number of vectors) |
| 12 | uint32 | `dim` (numbers per vector) |
| 16 | count x dim x float32 | The vectors, one after another |

Vector `i` gets note ID `i`. Binary instead of text because it is exact, about 4x smaller,
and C reads it in a single `fread`.

## 5. Notes file (Python writes, C reads)

UTF-8 text, one line per note, fields separated by a tab:

```
id <TAB> subject <TAB> file <TAB> page <TAB> snippet
```

- `id` starts at 0 and must equal the line number (so it matches the vector order).
- `file` is relative to `data/notes/`; `page` starts at 1.
- `snippet`: at most 300 bytes, with tabs and newlines replaced by spaces.
- Maximum line length: 1,024 bytes.

## 6. Index file (C only)

Binary, starts with the tag `DSAI` and a version number. The exact layout is defined in
step 4, once step 3 has fixed how the graph is stored in memory.

## 7. IDs

Note IDs are 0, 1, 2, ... in the order notes are added, and the ID is the position in every
array (vectors, records, graph). Lookups are `array[id]`: no searching, no hash table.
Notes are never deleted; to remove notes, rebuild the index.
