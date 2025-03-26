# Locate the glfw3 library
#
# This module defines the following variables (for backward compatibility):
#
# GLFW3_FOUND          - True if GLFW3 and required components were found.
# GLFW3_INCLUDE_DIRS   - Include directories needed to use GLFW3. (Legacy: GLFW3_INCLUDE_DIR)
# GLFW3_LIBRARIES      - Libraries needed to link against GLFW3. (Legacy: GLFW3_LIBRARY)
# GLFW3_VERSION_STRING - The version of GLFW3 found (e.g., "3.3.8").
# GLFW3_VERSION_MAJOR  - The major version number.
# GLFW3_VERSION_MINOR  - The minor version number.
# GLFW3_VERSION_PATCH  - The patch/revision version number.
#
# It also defines the IMPORTED target:
#
# GLFW::glfw           - The main imported target for using GLFW3.
#                        Link against this target using target_link_libraries().
#                        It automatically handles include directories, link libraries,
#                        and required compile definitions.
#
# Configuration Variables:
#
# GLFW3_ROOT           - User-defined variable: Set this to the root directory of a GLFW3
#                        installation if it's not in standard system paths.
#                        An environment variable $ENV{GLFW3_ROOT} can also be used.
# GLFW3_USE_STATIC_LIBS- Set to TRUE to prefer linking against static libraries (.a, .lib)
#                        if available. Defaults to FALSE (prefer shared libraries).
# CMAKE_PREFIX_PATH    - Standard CMake variable: Add GLFW3 install prefixes here.
#
# Hints:
# - On Windows, ensure the architecture (x64/x86) of the found library matches
#   your project's architecture.
# - If using a package manager (vcpkg, Conan, etc.), prefer using their integration
#   methods (e.g., setting CMAKE_TOOLCHAIN_FILE for vcpkg) over this Find module,
#   as they often provide pre-configured targets. This module acts as a fallback.
#
# Original source: https://github.com/daw42/glslcookbook (heavily modified)
# Last Updated: 2025-03-26 (Based on common practices)

# Guard against multiple inclusions (though find_package handles this)
if(TARGET GLFW::glfw)
  return()
endif()

# Include necessary CMake modules
include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations) # Helper for Debug/Release libs

# --- 1. Try Config-Mode First ---
# Modern packages often provide glfw3Config.cmake or glfw3-config.cmake
# Use find_package in CONFIG mode silently first.
# We pass down version/component requests if provided by the caller.
set(_glfw3_find_args ${GLFW3_FIND_ARGS}) # Capture original arguments like VERSION, EXACT, QUIET, REQUIRED, COMPONENTS
# Common config package names are lowercase 'glfw3' or 'GLFW3'
find_package(glfw3 CONFIG QUIET NO_MODULE ${_glfw3_find_args}) # Check for glfw3Config.cmake
if(NOT glfw3_FOUND)
  find_package(GLFW3 CONFIG QUIET NO_MODULE ${_glfw3_find_args}) # Check for GLFW3Config.cmake
  if(GLFW3_FOUND) # If found via uppercase name
    set(glfw3_FOUND TRUE) # Normalize found status
    # Add alias if needed (e.g., if config provided GLFW::glfw but user expects glfw::glfw)
    if(TARGET GLFW::glfw AND NOT TARGET glfw::glfw)
        add_library(glfw::glfw ALIAS GLFW::glfw)
    endif()
  endif()
endif()


