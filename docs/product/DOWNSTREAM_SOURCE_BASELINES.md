# Downstream Source Baselines

Status: `4_OF_4_IMPORTED_WITH_ONE_IDENTITY_CONFLICT`

## Accepted archives

| ID | Repository | Exact revision | Source archive SHA-256 | CURRENT ONLY/collection SHA-256 | Intake state |
|---|---|---|---|---|---|
| `ASK-COMM` | AnoSecureKit Community | `694459ebe497d15ba75ef76a52fa7c36ddd7bcce` | `951f523d9444450e3894f72fae5a45594441530b559dc3d7b4766258ee414484` | `dc5d27afbf19b903105057aac4a47f07e2f6f8c499b1d251314104c9b83ede2c` | `IMPORTED` |
| `ASK-ENT` | AnoSecureKit Enterprise | `3a62318fce27985d091bb5445fc59d0fca7033a2` | `ebf8a4cbbdda8103f4678cbe28c56b1758da26761ecfcd3f5b6990a22c8f10c5` | `13448abe43fa45307d46f70e8b44b5af7a16c95d2835b1f0c6094a134df694b3` | `IMPORTED_WITH_DEPENDENCY_IDENTITY_CONFLICT` |
| `APKI-COMM` | AnoPKI Community | `ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5` | `2f5b5750631a4af39293e10a420a6667ff430e682a3ab24bf3f3e54dbc298ac2` | `0c9c9b0a1c9deaf17554ce22770e2fc5c9da426f85ddadccb516e04c91125268` | `IMPORTED` |
| `APKI-ENT` | AnoPKI Enterprise | `68b379e7684c9d8b44c53728949774088352a9d9` | `af5acc349deee69650a4767a51fdc0214cd529561a9ece906a18ab0eebe7033d` | `ed55d2aa2ebdd561dc2549d01d1bd8f3fd0bd2d2144c3ce3b74083d86fb47704` | `IMPORTED` |

All delivered SHA-256 values were recomputed locally and matched the supplied manifests.

## Authority rule

- Exact source archives are code authority.
- CURRENT ONLY/collection packs are curated requirement evidence.
- A document revision marker that conflicts with its enclosing collection manifest does not override the exact archive/collection identity.
- OpenSSL calls are implementation evidence, not a proposed AnoCrypto-C API.
