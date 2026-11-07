# Legacy Diagen corpus

`diagen-legacy.zip` contains the unchanged public `data/diagen.text` and
`data/diagen_eng.text` from commit
`73cabb98df24fe7a5840d81e66ba538b595ba185` of this repository. These are the
historical CP1251 `DG` archives, under the same licensing as the original data.
No private assets or savegames are included.

The fixture is immutable evidence for legacy input/save compatibility and
pre-ID dialogue behavior. CMake extracts it into the external build tree.
Tests compare the typed production-parser models with the new explicit-ID
archives, then load old RU/EN progress into the other locale and back. Cell
state is compared with the old name-based grid-save semantics; question IDs
and answer indices are independently required to survive in full.

Do not regenerate these old archives from current translations or overwrite
them when updating the new packaged resources. An intentional new content
change must be distinguished from this historical compatibility baseline.

`diagen-en-progress.zip` adds synthetic nested saves, not personal/campaign
savegames. Three all-topic v2/v3/v4 records were generated with the original
English 73cabb9 answer layout before canonical regrouping. Twelve small records
plus `english-indices.tsv` exercise every valid old slot of all 18 affected
questions (147 version/slot cases). They are immutable migration evidence.
The tests verify the Russian-canonical result in either locale and a subsequent
v5 save/load, rather than preserving the old English fragment indices.
