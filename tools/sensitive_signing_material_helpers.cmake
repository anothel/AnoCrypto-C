cmake_minimum_required(VERSION 3.16)

# Return a short reason when text contains a private-key PEM marker or, when
# requested, a line-oriented sensitive assignment. Marker strings are assembled
# in pieces so this policy helper does not flag its own source archive.
function(ano_crypto_detect_sensitive_signing_material_text
         _content _check_assignments _out_reason)
    string(TOLOWER "${_content}" _content_lower)

    string(CONCAT _private_begin_regex
        "-----begin " "[^\r\n]*private" " key[^\r\n]*-----")
    string(REGEX MATCH "${_private_begin_regex}"
        _private_marker_match "${_content_lower}")
    if(_private_marker_match)
        set(${_out_reason} "private-key PEM marker" PARENT_SCOPE)
        return()
    endif()

    if(_check_assignments)
        string(CONCAT _private_key_assignment "private" "_key")
        string(CONCAT _certificate_key_assignment "certificate" "_private_key")
        string(CONCAT _signing_token_assignment "signing" "_token")
        string(CONCAT _entropy_assignment "entropy" "_material")
        string(CONCAT _seed_assignment "seed" "_material")
        string(CONCAT _drbg_assignment "drbg" "_state")
        foreach(_assignment IN ITEMS
                "${_private_key_assignment}"
                "${_certificate_key_assignment}"
                "${_signing_token_assignment}"
                "${_entropy_assignment}"
                "${_seed_assignment}"
                "${_drbg_assignment}")
            string(REGEX MATCH
                "(^|[\r\n])[ \t]*(export[ \t]+|set[ \t]+)?${_assignment}[ \t]*="
                _assignment_match "${_content_lower}")
            if(_assignment_match)
                set(${_out_reason} "sensitive assignment" PARENT_SCOPE)
                return()
            endif()
        endforeach()

        foreach(_assignment IN ITEMS
                aes_key aes_iv plaintext ciphertext cmac_tag)
            string(REGEX MATCH
                "(^|[\r\n])[ \t]*(export[ \t]+|set[ \t]+)?${_assignment}[ \t]*="
                _assignment_match "${_content_lower}")
            if(_assignment_match)
                set(${_out_reason} "sensitive cryptographic value assignment" PARENT_SCOPE)
                return()
            endif()
        endforeach()

        set(_status_lines "${_content_lower}")
        string(REPLACE ";" "\;" _status_lines "${_status_lines}")
        string(REPLACE "\r\n" "\n" _status_lines "${_status_lines}")
        string(REPLACE "\r" "\n" _status_lines "${_status_lines}")
        string(REPLACE "\n" ";" _status_lines "${_status_lines}")
        foreach(_status_line IN LISTS _status_lines)
            foreach(_status_key IN ITEMS token actual_signature signature_value
                    detached_signature timestamp_response)
                if(_status_line MATCHES
                   "^[ \t]*(export[ \t]+|set[ \t]+)?${_status_key}[ \t]*=[ \t]*(.*)$")
                    set(_status_value "${CMAKE_MATCH_2}")
                    string(STRIP "${_status_value}" _status_value)
                    if(NOT _status_value MATCHES
                       "^(no|none|absent|not_created|not_run|unprovisioned|unreviewed)$")
                        set(${_out_reason} "sensitive status/value assignment" PARENT_SCOPE)
                        return()
                    endif()
                endif()
            endforeach()
            if(_status_line MATCHES
               "^[ \t]*(export[ \t]+|set[ \t]+)?(entropy|seed|signature|[a-z0-9_]*token[a-z0-9_]*)[ \t]*=[ \t]*(.*)$")
                set(_status_key_match "${CMAKE_MATCH_2}")
                set(_status_value "${CMAKE_MATCH_3}")
                string(STRIP "${_status_value}" _status_value)
                # CTest emits label-time summary rows such as
                # `signature = 0.09 sec*proc (2 tests)`. This is duration
                # metadata, not cryptographic material, so retain the evidence
                # log while keeping every other non-placeholder assignment
                # fail-closed.
                if(_status_value MATCHES "^[0-9]+([.][0-9]+)?[ \t]+sec[*]proc[ \t]+[(][0-9]+[ \t]+tests?[)]$")
                    continue()
                endif()
                if(NOT _status_value MATCHES
                   "^(no|none|absent|not_created|not_run|unprovisioned|unreviewed)$")
                    set(${_out_reason} "sensitive generic value assignment" PARENT_SCOPE)
                    return()
                endif()
            endif()
        endforeach()

        string(CONCAT _json_sensitive_keys
            "private" "_key|certificate" "_private_key|signing" "_token|"
            "entropy" "_material|seed" "_material|drbg" "_state|"
            "aes" "_key|aes" "_iv|plain" "text|cipher" "text|cmac" "_tag|"
            "entropy|seed|signature|[a-z0-9_]*to" "ken[a-z0-9_]*|"
            "to" "ken|actual" "_signature|signature" "_value|"
            "detached" "_signature|timestamp" "_response")
        string(REGEX MATCH
            "(^|[\r\n{,])[ \t]*[\"']?(${_json_sensitive_keys})[\"']?[ \t]*:"
            _json_sensitive_match "${_content_lower}")
        if(_json_sensitive_match)
            set(${_out_reason} "sensitive JSON/record key" PARENT_SCOPE)
            return()
        endif()
    endif()

    set(${_out_reason} "" PARENT_SCOPE)
endfunction()

function(ano_crypto_detect_sensitive_signing_material_file
         _path _check_assignments _out_reason)
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}" OR IS_SYMLINK "${_path}")
        set(${_out_reason} "missing or unsafe file" PARENT_SCOPE)
        return()
    endif()
    string(TOLOWER "${_path}" _path_lower)
    if(_path_lower MATCHES "[.](key|pem|sig|asc|p12|pfx|jks|keystore|token|tsq|tsr|ed25519)$")
        set(${_out_reason} "credential/signature-like filename" PARENT_SCOPE)
        return()
    endif()
    file(SIZE "${_path}" _size)
    if(_size GREATER 16777216)
        set(${_out_reason} "file exceeds 16 MiB content-scan limit" PARENT_SCOPE)
        return()
    endif()
    file(READ "${_path}" _content)
    ano_crypto_detect_sensitive_signing_material_text(
        "${_content}" "${_check_assignments}" _reason)
    set(${_out_reason} "${_reason}" PARENT_SCOPE)
endfunction()
