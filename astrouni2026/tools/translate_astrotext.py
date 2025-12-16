import argparse
import re
import sys
from pathlib import Path


def _unescape_value(value: str) -> str:
    out = []
    i = 0
    while i < len(value):
        ch = value[i]
        if ch != "\\" or i + 1 >= len(value):
            out.append(ch)
            i += 1
            continue
        n = value[i + 1]
        if n == "n":
            out.append("\n")
            i += 2
        elif n == "r":
            out.append("\r")
            i += 2
        elif n == "t":
            out.append("\t")
            i += 2
        elif n == "\\":
            out.append("\\")
            i += 2
        else:
            out.append(ch)
            i += 1
    return "".join(out)


def _escape_value(value: str) -> str:
    out = []
    for ch in value:
        if ch == "\\":
            out.append("\\\\")
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\r":
            out.append("\\r")
        elif ch == "\t":
            out.append("\\t")
        else:
            out.append(ch)
    return "".join(out)


def _protect_tokens(text: str):
    repl = {}

    def _store(prefix: str, s: str) -> str:
        key = f"__{prefix}{len(repl)}__"
        repl[key] = s
        return key

    text = re.sub(r"%([1-9]\d*)", lambda m: _store("PH", m.group(0)), text)
    text = re.sub(r"<[^>]+>", lambda m: _store("TAG", m.group(0)), text)

    return text, repl


def _restore_tokens(text: str, repl: dict) -> str:
    for k, v in repl.items():
        text = text.replace(k, v)
    return text


def _get_translator(from_code: str, to_code: str, install: bool):
    try:
        from argostranslate import package, translate
    except Exception as e:
        raise RuntimeError("argostranslate is not installed") from e

    installed_langs = translate.get_installed_languages()
    from_lang = next((l for l in installed_langs if l.code == from_code), None)
    to_lang = next((l for l in installed_langs if l.code == to_code), None)

    if (from_lang is None or to_lang is None) and install:
        package.update_package_index()
        available = package.get_available_packages()
        pkg = next((p for p in available if p.from_code == from_code and p.to_code == to_code), None)
        if pkg is None:
            raise RuntimeError(f"No translation package available for {from_code}->{to_code}")
        package.install_from_path(pkg.download())
        installed_langs = translate.get_installed_languages()
        from_lang = next((l for l in installed_langs if l.code == from_code), None)
        to_lang = next((l for l in installed_langs if l.code == to_code), None)

    if from_lang is None or to_lang is None:
        raise RuntimeError(f"Translation languages not installed for {from_code}->{to_code}")

    translation = from_lang.get_translation(to_lang)

    def _translate(text: str) -> str:
        return translation.translate(text)

    return _translate


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("astrotext_path")
    ap.add_argument("--install", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    path = Path(args.astrotext_path)
    data = path.read_text(encoding="utf-8")
    lines = data.splitlines()

    current_lang = None
    de_map = {}

    for line in lines:
        t = line.strip()
        if t.startswith("[") and t.endswith("]"):
            current_lang = t[1:-1].strip()
            continue
        if not current_lang or current_lang != "de":
            continue
        if not t or t.startswith("#") or t.startswith(";"):
            continue
        eq = t.find("=")
        if eq <= 0:
            continue
        key = t[:eq].strip()
        value = line.split("=", 1)[1]
        de_map[key] = _unescape_value(value)

    translate_fn = None
    if not args.dry_run:
        translate_fn = _get_translator("de", "en", install=args.install)

    out_lines = []
    current_lang = None
    changed = 0
    skipped = 0

    for line in lines:
        t = line.strip()
        if t.startswith("[") and t.endswith("]"):
            current_lang = t[1:-1].strip()
            out_lines.append(line)
            continue

        if current_lang != "en":
            out_lines.append(line)
            continue

        if not t or t.startswith("#") or t.startswith(";"):
            out_lines.append(line)
            continue

        eq = t.find("=")
        if eq <= 0:
            out_lines.append(line)
            continue

        key = t[:eq].strip()
        en_value_esc = line.split("=", 1)[1]
        en_value = _unescape_value(en_value_esc)
        de_value = de_map.get(key)
        if de_value is None:
            out_lines.append(line)
            continue

        if en_value != de_value:
            skipped += 1
            out_lines.append(line)
            continue

        if args.dry_run:
            out_lines.append(line)
            continue

        lead_ws = re.match(r"^\s*", de_value).group(0)
        trail_ws = re.search(r"\s*$", de_value).group(0)
        core = de_value[len(lead_ws):]
        if trail_ws:
            core = core[:-len(trail_ws)]

        protected, repl = _protect_tokens(core)
        translated = translate_fn(protected)
        translated = _restore_tokens(translated, repl)
        translated = lead_ws + translated + trail_ws

        out_lines.append(f"{key}={_escape_value(translated)}")
        changed += 1

    if args.dry_run:
        print("dry-run")
        print(f"en lines with en==de and de present: {sum(1 for k, v in de_map.items() if True)}")
        return 0

    path.write_text("\n".join(out_lines) + "\n", encoding="utf-8")

    print(f"translated: {changed}")
    print(f"skipped (en!=de): {skipped}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
