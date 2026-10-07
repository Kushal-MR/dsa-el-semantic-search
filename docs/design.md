# Engine design notes

One section per build step. Written before the code, kept up to date after it.
Useful for the report and the viva.

## Step 2: vectors, distance, brute force, heaps

### Vector store (dynamic array)

- All vectors live in **one contiguous `float` array**, row after row:
  vector `i` starts at `data[i * dim]`.
- Why one array instead of one allocation per vector:
  - cache-friendly: the CPU reads memory in blocks, so neighbouring numbers arrive together;
  - the vectors file loads with a single `fread`;
  - no per-vector bookkeeping or pointers.
- When full, capacity **doubles** (`realloc`). Copying happens rarely, so appending a vector
  is O(1) amortised. This is the dynamic array from Unit I.
- The vector's ID is its row number (see `protocol.md`, section 7).

### Distance

- `l2sq(a, b, dim)` = squared Euclidean distance (no square root, see `protocol.md`).
- Every call increments a comparison counter. The counter is what SEARCH and BRUTE report
  as `compared`, the fair measure of work done.

### Brute-force search (the referee)

- Compare the query with every vector, keeping the best `k` in a **max-heap of size k**.
  The worst of the current best is on top; a closer vector replaces it.
- Cost: O(n log k) instead of O(n log n) for sorting every distance.
- Ties are broken by the smaller ID so results are identical on every run.

### Binary heap

- Stores `(distance, id)` pairs in an array. For index `i`: parent `(i - 1) / 2`,
  children `2i + 1` and `2i + 2`. No pointers.
- One heap type with a min/max setting (avoids two copies of the same code).
  - Min-heap: HNSW candidate list (closest unexplored first).
  - Max-heap: best-k results (worst of the best on top, easy to drop).
- `push` / `pop`: O(log n) (sift up / sift down). `top`: O(1). Capacity doubles when full.

### Files

| File | Contents |
| --- | --- |
| `engine/include/vecstore.h`, `engine/src/vecstore.c` | Dynamic vector array |
| `engine/include/distance.h`, `engine/src/distance.c` | `l2sq` and the comparison counter |
| `engine/include/heap.h`, `engine/src/heap.c` | Binary heap (min or max) |
| `engine/include/brute.h`, `engine/src/brute.c` | Brute-force top-k |
| `engine/tests/test_heap.c` | 10,000 random pushes pop out in sorted order |
| `engine/tests/test_brute.c` | Brute-force top-k equals sorting all distances |

### Done when

- `test_heap` and `test_brute` pass (`mingw32-make test`), with no memory errors.
