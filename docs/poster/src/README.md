# IAC 2024 poster — A0 rebuild (source)

Single-page A0 portrait poster (841 × 1189 mm) for paper IAC-24,B6,IPB,15,x82617,
presented at the 75th International Astronautical Congress, Milan, 14–18 October 2024.

The rendered PDF and a preview PNG live in the parent folder (`docs/poster/`), committed as:

- `jasiukowicz_2024_iac_poster_opssat_sdr.pdf`
- `jasiukowicz_2024_iac_poster_opssat_sdr_preview.png`

## How to render

System packages: Google Chrome (the script looks for `/usr/bin/google-chrome`) and `poppler-utils` (`pdftoppm`, `pdfinfo`).

From this folder:

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
.venv/bin/python render.py 10.5281/zenodo.XXXXXXX
```

Pass the real DOI as the argument when you have one. With no argument the placeholder `10.5281/zenodo.XXXXXXX` is used.

`render.py` writes these names one directory up, into `docs/poster/`:

- `iac2024_poster_A0.pdf` — 841 × 1189 mm, 1 page, fonts embedded
- `iac2024_poster_A0_preview.png` — 2000 px tall

Rename them to the committed filenames above if you are updating the copies in this repository. The script prints an overflow report (`issues` must be empty) and `pdfinfo`.

`playwright` is required to render. `qrcode` and `fonttools` were used when preparing the QR code and font files; they are not imported by `render.py`.

## Layout

- `poster.html` — layout and verbatim text; `{{DOI}}` is replaced at render time.
- `render.py` — renders the HTML to PDF/PNG with headless Chrome.
- `img/` — figures used in the layout (including the updated author photo and circular OPS-SAT logo).
- `img_orig/` — original downloads (iPosters / Cloudinary and the ESA author photo).
- `fonts/` — static instances of Source Sans 3 and Roboto Condensed (see licences below).
- `_build.html` is generated at render time and is gitignored.

## Font licences

Both families may be redistributed with this source:

- **Source Sans 3** (Adobe, SIL Open Font License 1.1) — see `fonts/OFL.txt`.
- **Roboto Condensed** (Google, Apache License 2.0) — see `fonts/LICENSE-Apache-2.0.txt`.

## Licence of the poster

Licensed under CC BY 4.0, except third-party images (©ESA and others as credited).
