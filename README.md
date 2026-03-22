# html-renderer

Renders HTML files to a window. Written in C++ using Raylib for the graphics side. No browser engine, no libxml, no regex just a state machine tokenizer and a two-pass layout loop.

```bash
./main                        # opens test.html
./main path/to/something.html
```

The bundled `test.html` is a chapter from [Web Browser Engineering](https://browser.engineering/), from which this project is followed.

---

## How the parser works

Most toy HTML parsers use regex or just scan for `<` and `>`. That breaks on anything real. This one uses an explicit state machine with named states that match how the [HTML5 spec](https://html.spec.whatwg.org/multipage/parsing.html) describes tokenization:

```
DATA → TAG_OPEN → TAG_NAME → emit START_TAG
     → END_TAG_OPEN → END_TAG_NAME → emit END_TAG
     → ENTITY → decode → back to DATA
     → COMMENT → skip until >
```

Characters in `DATA` accumulate into a buffer and flush as `TEXT_NODE` tokens when a `<` is hit. The renderer then walks the flat token list and updates font size / cursor position / color as it goes.

## What's implemented

Tags: `<h1>` (bigger font, line break), `<p>` (paragraph spacing), `<div>` (block break), `<a>` (blue + underline), `<code>` (preserves newlines inside). Content inside `<style>`, `<script>`, and `<title>` is skipped.

Entities: `&amp;`, `&lt;`, `&gt;`, `&nbsp;`, `&copy;`, `&hellip;`, `&ndash;`, and a bunch more.

Attributes are recognized by the parser (it transitions to an ATTRIBUTES state when it sees a space inside a tag) but their values aren't extracted yet, so `href` on links does nothing. That's the main thing missing.

## Build

**Windows (MinGW)**

```bash
make
```

The repo includes a prebuilt `lib/libraylib.a` for Windows so no extra install needed.

**Linux**

```bash
apt install libraylib-dev

make
```

The Makefile detects the platform and links accordingly `-lraylib -lopengl32 -lgdi32 -lwinmm` on Windows, `-lraylib -lX11` on Linux. The vendored static lib in `lib/` is Windows-only and is ignored on Linux, so the system install is required. If your distro's Raylib package is too old you can build from source:

```bash
git clone https://github.com/raysan5/raylib
cd raylib/src && make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

## Related

Part of a larger thing — [http-cli](https://github.com/Addy897/http-cli) handles fetching over HTTP/HTTPS. 

## What's left

- Actually use `href` attributes so links are clickable
- `<img>` image renddering
- `<ul>`/`<ol>` list rendering
- more CSS support
