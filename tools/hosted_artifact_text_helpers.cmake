cmake_minimum_required(VERSION 3.16)

function(ano_crypto_require_hosted_text _path _relative _size)
    set(_offset 0)
    while(_offset LESS_EQUAL _size)
        # HEX preserves NUL and the original bytes. Match complete pairs in
        # bounded chunks to keep the regex stack and string copies small.
        file(READ "${_path}" _hex OFFSET ${_offset} LIMIT 1024 HEX)
        if(_hex STREQUAL "")
            break()
        endif()
        # Allow TAB, LF, CR, 0x20-0x7e and 0x80-0xff, as in the byte scan.
        if(NOT _hex MATCHES "^(0[9ad]|[2-6][0-9a-f]|7[0-9a-e]|[89a-f][0-9a-f])+$")
            message(FATAL_ERROR "Hosted artifact must be text-only: ${_relative}")
        endif()
        string(LENGTH "${_hex}" _hex_length)
        math(EXPR _offset "${_offset} + ${_hex_length} / 2")
    endwhile()
    if(NOT _offset EQUAL _size)
        message(FATAL_ERROR "Hosted artifact size changed during text scan: ${_relative}")
    endif()
endfunction()
