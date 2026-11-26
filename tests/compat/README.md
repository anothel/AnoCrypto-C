# Release compatibility consumers

The standalone C99 project requires the installed CMake package
`AnoCryptoC` version `0.1.0` exactly. Both consumers expect the P3-10
prerelease runtime version `0.1.0-p3-10-restricted-sha1-ocsp-certid`:

1. a current-header/current-library consumer, which checks the installed source
   surface and package discovery;
2. a retained P1-29-header/current-library consumer, which checks the frozen
   `0.1.0-p1-29-cmac-hardening` header contract against the installed P3-10
   library.

The retained-header probe references the 69 baseline ABI-0 functions. The
current-header probe references those functions plus 15 additions, for 84
references. These counts describe the probe bindings. A full export inventory
requires a separate check. Both modes check stable enum, macro, handle-width,
and build-identity layout assumptions. Runtime output is limited to version,
header mode, data-model sizes, counts, and pass/fail status.
It does not print keys, messages, tags, digests, entropy, generated random
bytes, seed material, or DRBG state.

This is an engineering regression guard. It does not establish compatibility
for untested compiler ABIs, calling conventions, loaders, or all semantic
behavior.

## Standalone Linux run

Use CMake and CTest 3.20 or newer with Unix Makefiles. From the repository
root, set the absolute path to a freshly installed shared SDK and choose a
new build directory. Point `sdk_config_dir` at the directory containing the
installed `AnoCryptoCConfig.cmake`. The example uses a `lib` install layout.

```sh
sdk_prefix="/absolute/path/to/installed SDK"
sdk_config_dir="$sdk_prefix/lib/cmake/AnoCryptoC"
build_dir="/absolute/path/to/compat build"
retained_headers="$PWD/abi/baselines/0.1.0-p1-29-cmac-hardening/include"

cmake -S tests/compat -B "$build_dir" -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release "-DAnoCryptoC_DIR=$sdk_config_dir" \
  "-DANO_CRYPTO_P129_BASELINE_INCLUDE_DIR=$retained_headers"
cmake --build "$build_dir"
env -u LD_LIBRARY_PATH -u LD_PRELOAD ctest --test-dir "$build_dir" --output-on-failure
```

CTest runs `current_header_compatibility` and
`p129_header_current_library_compatibility`. Confirm that `AnoCryptoC_DIR` in
the build's `CMakeCache.txt` matches the installed config directory.

Run `env -u LD_LIBRARY_PATH -u LD_PRELOAD ldd` on each built executable,
`anocrypto_c_current_header_compatibility_consumer` and
`anocrypto_c_p129_header_compatibility_consumer`. Both should resolve
`libanocrypto_c.so.0` inside the supplied SDK prefix. Record the resolved path
and hash the file with `sha256sum`, quoting its path. The original Q9 SDK
requires separate verification against the selected artifact.
