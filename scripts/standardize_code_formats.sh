#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CHECK=0
VERBOSE=0
TARGET_PATH=""
ENFORCE_NAMES=0

while (( "$#" )); do
  case "$1" in
    --check) CHECK=1; shift ;;
    --verbose) VERBOSE=1; shift ;;
    --path) TARGET_PATH="${2:-}"; shift 2 ;;
    --names) ENFORCE_NAMES=1; shift ;;
    *) shift ;;
  esac
done

if ! command -v clang-format >/dev/null 2>&1; then
  echo "Error: clang-format is not available in PATH" >&2
  exit 1
fi

if [ "$VERBOSE" -eq 1 ]; then
  clang-format --version || true
fi

DIRS=()
if [ -n "$TARGET_PATH" ]; then
  if [ -d "$TARGET_PATH" ]; then
    DIRS=("$TARGET_PATH")
  else
    echo "Error: Specified path does not exist: $TARGET_PATH" >&2
    exit 1
  fi
else
  DIRS=("$ROOT/src" "$ROOT/tests")
fi

mapfile -d '' FILES < <(for d in "${DIRS[@]}"; do if [ -d "$d" ]; then find "$d" -type f \( -name "*.cc" -o -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp" \) -print0; fi; done)

if [ "${#FILES[@]}" -eq 0 ]; then
  echo "No source files found to format."
  exit 0
fi

success=0
failure=0
needs=0
bomconv=0

convert_bom() {
  local f="$1"
  local hex
  hex=$(od -An -t x1 -N 3 "$f" 2>/dev/null | tr -d ' \n')
  if [ "$hex" = "efbbbf" ]; then
    [ "$VERBOSE" -eq 1 ] && echo "Converting UTF-8 BOM: $f"
    tmp="$(mktemp)"
    tail -c +4 "$f" > "$tmp" && mv "$tmp" "$f"
    return 0
  fi
  return 1
}

if [ "$CHECK" -eq 0 ]; then
  for f in "${FILES[@]}"; do
    if convert_bom "$f"; then bomconv=$((bomconv+1)); fi
  done
fi

for f in "${FILES[@]}"; do
  if [ "$CHECK" -eq 1 ]; then
    if ! clang-format --dry-run --Werror -style='{
        BasedOnStyle: Google,
        AccessModifierOffset: -4,
        IndentWidth: 4
      }' "$f" 2>/dev/null; then
      [ "$VERBOSE" -eq 1 ] && echo "NEEDS FORMATTING: $f"
      needs=$((needs+1))
    else
      [ "$VERBOSE" -eq 1 ] && echo "OK: $f"
      success=$((success+1))
    fi
  else
    [ "$VERBOSE" -eq 1 ] && echo "Formatting: $f"
    if clang-format -i -style='{
          BasedOnStyle: Google,
          AccessModifierOffset: -4,
          IndentWidth: 4
      }' "$f" 2>/dev/null; then
      success=$((success+1))
    else
      failure=$((failure+1))
      echo "Failed to format: $f" >&2
    fi
  fi
done

invalid_names=0
if [ "$ENFORCE_NAMES" -eq 1 ]; then
  while IFS= read -r -d '' f; do
    base="$(basename "$f")"
    name="${base%.*}"
    ext="${base##*.}"
    case "$ext" in h|cc) ;; *) echo "[NAME] invalid extension: $f" >&2; invalid_names=$((invalid_names+1)); continue ;; esac
    if ! echo "$name" | grep -Eq '^[a-z0-9_]+$'; then
      echo "[NAME] invalid basename: $f" >&2
      invalid_names=$((invalid_names+1))
    fi
  done < <(find "$ROOT/src" "$ROOT/tests" -type f \( -name "*.h" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.hh" -o -name "*.hxx" \) -print0)
fi

echo ""
if [ "$CHECK" -eq 1 ]; then
  echo "Files checked: ${#FILES[@]}"
  echo "Already formatted: $success"
  echo "Need formatting: $needs"
  if [ "$invalid_names" -gt 0 ]; then echo "Naming violations: $invalid_names"; fi
  if [ "$needs" -gt 0 ] || [ "$invalid_names" -gt 0 ]; then exit 1; else echo "All files are properly formatted!"; exit 0; fi
else
  echo "Files processed: ${#FILES[@]}"
  if [ "$bomconv" -gt 0 ]; then echo "UTF-8 BOM converted: $bomconv"; fi
  echo "Successfully formatted: $success"
  echo "Failed: $failure"
  if [ "$invalid_names" -gt 0 ]; then echo "Naming violations: $invalid_names"; fi
  if [ "$failure" -gt 0 ] || [ "$invalid_names" -gt 0 ]; then echo "Some files failed to format or violate naming." >&2; exit 1; else echo "All files formatted successfully!"; exit 0; fi
fi