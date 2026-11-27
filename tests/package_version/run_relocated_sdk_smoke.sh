#!/usr/bin/env bash
set -euo pipefail

fail() {
    printf '%s\n' "$*" >&2
    exit 1
}

run_step() {
    local name=$1 duration=$2 status
    shift 2
    if timeout --kill-after=5s "$duration" "$@"; then
        printf '%s exit 0.\n' "$name"
    else
        status=$?
        printf '%s failed (exit %s).\n' "$name" "$status" >&2
        exit "$status"
    fi
}

case "${1-}:$#" in
    :0) missing_config=0 ;;
    --missing-config:1) missing_config=1 ;;
    *) fail 'Usage: run_relocated_sdk_smoke.sh [--missing-config]' ;;
esac

[[ $(uname -s) == Linux ]] || fail 'This smoke test requires Linux.'
for tool in cmake gcc make ldd realpath sha256sum timeout; do
    command -v "$tool" >/dev/null || fail "Required tool unavailable: $tool"
done

source_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd -P)
tmp_parent=$(realpath -e -- "${TMPDIR:-/tmp}")
scratch=$(mktemp -d -- "$tmp_parent/anocrypto-relocated.XXXXXXXX")
printf 'Scratch directory retained: %s\n' "$scratch"

original="$scratch/original"
relocated="$scratch/relocated SDK"
sdk_build="$scratch/sdk-build"
probe_build="$scratch/probe-build"

# Use the same clean loader environment for the loader check and the probe.
unset LD_LIBRARY_PATH LD_PRELOAD LD_AUDIT LD_DEBUG LD_DEBUG_OUTPUT
unset LD_PROFILE LD_PROFILE_OUTPUT LD_TRACE_LOADED_OBJECTS
export LC_ALL=C

run_step 'SDK configure' 120s cmake -S "$source_root" -B "$sdk_build" -G 'Unix Makefiles' \
    -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON \
    -DANO_CRYPTO_BUILD_TESTS=OFF -DANO_CRYPTO_BUILD_EXAMPLES=OFF \
    -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF \
    -DCMAKE_INSTALL_LIBDIR=lib "-DCMAKE_INSTALL_PREFIX=$original"
run_step 'SDK build' 300s cmake --build "$sdk_build" --parallel 4
run_step 'SDK install' 120s cmake --install "$sdk_build"

original_library=$(realpath -e -- "$original/lib/libanocrypto_c.so")
[[ -f $original_library && $original_library == "$original/lib/"* ]] ||
    fail 'Original library resolves outside the installed SDK.'
original_digest=$(sha256sum <"$original_library")
mv -T -- "$original" "$relocated"
[[ ! -e $original && ! -L $original ]] || fail 'Old prefix still exists.'
printf 'Old prefix absent.\n'

relocated_library=$(realpath -e -- "$relocated/lib/libanocrypto_c.so")
[[ -f $relocated_library && $relocated_library == "$relocated/lib/"* ]] ||
    fail 'Relocated library resolves outside the moved SDK.'
relocated_digest=$(sha256sum <"$relocated_library")
[[ $original_digest == "$relocated_digest" ]] || fail 'Library digest changed after relocation.'
printf 'Original library SHA-256: %s\n' "${original_digest%% *}"
printf 'Relocated library SHA-256: %s\n' "${relocated_digest%% *}"

package_root=$relocated
config_dir="$relocated/lib/cmake/AnoCryptoC"
if ((missing_config)); then
    # Keep the negative search entirely within an empty, owned root.
    package_root="$scratch/empty-package-root"
    mkdir -- "$package_root"
    config_dir="$package_root/missing"
fi

run_step 'Probe configure' 120s env -u CMAKE_PREFIX_PATH -u AnoCryptoC_ROOT -u ANOCRYPTOC_ROOT \
    cmake -S "$source_root/tests/package_version/exact" -B "$probe_build" -G 'Unix Makefiles' \
    -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release "-DAnoCryptoC_DIR:PATH=$config_dir" \
    "-DCMAKE_FIND_ROOT_PATH:PATH=$package_root" -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
    -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF
((missing_config == 0)) || fail 'Missing-config case configured unexpectedly. Refusing to build or run the probe.'

selected_config=$(sed -n 's/^AnoCryptoC_DIR:[^=]*=//p' "$probe_build/CMakeCache.txt")
[[ $selected_config == "$config_dir" && -f $selected_config/AnoCryptoCConfig.cmake ]] ||
    fail "Unexpected cached package directory: $selected_config"
printf 'Selected config: %s/AnoCryptoCConfig.cmake\n' "$selected_config"

run_step 'Probe build' 120s cmake --build "$probe_build" --parallel 4
probe="$probe_build/anocrypto_c_exact_package_version_probe"
loader_log="$scratch/loader.txt"
run_step 'Loader inspection' 30s ldd "$probe" >"$loader_log"
cat -- "$loader_log"

soname='^[[:space:]]*libanocrypto_c\.so(\.[0-9]+)*[[:space:]]+=>[[:space:]]+(/.+) \(0x[0-9a-fA-F]+\)$'
loaded_library=
while IFS= read -r line; do
    [[ $line != *'=> not found'* ]] || fail 'A shared dependency was not found.'
    if [[ $line =~ $soname ]]; then
        [[ -z $loaded_library ]] || fail 'Multiple AnoCryptoC libraries resolved.'
        loaded_library=$(realpath -e -- "${BASH_REMATCH[2]}")
    fi
done <"$loader_log"
[[ $loaded_library == "$relocated_library" ]] ||
    fail "Unexpected loader resolution: ${loaded_library:-missing}"
printf 'Loader resolved: %s\n' "$loaded_library"

run_step 'Probe' 30s "$probe"