if(glfw3_FOUND)
  message(STATUS "Found GLFW3 (via config mode)")

  # If config mode succeeded, assume it set up targets correctly (e.g., glfw::glfw or GLFW::glfw)
  # We just need to ensure our expected variables are set for compatibility.
  # Prefer using the target (e.g., glfw::glfw) directly in your CMakeLists.txt!

  # Create alias for backward compatibility if needed
  if(TARGET glfw::glfw AND NOT TARGET GLFW::glfw)
    add_library(GLFW::glfw ALIAS glfw::glfw)
    message(STATUS "Creating alias GLFW::glfw for config-provided target glfw::glfw")
  endif()

  # Populate legacy variables if the config file didn't, deriving from the target
  if(TARGET GLFW::glfw)
    if(NOT DEFINED GLFW3_INCLUDE_DIRS)
      get_target_property(_glfw3_includes GLFW::glfw INTERFACE_INCLUDE_DIRECTORIES)
      if(_glfw3_includes)
        set(GLFW3_INCLUDE_DIRS ${_glfw3_includes})
        # Set singular version for extreme backward compatibility
        list(GET _glfw3_includes 0 GLFW3_INCLUDE_DIR)
      else()
        set(GLFW3_INCLUDE_DIRS "")
        set(GLFW3_INCLUDE_DIR "")
      endif()
      unset(_glfw3_includes)
    endif()

    if(NOT DEFINED GLFW3_LIBRARIES)
      get_target_property(_glfw3_libs GLFW::glfw INTERFACE_LINK_LIBRARIES)
      # This might contain other targets. Extracting a simple library path is difficult/unreliable.
      # If the target LOCATION is set, use that.
      get_target_property(_glfw3_loc GLFW::glfw LOCATION)
       if(_glfw3_loc)
           set(GLFW3_LIBRARIES ${_glfw3_loc})
           set(GLFW3_LIBRARY ${_glfw3_loc}) # Singular legacy variable
       elseif(_glfw3_libs)
            # Attempt to find a non-target library path in the list (heuristic)
            set(_glfw3_found_lib_path "")
            foreach(_lib IN LISTS _glfw3_libs)
                if(NOT TARGET ${_lib} AND (IS_ABSOLUTE ${_lib} OR EXISTS ${_lib}))
                     set(_glfw3_found_lib_path ${_lib})
                     break()
                endif()
            endforeach()
             if(_glfw3_found_lib_path)
                 set(GLFW3_LIBRARIES ${_glfw3_found_lib_path})
                 set(GLFW3_LIBRARY ${_glfw3_found_lib_path})
             else()
                 # Cannot easily determine legacy variables from target-only dependencies
                 set(GLFW3_LIBRARIES "") # Indicate it links via target
                 set(GLFW3_LIBRARY "")
             endif()
            unset(_glfw3_found_lib_path)
       else()
            set(GLFW3_LIBRARIES "")
            set(GLFW3_LIBRARY "")
       endif()
        unset(_glfw3_loc)
        unset(_glfw3_libs)
    endif()

    # Version information (config mode *should* set this if version was requested)
    if(NOT DEFINED GLFW3_VERSION_STRING AND DEFINED glfw3_VERSION)
      set(GLFW3_VERSION_STRING ${glfw3_VERSION})
      set(GLFW3_VERSION_MAJOR ${glfw3_VERSION_MAJOR})
      set(GLFW3_VERSION_MINOR ${glfw3_VERSION_MINOR})
      set(GLFW3_VERSION_PATCH ${glfw3_VERSION_PATCH})
    endif()
  endif()

  # Set the primary FOUND variable based on config result
  set(GLFW3_FOUND TRUE)

  # Config mode handles components, versions etc. Return early.
  return()
endif()

message(STATUS "GLFW3 config-mode failed, trying module mode.")

# --- 2. Module-Mode Search ---

# Define search paths
# Use user hint, environment hint, standard CMake paths, and common system locations
set(_glfw3_search_hints
    ${GLFW3_ROOT}          # User-provided CMake variable
    $ENV{GLFW3_ROOT}       # User-provided environment variable
    )
set(_glfw3_search_paths
    ${CMAKE_PREFIX_PATH}   # Standard CMake search paths
    ${CMAKE_FRAMEWORK_PATH} # For macOS frameworks
    ${CMAKE_APPBUNDLE_PATH} # For macOS app bundles
    ${CMAKE_INSTALL_PREFIX} # Often useful if built locally
    # Common system install locations
    /usr/local
    /usr
    /opt/local             # MacPorts
    /opt/homebrew          # Homebrew on Apple Silicon
    /opt                   # General /opt installations
    # Common Windows manual install locations
    C:/glfw C:/tools/glfw C:/dev/glfw
    "C:/Program Files/glfw" "C:/Program Files (x86)/glfw"
    # Paths from original script (keep for compatibility, but lower priority)
    # "${CMAKE_SOURCE_DIR}/libs" # Generally discouraged unless vendored carefully
    # "C:/Program Files (x86)/glfw/lib-vc2022" # Too specific, covered by suffixes now
    )

# Define path suffixes for include and library directories
set(_glfw3_include_suffixes include include/GLFW)
set(_glfw3_lib_suffixes lib lib64 lib/x64 lib/x86_64 lib/amd64 lib-vc # Common MSVC suffixes
                        lib/Release lib/RelWithDebInfo lib/MinSizeRel lib/Debug
                        lib-static # If static libs are in separate dirs
                        )

