# CYD Air Monitor — Docs Site

Interactive **board-first** documentation built with [VitePress](https://vitepress.dev/).

## Local development

```bash
cd docs-site
npm install
npm run dev
```

Open `http://localhost:5173/cyd-air-monitor/`

## What's here

| Section | URL | Content |
|---------|-----|---------|
| Board Explorer | `/boards/` | Pick hardware → pins, flash commands, gotchas, references |
| Gotchas | `/boards/gotchas` | Filterable pitfalls by board and topic |
| References | `/boards/references` | 40+ curated links — pinouts, repos, libraries, forums |
| Compare | `/boards/compare` | Side-by-side env / driver / pin matrix |
| Flash guide | `/guide/getting-started` | Minimal firmware upload steps |

## Data files (single source of truth)

| File | Purpose |
|------|---------|
| `.vitepress/data/boards.ts` | Specs, pins, build flags per env |
| `.vitepress/data/gotchas.ts` | Pitfalls with severity and fixes |
| `.vitepress/data/references.ts` | External links with descriptions and tags |

## GitHub Pages

Pushes to `main` deploy via `.github/workflows/docs.yml` →  
https://deejpotter.github.io/cyd-air-monitor/
