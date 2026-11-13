# Embedded launcher localization

This fork ships exactly two launcher UI locales:

- `en_GB` — English (United Kingdom), the default and fallback language.
- `pl_PL` — Polish.

Both compiled Qt translation catalogs are embedded in `translations.qrc` and loaded
from Qt resources at runtime. The launcher does not fetch translation indexes or
catalogs, inspect a local translation cache, or expose other locales in the selector.

## Updating a catalog

To update either supported language, replace the corresponding Qt QM file in this
directory while retaining its canonical filename:

- `mmc_en_GB.qm`
- `mmc_pl_PL.qm`

The resource collection already references both files, so normal CMake builds embed
the updated catalogs into the launcher. Keep the locale identifiers and filenames
consistent; do not add additional language resources.

The English UK catalog is installed first. When Polish is selected, the Polish
catalog is installed above it, allowing untranslated Polish messages to fall back
to English UK. If the Polish resource cannot be loaded, the saved language is reset
to `en_GB`. If an English UK catalog entry is absent, Qt falls back to the original
source string.

Because this is an independently maintained Prism Launcher 8.4-era fork, catalog
updates should be built or reviewed against this fork's own source strings. Qt matches
translations by context and source text: obsolete entries are ignored, while missing
or changed entries use the English UK catalog or the original source string.

## Bundled catalog integrity

The catalogs included in this source revision were supplied as compiled Qt QM files.
Their SHA-1 values are recorded here to make accidental replacement or truncation
easy to detect:

| Catalog | SHA-1 |
|---|---|
| `mmc_en_GB.qm` | `1ecde2e7ba71a483724c2a80bbcd6603d57db51f` |
| `mmc_pl_PL.qm` | `c63d3d200b11e3cb56ec892ee18fcae976f07e5f` |

The upstream translation index used `pl` as its Polish entry key; this fork's
application-facing locale identifier remains `pl_PL`. No runtime dependency on that
index or its download service remains.