# Search for the header file (GLFW/glfw3.h)
find_path(GLFW3_INCLUDE_DIR
  NAMES GLFW/glfw3.h
  HINTS ${_glfw3_search_hints}
  PATHS ${_glfw3_search_paths}
  PATH_SUFFIXES ${_glfw3_include_suffixes}
  DOC "GLFW3 include directory"
)

# --- Version Parsing (from header) ---
set(GLFW3_VERSION_STRING "0.0.0") # Default if parsing fails
if(GLFW3_INCLUDE_DIR AND EXISTS "${GLFW3_INCLUDE_DIR}/GLFW/glfw3.h")
  # Read the header file content once
  file(READ "${GLFW3_INCLUDE_DIR}/GLFW/glfw3.h" _glfw3_header_content)

  # Extract version numbers using robust regex
  string(REGEX MATCH "#define[ \t]+GLFW_VERSION_MAJOR[ \t]+([0-9]+)" _glfw3_major_match "${_glfw3_header_content}")
  if(_glfw3_major_match) set(GLFW3_VERSION_MAJOR ${CMAKE_MATCH_1}) endif()

  string(REGEX MATCH "#define[ \t]+GLFW_VERSION_MINOR[ \t]+([0-9]+)" _glfw3_minor_match "${_glfw3_header_content}")
  if(_glfw3_minor_match) set(GLFW3_VERSION_MINOR ${CMAKE_MATCH_1}) endif()

  string(REGEX MATCH "#define[ \t]+GLFW_VERSION_REVISION[ \t]+([0-9]+)" _glfw3_patch_match "${_glfw3_header_content}")
  if(_glfw3_patch_match) set(GLFW3_VERSION_PATCH ${CMAKE_MATCH_1}) endif()

  # Construct version string if all parts found
  if(DEFINED GLFW3_VERSION_MAJOR AND DEFINED GLFW3_VERSION_MINOR AND DEFINED GLFW3_VERSION_PATCH)
    set(GLFW3_VERSION_STRING "${GLFW3_VERSION_MAJOR}.${GLFW3_VERSION_MINOR}.${GLFW3_VERSION_PATCH}")
  else()
    # Reset partial findings if parsing failed
    unset(GLFW3_VERSION_MAJOR)
    unset(GLFW3_VERSION_MINOR)
    unset(GLFW3_VERSION_PATCH)
    set(GLFW3_VERSION_STRING "0.0.0") # Indicate failure
  endif()

  unset(_glfw3_header_content)
  unset(_glfw3_major_match)
  unset(_glfw3_minor_match)
  unset(_glfw3_patch_match)
endif()


# --- Library Search ---

# Handle static vs. shared preference
set(_glfw3_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
if(GLFW3_USE_STATIC_LIBS)
    message(STATUS "Searching for GLFW3 static libraries.")
    if(WIN32)
        # On Windows, static libs are typically .lib, shared import libs are also .lib
        # We might need extra checks later if a .lib could be static or import.
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    elseif(APPLE)
         set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib ${CMAKE_FIND_LIBRARY_SUFFIXES}) # .a preferred for static
    else() # Linux, etc.
         set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib ${CMAKE_FIND_LIBRARY_SUFFIXES}) # .a preferred for static
    endif()
else()
    message(STATUS "Searching for GLFW3 shared libraries (or static as fallback).")
    if(WIN32)
        # Prefer .lib (import lib) then .dll (though linking .dll directly is rare)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})
    elseif(APPLE)
         set(CMAKE_FIND_LIBRARY_SUFFIXES .dylib .tbd ${CMAKE_FIND_LIBRARY_SUFFIXES}) # .dylib preferred for shared
    else() # Linux, etc.
         set(CMAKE_FIND_LIBRARY_SUFFIXES .so ${CMAKE_FIND_LIBRARY_SUFFIXES}) # .so preferred for shared
    endif()
endif()

# Define library names to search for (including debug/release variants)
set(_glfw3_names glfw3 glfw) # Base names
set(_glfw3_names_debug glfw3d glfw_d) # Common debug suffixes/names
set(_glfw3_names_release ${_glfw3_names})

