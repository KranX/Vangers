# Dialogue sources

These are the authoritative sources of `../diagen.text` and `../diagen_eng.text`.

- `ru/` and `en/`: single-language UTF-8 `.query` / `.text` files, with
  `utf8_text` markers, explicit `[#topic.id]` headers and `{#topic.id}` links.
- Root `.dil` files and `room.lst`: unchanged shared scenario/lookup metadata.
  Their historical CP1251 source is decoded by the existing source compiler;
  the resulting archives, including this metadata, are UTF-8 (`DU`).

Change a question's localized **second** header, never its first ID header.
Keep the same ID in both locales. Scenario cells, answer levels, molecule names
and variant action codes are not translations. See
[`diagen-stable-ids.md`](../../docs/diagen-stable-ids.md) for the full contract.

The existing `DIAGEN_TEST` compiler reads localized files from the selected
subdirectory, but writes the same logical `data/<name>` archive keys. Historical
flat source fixtures remain supported. Obsolete bilingual `.query` / `.text`
sources, backup `sav__.dil`, unused `query.lst` and Python-2 link checker have
been replaced by these sources and production-parser tests.

## Rebuilding archives

Configure an **external** build with `BUILD_TESTING=ON`, then build
`diagen_source_test`. From the repository root, generate reviewed outputs without
writing into the source tree:

```sh
OUT=$(mktemp -d /tmp/vangers-diagen-XXXXXXXX)
cmake \
  -DSOURCE_PARSER=/absolute/build/src/diagen_source_test \
  -DSOURCE_ROOT="$PWD/data/data" \
  -DARCHIVE_ROOT="$PWD/data" \
  -DTEST_ROOT="$OUT" \
  -DVERIFY_ARCHIVES=OFF \
  -P tests/diagen_shipped_roundtrip.cmake
```

After reviewing the change, copy `$OUT/ru/diagen_new.text` to `data/diagen.text`
and `$OUT/en/diagen_new.text` to `data/diagen_eng.text`. Run:

```sh
cmake --build /absolute/build
ctest --test-dir /absolute/build --output-on-failure -R diagen
```

`diagen_shipped_roundtrip` recompiles both locales and requires byte-identical
archives, so an edit cannot silently leave a stale packaged resource. The
shipped-runtime tests compare Russian with the immutable historical corpus plus
the two approved choice-markup fixes. English structure must match Russian;
its existing wording is checked separately, allowing the reviewed repairs only.
See [the audit](../../docs/diagen-english-audit.md). New translations need an
explicit update to that wording expectation, never a rewrite of historical
compatibility fixtures.

Quoted inventory prose contains intentional CRLF inside tokens. Git attributes
disable newline conversion for the localized sources; preserve those token
bytes when changing source formatting. Header/record line formatting itself
does not control the displayed layout.
