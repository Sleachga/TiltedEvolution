# NOTICE — this is a modified fork

**This repository is not Tilted Online.** It is a personal fork of it, and if you
arrived here looking for the real project you want
**<https://github.com/tiltedphoques/TiltedEvolution>** instead. Issues, pull requests
and support requests belong there, not here.

GPLv3 §5(a) asks that modified files carry prominent notices saying they changed and
when. This file says who did the modifying and why; the per-file convention at the
bottom says how each individual change is marked.

---

## The facts

| | |
|---|---|
| **Upstream project** | [tiltedphoques/TiltedEvolution](https://github.com/tiltedphoques/TiltedEvolution) — "Tilted Online", by The Together Team |
| **Forked by** | Sanford Leach ([@Sleachga](https://github.com/Sleachga)) |
| **Cut from** | tag **`v1.8.0`**, commit **`9c23efa422bbc1e5c06eef5522ca73971a513e35`** (2025-11-06) |
| **Working branch** | **`sandman`** |
| **Licence** | Unchanged: **GPL-3.0-or-later**, with `Code/immersive_launcher` under **LGPL-2.1** (it derives from CitizenFX/FiveM code). See `LICENSE`. |
| **Upstream Nexus page** | <https://www.nexusmods.com/skyrimspecialedition/mods/69993> |

`LICENSE` is byte-for-byte the upstream file and stays that way. Nothing in this fork
relicenses anything, and nothing here restricts a right the GPL grants you. If those
two statements ever stop being true, that is a bug — please open an issue.

## What the `sandman` branch is for

A private Skyrim Together server for a handful of friends, and the client builds those
friends run. The changes are operational and small by intention — build fixes,
quality-of-life tweaks, occasional instrumentation — not a competing distribution and
not a hard fork. There is no roadmap here and no promise of maintenance.

Builds made from this branch are handed to a small group of named people. **That is
distribution**, which is exactly why this repository is public rather than a private
mirror: it is how those people get the corresponding source the GPL entitles them to.
Keeping it public is not a courtesy, it is the compliance mechanism. If it were ever
made private while builds were in circulation, we would owe every recipient the source
directly.

## What is actually changed here

Ask git rather than trusting prose that will go stale:

```bash
git fetch origin
git diff --stat v1.8.0..origin/sandman     # every file that differs from upstream
git log --oneline v1.8.0..origin/sandman   # and why
```

## Upstream conventions this fork follows

Mirrored so a diff against upstream stays readable, and so any change here could be
offered back without reformatting:

- **`CODE_GUIDELINES.md`** is upstream's and still applies here — C++20; `aArgument`,
  `cConst`, `pPointer`, `m_member`, `s_static`, `g_global`; `PascalCase` functions;
  brace on its own line; no exceptions and no throwing STL. **Run `clang-format`**
  (`.clang-format` is in the repo root).
- **Conventional commit prefixes**, per that same file: `feat:`, `tweak:`, `fix:`,
  `refactor:`. Upstream also ships a commit template at `.gitmessage/conventional_commit`
  and drives releases from `.releaserc`.
- **There is no upstream `CONTRIBUTING.md`.** The contributing rules live in
  upstream's `README.md`: fork, PR into **`dev`** (never `master` or `prerel`), keep to
  the code guidelines. Attribution/credits upstream are handled by the GitHub
  contributor graph — its Nexus page's File Credits section simply points at
  <https://github.com/tiltedphoques/TiltedEvolution/graphs/contributors> — so there is
  no separate AUTHORS or CREDITS file to add a name to, here or there.

## Per-file change notes — the convention

Every upstream file this fork modifies gets a note at the top of the file, immediately
after any existing licence or include-guard header and before the first declaration.
Add the file to the list on your first change to it; **update the date and reason when
you change it again** rather than stacking a second block.

C++ (`.cpp`, `.h`, `.inl`):

```cpp
// MODIFIED IN THE `sandman` FORK — not upstream Tilted Online.
//   2026-09-03  Widen the party sync window so a 90 ms link stops desyncing.
//   Fork: https://github.com/Sleachga/TiltedEvolution (branch: sandman)
//   Upstream: https://github.com/tiltedphoques/TiltedEvolution
```

Lua, `.gitignore`-style, shell, and `xmake.lua`:

```lua
-- MODIFIED IN THE `sandman` FORK — not upstream Tilted Online.
--   2026-09-03  <one line: what changed and why>
--   Fork: https://github.com/Sleachga/TiltedEvolution (branch: sandman)
```

YAML (workflows) and `.editorconfig`-style:

```yaml
# MODIFIED IN THE `sandman` FORK — not upstream Tilted Online.
#   2026-09-03  <one line: what changed and why>
#   Fork: https://github.com/Sleachga/TiltedEvolution (branch: sandman)
```

TypeScript / SCSS in `Code/skyrim_ui`:

```ts
/* MODIFIED IN THE `sandman` FORK — not upstream Tilted Online.
 *   2026-09-03  <one line: what changed and why>
 *   Fork: https://github.com/Sleachga/TiltedEvolution (branch: sandman)
 */
```

**Rules that make this worth doing:**

- **One line per change, saying why.** "Fixed build" is not a reason; "xmake 2.9.8
  cannot detect VS 2026, so pin the runner" is.
- **Never remove or edit an upstream copyright or licence header** to make room. The
  note goes *after* it.
- **Files this fork creates from scratch** — like this one — do not need the note.
  They are ours and are obviously not upstream's.
- **Keep it out of vendored trees.** `Libraries/` and `modules/` are third-party or
  submodules; if you must patch one, the note goes in the commit and in this file's
  git history, not scattered through someone else's code.
- **A revert removes the note.** If a file goes back to matching upstream exactly, the
  note is now a lie — delete it in the same commit.

## Files modified so far

Keep this in sync with `git diff --stat v1.8.0..sandman`; if they disagree, git wins.

| File | First changed | Why |
|---|---|---|
| `.github/workflows/windows.yml` | 2026-09-03 | `windows-latest` now resolves to a VS 2026 image; xmake 2.9.8 cannot detect VS 2026, so the CI matrix is pinned to `windows-2022`. |
| `xmake.lua` | 2026-09-03 | Toolchain pins. Now also forces every `tiltedcore` require to v0.2.9 — the `Libraries/TiltedConnect` and `Libraries/TiltedHooks` submodules still ask for v0.2.7, whose unpinned nested `mimalloc` require resolves to v3.5.0, where `mi_malloc_size` is gone. |
| `Code/client/xmake.lua` | 2026-09-03 | The client keeps its own debug `tiltedcore` pin; bumped to v0.2.9 to match the root. |
| `Dockerfile` | 2026-09-03 | Pin xmake to 2.9.8; the unpinned installer now pulls 3.x onto a tree declaring `set_xmakever("2.8.5")`. |
| `Code/client/Services/Generic/OverlayService.cpp` | 2026-09-03 | `GetCellName` returned the worldspace outdoors, so the party menu's Location column read "Skyrim" for every outdoor player. |
| `Code/client/Games/Skyrim/BSGraphics/BSGraphicsRenderer.cpp` | 2026-09-04 | Skyrim holds a `ClipCursor` rectangle over its own window and never releases it, so the mouse could not reach a second monitor after alt-tabbing. The per-frame render hook now clears the clip while the game is not the foreground window. |
| `Code/client/Services/Generic/PlayerService.cpp` | 2026-09-05 | `OnServerSettingsReceived` re-captured the player's own difficulty on every live settings change, by which point it had already been forced to the server's value — so disconnecting left them stuck on the server's difficulty. Now captured only on the first settings message of a session. |

---

*Not legal advice. This file describes what the GPL asks for and what this fork does
about it.*
