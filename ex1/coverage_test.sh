#!/usr/bin/env bash
# coverage_test.sh — build + run + coverage for: main.cpp, graph.cpp, point.cpp
# Produces ONLY: graph.cpp.gcov, point.cpp.gcov, and prints a compact summary.

set -Eeuo pipefail

# ---- Config ----
ALL_SRCS=(main.cpp graph.cpp point.cpp)
REPORT_SRCS=(graph.cpp point.cpp)   # only these are reported/kept
BUILD_DIR="build/coverage"
BIN="$BUILD_DIR/graph_cov"

CXX=${CXX:-g++}
CXXFLAGS="-std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage"

# pick matching gcov tool
if "$CXX" --version 2>/dev/null | grep -qi clang; then
  GCOV_TOOL=(llvm-cov gcov)
else
  GCOV_TOOL=(gcov)
fi

msg(){ printf "\033[1;34m==== %s\033[0m\n" "$*"; }
err(){ printf "\033[1;31mERROR: %s\033[0m\n" "$*" >&2; }

# ---- Clean & prepare ----
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
# remove old .gcov so we only see fresh ones
find . -maxdepth 1 -type f -name '*.gcov' -delete 2>/dev/null || true

# ---- Compile per file (creates .gcno) ----
msg "Compiling sources"
for s in "${ALL_SRCS[@]}"; do
  "$CXX" $CXXFLAGS -c "$s" -o "$BUILD_DIR/${s%.cpp}.o"
done
ls "$BUILD_DIR"/*.gcno >/dev/null 2>&1 || { err "no .gcno created"; exit 1; }

# ---- Link ----
msg "Linking"
objs=()
for s in "${ALL_SRCS[@]}"; do objs+=("$BUILD_DIR/${s%.cpp}.o"); done
"$CXX" $CXXFLAGS -o "$BIN" "${objs[@]}"

# ---- Run tests (creates .gcda) ----
msg "Running tests"
"$BIN" -h  >/dev/null 2>&1 || true
"$BIN" -t  >/dev/null 2>&1 || true
"$BIN" -v 6  -e 8  -s 123  >/dev/null 2>&1 || true
"$BIN" -v 1  -e 0  -s 1    >/dev/null 2>&1 || true
"$BIN" -v 5  -e 10 -s 42   >/dev/null 2>&1 || true
"$BIN" -v -5 -e 5  -s 42   >/dev/null 2>&1 || true
"$BIN"                     >/dev/null 2>&1 || true
ls "$BUILD_DIR"/*.gcda >/dev/null 2>&1 || { err "no .gcda created"; exit 1; }

# ---- Generate .gcov only for requested sources ----
msg "Generating .gcov files (only for ${REPORT_SRCS[*]})"
# if gcov returns non-zero, do not kill the script (some versions do that when sources are empty)
set +e
"${GCOV_TOOL[@]}" -b -c -o "$BUILD_DIR" "${REPORT_SRCS[@]}" >/dev/null 2>&1
set -e

# Keep only our target .gcov (just in case)
shopt -s nullglob
for f in *.gcov; do
  case "$f" in
    graph.cpp.gcov|point.cpp.gcov) : ;;  # keep
    *) rm -f -- "$f" ;;
  esac
done
shopt -u nullglob

# ---- Compact summary (robust; never fails the script) ----
echo "Coverage Results:"
echo "=================="

summarize() {
  set +e  # avoid set -e pitfalls inside this function
  local src_file="$1"
  local gcov_file="${src_file}.gcov"

  echo "Processing $src_file..."
  echo "gcov output: File '$src_file'"

  if [[ ! -f "$gcov_file" ]]; then
    echo "No .gcov file created for $src_file"
    echo "---"
    set -e
    return
  fi

  local total_lines=0 covered_lines=0
  # parse gcov lines: "<count>: <lineno>: <code>"
  while IFS=: read -r count _ rest; do
    count="${count#"${count%%[![:space:]]*}"}"  # ltrim
    [[ -z "$count" || "$count" == "-" ]] && continue
    if [[ "$count" == "#####" ]]; then
      ((total_lines++))
    elif [[ "$count" =~ ^[0-9]+$ ]]; then
      ((total_lines++))
      (( count > 0 )) && ((covered_lines++))
    fi
  done < "$gcov_file"

  if (( total_lines > 0 )); then
    # integer percentage (rounded)
    local pct=$(( (covered_lines*100 + total_lines/2) / total_lines ))
    printf "Lines executed:%d%% of %d\n" "$pct" "$total_lines"
    printf "Creating '%s.gcov'\n" "$src_file"
    printf "%s: %d%% (%d/%d lines)\n" "$src_file" "$pct" "$covered_lines" "$total_lines"
  else
    echo "No executable lines found"
  fi
  echo "---"
  set -e
}

summarize "graph.cpp"
summarize "point.cpp"

echo "Files created:"
[[ -f graph.cpp.gcov ]] && echo "- graph.cpp.gcov"
[[ -f point.cpp.gcov ]] && echo "- point.cpp.gcov"
