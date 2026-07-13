# Canonical-path checks shared by hosted-artifact creation and verification.
cmake_minimum_required(VERSION 3.16)

function(_ano_hosted_normalize_path _path _out)
    get_filename_component(_absolute "${_path}" ABSOLUTE)
    file(TO_CMAKE_PATH "${_absolute}" _normalized)
    string(REGEX REPLACE "/+$" "" _normalized "${_normalized}")
    if(WIN32)
        string(TOLOWER "${_normalized}" _normalized)
    endif()
    set(${_out} "${_normalized}" PARENT_SCOPE)
endfunction()

function(_ano_hosted_real_path _path _out)
    get_filename_component(_real "${_path}" REALPATH)
    _ano_hosted_normalize_path("${_real}" _normalized)
    set(${_out} "${_normalized}" PARENT_SCOPE)
endfunction()

# Reject a symlink or junction at this entry while allowing aliases in parent
# directories outside the caller-owned root.
function(ano_crypto_reject_hosted_root_alias _path _description)
    if(NOT EXISTS "${_path}" OR NOT IS_DIRECTORY "${_path}")
        message(FATAL_ERROR "${_description} is unavailable: ${_path}")
    endif()
    if(IS_SYMLINK "${_path}")
        message(FATAL_ERROR "${_description} may not be a symlink: ${_path}")
    endif()
    get_filename_component(_parent "${_path}" DIRECTORY)
    get_filename_component(_name "${_path}" NAME)
    _ano_hosted_real_path("${_parent}" _parent_real)
    _ano_hosted_normalize_path("${_parent_real}/${_name}" _expected)
    _ano_hosted_real_path("${_path}" _actual)
    if(NOT _actual STREQUAL _expected)
        message(FATAL_ERROR "${_description} may not be a junction or path alias: ${_path}")
    endif()
endfunction()

# The caller performs its existing lexical relative-path validation first.
# Comparing the resolved child with root-real/relative rejects leaf and
# intermediate symlinks/junctions, including aliases that still point inside.
function(ano_crypto_require_regular_unaliased_hosted_child
         _root _relative _description)
    if(NOT EXISTS "${_root}" OR NOT IS_DIRECTORY "${_root}")
        message(FATAL_ERROR "Hosted artifact root is unavailable: ${_root}")
    endif()
    set(_full "${_root}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "${_description} is missing or unsafe: ${_relative}")
    endif()
    _ano_hosted_real_path("${_root}" _root_real)
    _ano_hosted_normalize_path("${_root_real}/${_relative}" _expected)
    _ano_hosted_real_path("${_full}" _actual)
    file(RELATIVE_PATH _contained "${_root_real}" "${_actual}")
    file(TO_CMAKE_PATH "${_contained}" _contained)
    if(IS_ABSOLUTE "${_contained}" OR
       _contained MATCHES "(^|/)[.][.](/|$)" OR
       NOT _actual STREQUAL _expected)
        message(FATAL_ERROR
            "${_description} traverses a symlink, junction, or root boundary: ${_relative}")
    endif()
endfunction()

# Canonicalize an existing path, or a not-yet-created direct child of an
# existing directory. Used before destructive output-directory replacement.
function(ano_crypto_get_hosted_candidate_real_path _path _out)
    if(EXISTS "${_path}" OR IS_SYMLINK "${_path}")
        _ano_hosted_real_path("${_path}" _resolved)
    else()
        get_filename_component(_parent "${_path}" DIRECTORY)
        get_filename_component(_name "${_path}" NAME)
        if(NOT IS_DIRECTORY "${_parent}")
            message(FATAL_ERROR
                "Hosted artifact output parent is unavailable: ${_parent}")
        endif()
        _ano_hosted_real_path("${_parent}" _parent_real)
        _ano_hosted_normalize_path("${_parent_real}/${_name}" _resolved)
    endif()
    set(${_out} "${_resolved}" PARENT_SCOPE)
endfunction()
