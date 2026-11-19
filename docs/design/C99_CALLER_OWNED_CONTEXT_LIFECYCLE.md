# C99 caller-owned cryptographic context lifecycle

## Storage and module lifetime

Query the service's context size for the current library and allocate that
many writable bytes with suitable C object alignment, for example with
`malloc`. Keep the storage alive through finalization or explicit cleanup,
and serialize every access to a mutable context, including cleanup. Context
size queries do not require an operational module. The random-service size
query additionally requires a supported entropy provider and process marker.

Initialize the module successfully before starting or processing an operation.
For otherwise valid calls on an active context, processing during orderly
`SHUTDOWN` returns `ANO_CRYPTO_ERR_NOT_INITIALIZED`. The table below describes
what to do with the context and any caller-owned output when an operation ends.
In particular, streaming GCM finalization may consume the context even when
the operation cannot finish successfully.

Quiesce service calls, then finish or explicitly clear all caller-owned
contexts before calling `ano_crypto_module_shutdown()`. If shutdown has
already occurred, use the cleanup path listed below. Module-wide zeroization
covers module-owned CSP slots. Context storage and buffers holding keys, IVs,
entropy, or output remain the caller's responsibility. A context initialized
from a live AES handle retains private operation state after that handle is
destroyed. Continue such an operation only while the module is operational.
[CSP lifecycle](C99_CSP_LIFECYCLE.md)

After a successful module reinitialization, start each operation with its
context init or instantiate call. Previously cleared storage can be reused.
Import new CSP handles as needed and supply the operation's usual inputs,
including an appropriate CBC IV, a unique GCM nonce per encryption under the
same key, or fresh entropy and a nonce for explicit HMAC_DRBG instantiation.
The random service obtains its own entropy. This procedure applies to orderly
shutdown. Recovery from sticky `ERROR_STATE` requires a fresh process/load.
[Module lifecycle contract](C99_THREAD_SAFETY_POLICY.md#phase-2-module-initshutdown-contract-issue-8)

## Per-service lifecycle

| Context | Initialize and process while operational | Finish or abandon |
| --- | --- | --- |
| HMAC streaming | Query by SHA-256, SHA-384, or SHA-512 and call `ano_crypto_hmac_init()` with the raw key. Submit message chunks with `ano_crypto_hmac_update()`. | `ano_crypto_hmac_final()` emits the selected digest's MAC length and clears the opaque context on success. A short output buffer can be retried while operational. For an abandoned operation, call `ano_crypto_csp_zeroize(ctx, ctx_size)`. |
| AES-CBC | Query for AES/CBC and initialize with a live block-cipher handle, direction, and 16-byte IV. Updates process non-empty whole blocks and advance the chaining state. | Finish after the last update by calling `ano_crypto_cipher_uninit()`, which clears the supplied extent. The same path applies when abandoning the operation. |
| AES-CMAC | Initialize with a live CMAC-purpose handle. Submit arbitrary chunks or use an empty message. | `ano_crypto_cmac_final()` emits a full 16-byte MAC and clears the opaque context on success. Preflight errors such as a short tag buffer allow a retry. Use `ano_crypto_cmac_uninit()` to abandon an operation. |
| AES-256-GCM streaming | Initialize with a live 32-byte block-cipher handle, direction, and 12-byte nonce. Supply AAD before the first non-empty payload update. Withhold decrypt output until the tag is verified. | A correct-direction finalization attempt on an active context consumes and clears it, including on a tag error or module-state failure. A wrong-direction final call is rejected. On decrypt final failure or abandonment, wipe all unverified plaintext already emitted. Use `ano_crypto_aes256_gcm_stream_uninit()` to abandon the context. |
| HMAC_DRBG | Instantiate HMAC_DRBG-SHA-256 with caller-supplied entropy and nonce. Use `ano_crypto_drbg_generate()` and supply entropy explicitly to `ano_crypto_drbg_reseed()`. | End with `ano_crypto_drbg_uninstantiate()`, which clears the supplied extent. For fully cleared context storage, `ano_crypto_drbg_get_state()` reports `UNINSTANTIATED` even outside `OPERATIONAL`. |
| Random service | Check availability and call `ano_crypto_random_init()`. Generate and reseed through the random-service APIs. After a detected process change, reseed in the new process before retrying generation. | End with `ano_crypto_random_uninit()`, which clears the supplied extent. A valid generate request rejected during shutdown clears its output. |

## Explicit cleanup lengths

All of the explicit cleanup paths above are available outside `OPERATIONAL`.
Pass the queried size to clear the full opaque context. Each accepts
`(NULL, 0)` as a no-op and rejects `NULL` with a nonzero length.

- HMAC's buffer helper, CBC uninit, DRBG uninstantiate, and random uninit
  clear exactly the supplied writable length and return `ANO_CRYPTO_OK`,
  including for zero-length and short calls. A length below the queried size
  leaves cleanup incomplete. A larger valid length clears trailing bytes too.
- CMAC and streaming GCM uninit clear only the queried extent on success.
  A nonzero short length clears the supplied prefix and returns
  `ANO_CRYPTO_ERR_BUFFER_TOO_SMALL`. A zero length with non-null storage
  returns the same error without writing. Retry with the full queried size.

## Regression coverage and follow-up

The existing KATs and negative and stress tests cover normal operation, output
validation, and service-specific error handling. The CBC, CMAC, and GCM tests
also exercise use of captured operation state after handle destruction.
The lifecycle regressions below use synthetic operation inputs. Random-service
success cases use the supported platform provider and report status only.

| Existing CTest entry | Lifecycle regression coverage |
| --- | --- |
| `ano_crypto_hash_hmac_api_negative_tests` | Shutdown admission, explicit clearing of abandoned streams, and fresh streams with short-final retry and final clearing for all three HMAC variants. |
| `ano_crypto_cipher_api_negative_tests` | Active CBC context cleanup during shutdown, supplied-length clearing, stale handle rejection, and operation with a fresh context. |
| `ano_crypto_cmac_api_negative_tests` | Final admission during shutdown, active-context cleanup, stale handle rejection, and verified MAC and clearing after fresh init. Existing cases check zero, short, and oversized cleanup lengths. |
| `ano_crypto_aes256_gcm_streaming_negative_tests` | Shutdown update rejection and output clearing, context and tag clearing on shutdown final, zero/short/oversized cleanup bounds, stale handle rejection, and update/final after fresh init. |
| `ano_crypto_drbg_api_negative_tests` | Shutdown generate/reseed rejection, clearing of the supplied extent, state query after cleanup, and fresh instantiation and generation. |
| `ano_crypto_random_service_tests` | Shutdown generate/reseed rejection, output and context clearing, and fresh init/reseed/generate on supported platforms. |

The caller procedure above defines the supported restart sequence. Whether to
enforce invalidation for contexts carried across reinitialization, and what
completion and output guarantees apply to calls overlapping shutdown or a
fatal transition, require specialized review. [Issue #11](https://github.com/anothel/AnoCrypto-C/issues/11)