# Find Release Library
find_library(GLFW3_LIBRARY_RELEASE
  NAMES ${_glfw3_names_release}
  HINTS ${_glfw3_search_hints}
  PATHS ${_glfw3_search_paths}
  PATH_SUFFIXES ${_glfw3_lib_suffixes}
  DOC "GLFW3 Release library"
)
# Find Debug Library (fallback to release name if specific debug name not found)
find_library(GLFW3_LIBRARY_DEBUG
  NAMES ${_glfw3_names_debug} ${_glfw3_names_release}
  HINTS ${_glfw3_search_hints}
  PATHS ${_glfw3_search_paths}
  PATH_SUFFIXES ${_glfw3_lib_suffixes}
  DOC "GLFW3 Debug library"
)

# Restore original library suffixes
set(CMAKE_FIND_LIBRARY_SUFFIXES ${_glfw3_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
unset(_glfw3_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES)

# Select the best library based on build configuration (uses _DEBUG and _RELEASE vars)
select_library_configurations(GLFW3) # Sets GLFW3_LIBRARY and GLFW3_LIBRARIES

# --- Handle Results with FindPackageHandleStandardArgs ---
find_package_handle_standard_args(GLFW3
  FOUND_VAR GLFW3_FOUND
  REQUIRED_VARS GLFW3_LIBRARIES GLFW3_INCLUDE_DIR # Use plural LIBRARIES now
  VERSION_VAR GLFW3_VERSION_STRING # Pass the parsed version for comparison
  # Optional: Define failure message
  # FAIL_MESSAGE "Could not find the GLFW3 library. Please set GLFW3_ROOT or ensure it's in CMAKE_PREFIX_PATH."
)

# --- Create Imported Target (if found in module mode) ---
if(GLFW3_FOUND AND NOT TARGET GLFW::glfw)
  message(STATUS "Found GLFW3 (via module mode)")

  # Set legacy variables (plural versions)
  set(GLFW3_INCLUDE_DIRS ${GLFW3_INCLUDE_DIR})
  set(GLFW3_LIBRARIES ${GLFW3_LIBRARIES}) # Already set by select_library_configurations

  # Create the imported target based on found library type (best guess)
  get_filename_component(_glfw3_lib_ext ${GLFW3_LIBRARY} EXT) # Use combined selected lib
  set(_glfw3_target_type UNKNOWN) # Default
  set(_glfw3_is_static FALSE)
  set(_glfw3_link_defs "")

  if(GLFW3_USE_STATIC_LIBS AND ("${_glfw3_lib_ext}" STREQUAL CMAKE_STATIC_LIBRARY_SUFFIX OR ("${_glfw3_lib_ext}" STREQUAL ".lib" AND WIN32)))
      # If static requested AND found .a (or .lib on Win), assume static
      set(_glfw3_target_type STATIC)
      set(_glfw3_is_static TRUE)
      message(STATUS "Creating GLFW::glfw as STATIC IMPORTED target")
  elseif("${_glfw3_lib_ext}" STREQUAL CMAKE_SHARED_LIBRARY_SUFFIX OR "${_glfw3_lib_ext}" STREQUAL ".tbd") # .so, .dylib, .tbd
      set(_glfw3_target_type SHARED)
      message(STATUS "Creating GLFW::glfw as SHARED IMPORTED target")
      if(WIN32) set(_glfw3_link_defs $<$<PLATFORM_ID:Windows>:GLFW_DLL>) endif()
  elseif("${_glfw3_lib_ext}" STREQUAL ".lib" AND WIN32)
      # Found .lib on Windows, could be static or import lib.
      # Heuristic: Check if a corresponding DLL exists in likely places (../bin)
      get_filename_component(_glfw3_lib_dir ${GLFW3_LIBRARY} DIRECTORY)
      get_filename_component(_glfw3_lib_name ${GLFW3_LIBRARY} NAME_WE) # Name without ext
      if(EXISTS "${_glfw3_lib_dir}/../bin/${_glfw3_lib_name}.dll" OR EXISTS "${_glfw3_lib_dir}/${_glfw3_lib_name}.dll")
          set(_glfw3_target_type SHARED)
          set(_glfw3_link_defs $<$<PLATFORM_ID:Windows>:GLFW_DLL>)
          message(STATUS "Creating GLFW::glfw as SHARED IMPORTED target (found .lib with corresponding .dll)")
      else()
           # Assume static if DLL not found nearby and static wasn't forced off
           if(NOT DEFINED GLFW3_USE_STATIC_LIBS OR GLFW3_USE_STATIC_LIBS)
               set(_glfw3_target_type STATIC)
               set(_glfw3_is_static TRUE)
               message(STATUS "Creating GLFW::glfw as STATIC IMPORTED target (found .lib without obvious .dll)")
           else()
               message(WARNING "Found GLFW3 .lib on Windows but no corresponding .dll nearby, and static libs were not preferred. Target type set to UNKNOWN.")
           endif()
      endif()
      unset(_glfw3_lib_dir)
      unset(_glfw3_lib_name)
  endif()

  # Add the library target
  if(NOT _glfw3_target_type STREQUAL "UNKNOWN")
    add_library(GLFW::glfw ${_glfw3_target_type} IMPORTED)

    # Set properties using configuration-specific variables if they exist
    set_target_properties(GLFW::glfw PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
      INTERFACE_COMPILE_DEFINITIONS "${_glfw3_link_defs}" # Add GLFW_DLL if needed
    )
     # Map configuration locations (_DEBUG, _RELEASE variables set by select_library_configurations)
     if(EXISTS "${GLFW3_LIBRARY_RELEASE}")
         set_property(TARGET GLFW::glfw APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
         set_property(TARGET GLFW::glfw PROPERTY IMPORTED_LOCATION_RELEASE "${GLFW3_LIBRARY_RELEASE}")
     endif()
     if(EXISTS "${GLFW3_LIBRARY_DEBUG}")
         set_property(TARGET GLFW::glfw APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
         set_property(TARGET GLFW::glfw PROPERTY IMPORTED_LOCATION_DEBUG "${GLFW3_LIBRARY_DEBUG}")
     endif()

     # If only the combined GLFW3_LIBRARY exists (e.g., no debug/release distinction found)
     if(NOT (EXISTS "${GLFW3_LIBRARY_RELEASE}" OR EXISTS "${GLFW3_LIBRARY_DEBUG}") AND EXISTS "${GLFW3_LIBRARY}")
         set_property(TARGET GLFW::glfw PROPERTY IMPORTED_LOCATION "${GLFW3_LIBRARY}")
     endif()

  else()
    # Create UNKNOWN type if type couldn't be determined reliably
    add_library(GLFW::glfw UNKNOWN IMPORTED)
    message(STATUS "Creating GLFW::glfw as UNKNOWN IMPORTED target")
    set_target_properties(GLFW::glfw PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
      # Set locations directly for UNKNOWN type
      IMPORTED_LOCATION "${GLFW3_LIBRARY}" # Fallback location
      # Optionally provide config-specific locations if known
      IMPORTED_LOCATION_DEBUG "${GLFW3_LIBRARY_DEBUG}"
      IMPORTED_LOCATION_RELEASE "${GLFW3_LIBRARY_RELEASE}"
      # Cannot reliably determine compile definitions for UNKNOWN
    )
  endif()


  # --- Find Dependencies and Link Them ---
  # Dependencies vary by OS and potentially GLFW compile options (Wayland, X11, etc.)
  set(_glfw3_dependencies "")
  find_package(Threads REQUIRED) # Almost always needed
  list(APPEND _glfw3_dependencies Threads::Threads)

  # Platform-specific dependencies
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      # Usually needs X11 and related libraries, unless built for Wayland explicitly (hard to detect here)
      # Prefer finding the X11 package first
      find_package(X11 QUIET)
      if(X11_FOUND)
          list(APPEND _glfw3_dependencies ${X11_LIBRARIES}) # Use legacy variable from FindX11
          # Ensure include dirs are available if needed by transitive usage (usually not needed just for linking)
          # set_property(TARGET GLFW::glfw APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
      else()
          # Fallback: find individual common X11 libs
          message(STATUS "X11 package not found, searching for individual X11 libraries.")
          find_library(X11_LIB NAMES X11)
          find_library(Xrandr_LIB NAMES Xrandr)
          find_library(Xinerama_LIB NAMES Xinerama)
          find_library(Xi_LIB NAMES Xi)
          find_library(Xcursor_LIB NAMES Xcursor)
          find_library(Xxf86vm_LIB NAMES Xxf86vm)
          if(X11_LIB) list(APPEND _glfw3_dependencies ${X11_LIB}) endif()
          if(Xrandr_LIB) list(APPEND _glfw3_dependencies ${Xrandr_LIB}) endif()
          if(Xinerama_LIB) list(APPEND _glfw3_dependencies ${Xinerama_LIB}) endif()
          if(Xi_LIB) list(APPEND _glfw3_dependencies ${Xi_LIB}) endif()
          if(Xcursor_LIB) list(APPEND _glfw3_dependencies ${Xcursor_LIB}) endif()
          if(Xxf86vm_LIB) list(APPEND _glfw3_dependencies ${Xxf86vm_LIB}) endif()
      endif()
      # Common system libs on Linux
      find_library(M_LIB NAMES m) # Math library
      if(M_LIB) list(APPEND _glfw3_dependencies ${M_LIB}) endif()
      find_library(DL_LIB NAMES dl) # Dynamic linking library
      if(DL_LIB) list(APPEND _glfw3_dependencies ${DL_LIB}) endif()
      find_library(RT_LIB NAMES rt) # Realtime library (timers)
      if(RT_LIB) list(APPEND _glfw3_dependencies ${RT_LIB}) endif()

  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # macOS
      # Needs various Apple Frameworks
      find_library(COCOA_FWK Cocoa REQUIRED)
      find_library(IOKIT_FWK IOKit REQUIRED)
      find_library(CORE_FOUNDATION_FWK CoreFoundation REQUIRED)
      find_library(CORE_VIDEO_FWK CoreVideo REQUIRED)
      find_library(CORE_GRAPHICS_FWK CoreGraphics) # Often needed
      # OpenGL framework is usually required, even if using Metal/Vulkan via MoltenVK later
      find_library(OPENGL_FWK OpenGL REQUIRED)

      list(APPEND _glfw3_dependencies ${COCOA_FWK} ${IOKIT_FWK} ${CORE_FOUNDATION_FWK} ${CORE_VIDEO_FWK} ${OPENGL_FWK})
      if(CORE_GRAPHICS_FWK) list(APPEND _glfw3_dependencies ${CORE_GRAPHICS_FWK}) endif()
      # Link frameworks using their full paths
      set_property(TARGET GLFW::glfw PROPERTY INTERFACE_LINK_LIBRARIES "${_glfw3_dependencies}")

  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      # Needs standard Windows libraries
      # These are often linked by default by MSVC but being explicit is safer.
      list(APPEND _glfw3_dependencies gdi32 user32 shell32)
      # Consider adding others if specific compile options were used (e.g., DInput?)
  endif()

  # OpenGL is commonly used with GLFW. Find it and add as a dependency.
  # Make it optional if GLFW could be used without OpenGL (e.g., only Vulkan surface creation).
  find_package(OpenGL QUIET) # Use modern FindOpenGL if available
  if(OpenGL_FOUND)
    list(APPEND _glfw3_dependencies OpenGL::GL) # Link modern OpenGL::GL target
  else()
    message(STATUS "Optional dependency OpenGL not found.")
    # If absolutely required, change QUIET to REQUIRED above
  endif()

  # Set the collected dependencies on the imported target
  # Use generator expressions to avoid issues with mixing targets and plain library names/paths
  set(_glfw3_processed_dependencies "")
  foreach(_dep IN LISTS _glfw3_dependencies)
      if(TARGET ${_dep})
          list(APPEND _glfw3_processed_dependencies ${_dep})
      else()
          # If it's not a target, assume it's a library name or path
          list(APPEND _glfw3_processed_dependencies $<LINK_ONLY:${_dep}>)
      endif()
  endforeach()

  set_property(TARGET GLFW::glfw APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${_glfw3_processed_dependencies}")

  # --- Cleanup ---
  unset(_glfw3_dependencies)
  unset(_glfw3_processed_dependencies)
  unset(_glfw3_lib_ext)
  unset(_glfw3_is_static)
  unset(_glfw3_link_defs)
  unset(_glfw3_target_type)

endif() # GLFW3_FOUND AND NOT TARGET GLFW::glfw

# Mark internal variables as advanced/hidden
mark_as_advanced(
    GLFW3_INCLUDE_DIR # Legacy singular
    GLFW3_LIBRARY     # Legacy singular
    GLFW3_LIBRARY_DEBUG
    GLFW3_LIBRARY_RELEASE
)

# --- Cleanup module-specific variables ---
unset(_glfw3_search_hints)
unset(_glfw3_search_paths)
unset(_glfw3_include_suffixes)
unset(_glfw3_lib_suffixes)
unset(_glfw3_names)
unset(_glfw3_names_debug)
unset(_glfw3_names_release)
