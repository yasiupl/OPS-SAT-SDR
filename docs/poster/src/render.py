#!/usr/bin/env python3
"""Render the A0 IAC 2024 poster to PDF + PNG preview.

Usage:  .venv/bin/python render.py [DOI]
  e.g.  .venv/bin/python render.py 10.5281/zenodo.1234567
Without an argument the placeholder 10.5281/zenodo.XXXXXXX is used.
"""
import pathlib, subprocess, sys, json
from playwright.sync_api import sync_playwright

HERE = pathlib.Path(__file__).resolve().parent
OUT = HERE.parent
DOI = sys.argv[1] if len(sys.argv) > 1 else "10.5281/zenodo.XXXXXXX"

html = (HERE / "poster.html").read_text(encoding="utf-8").replace("{{DOI}}", DOI)
build = HERE / "_build.html"
build.write_text(html, encoding="utf-8")

pdf = OUT / "iac2024_poster_A0.pdf"
png = OUT / "iac2024_poster_A0_preview.png"

CHECK_JS = r"""
() => {
  const main = document.querySelector('main');
  const r = main.getBoundingClientRect();
  const issues = [];
  // content spilling into a 4th column shows up as scrollWidth > clientWidth
  if (main.scrollWidth > main.clientWidth + 1) issues.push('main overflows horizontally: ' + main.scrollWidth + ' > ' + main.clientWidth);
  let maxBottom = 0, lastCol = null;
  for (const el of main.querySelectorAll('p, h2, h3, figure')) {
    for (const rc of el.getClientRects()) {
      if (rc.right > r.right + 1) issues.push('element right of main: ' + el.tagName + ' ' + el.textContent.slice(0,40));
      if (rc.bottom > r.bottom + 1) issues.push('element below main: ' + el.tagName + ' ' + el.textContent.slice(0,40));
      maxBottom = Math.max(maxBottom, rc.bottom);
    }
  }
  for (const el of document.querySelectorAll('header, footer, header *, footer *')) {
    if (el.scrollWidth > el.clientWidth + 1 && getComputedStyle(el).overflow !== 'visible') issues.push('clipped: ' + el.className);
  }
  for (const img of document.images) if (!img.complete || img.naturalWidth === 0) issues.push('image not loaded: ' + img.src);
  // free space at the bottom of each column (px at 96dpi)
  const colW = (main.clientWidth - parseFloat(getComputedStyle(main).columnGap) * 2) / 3;
  const cols = [0,0,0];
  for (const el of main.querySelectorAll('section')) for (const rc of el.getClientRects()) {
    const i = Math.min(2, Math.floor((rc.left - r.left + 5) / (colW + 1)));
    cols[i] = Math.max(cols[i], rc.bottom);
  }
  const mm = px => (px * 25.4 / 96).toFixed(1);
  return {issues, freeBottomMM: cols.map(b => mm(r.bottom - b)), fonts: [...document.fonts].filter(f=>f.status==='loaded').map(f=>f.family)};
}
"""

with sync_playwright() as p:
    browser = p.chromium.launch(executable_path="/usr/bin/google-chrome", args=["--allow-file-access-from-files"])
    page = browser.new_page(viewport={"width": 3179, "height": 4494})  # 841x1189 mm at 96 dpi
    page.goto(build.as_uri())
    page.evaluate("document.fonts.ready")
    page.wait_for_load_state("networkidle")
    page.emulate_media(media="print")
    report = page.evaluate(CHECK_JS)
    page.pdf(path=str(pdf), width="841mm", height="1189mm", print_background=True,
             margin={"top": "0", "right": "0", "bottom": "0", "left": "0"}, prefer_css_page_size=True)
    browser.close()

subprocess.run(["pdftoppm", "-png", "-singlefile", "-scale-to-y", "2000", "-scale-to-x", "-1", str(pdf), str(png.with_suffix(""))], check=True)
print(json.dumps(report, indent=1))
print(subprocess.run(["pdfinfo", str(pdf)], capture_output=True, text=True).stdout)
print("DOI:", DOI)
print("PDF:", pdf)
print("PNG:", png)
