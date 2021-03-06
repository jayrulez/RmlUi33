#!/usr/bin/env bash

basedir=".."
file=CMake/FileList.cmake
src='set(lib_SRC_FILES'
hdr='set(lib_HDR_FILES'
masterpubhdr='set(MASTER_lib_PUB_HDR_FILES'
pubhdr='set(lib_PUB_HDR_FILES'
srcdir='${PROJECT_SOURCE_DIR}'
fontdefaultbegin='if(NOT NO_FONT_INTERFACE_DEFAULT)'
fontdefaultend='endif()'
srcpath=Source
hdrpath=Include/RmlUi
luapath=Lua
fontdefaultpath=FontEngineDefault

printfiles() {
    # Print headers
    echo ${hdr/lib/$1} >>$file
    find  $srcpath/$1 -maxdepth 3 -path */$luapath -prune -o -path */$fontdefaultpath -prune -o \( -iname "*.h" -o -iname "*.hpp" \) -exec echo '    '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e ')\n' >>$file
    # Print master header for library
    echo ${masterpubhdr/lib/$1} >>$file
    echo '    '$srcdir/Include/RmlUi/$1.h >>$file
    echo -e ')\n' >>$file
    # Print public headers sub directory
    echo ${pubhdr/lib/$1} >>$file
    find  $hdrpath/$1 -maxdepth 3 -path */$luapath -prune -o -path */$fontdefaultpath -prune -o \( -iname "*.h" -o -iname "*.inl" -o -iname "*.hpp" \) -exec echo '    '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e ')\n' >>$file
    # Print source files
    echo ${src/lib/$1} >>$file
    find  $srcpath/$1 -maxdepth 3 -path */$luapath -prune -o -path */$fontdefaultpath -prune -o -iname "*.cpp" -exec echo '    '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e ')\n' >>$file
}

printfontdefaultfiles() {
    # Print headers
	echo $fontdefaultbegin >>$file
    echo '    '${hdr/lib/$1} >>$file
    echo '        ${'$1'_HDR_FILES}' >>$file
    find  $srcpath/$1/$fontdefaultpath -iname "*.h" -exec echo '        '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e '    )\n' >>$file
    # Print source files
    echo '    '${src/lib/$1} >>$file
    echo '        ${'$1'_SRC_FILES}' >>$file
    find  $srcpath/$1/$fontdefaultpath -iname "*.cpp" -exec echo '        '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e '    )' >>$file
	echo -e $fontdefaultend'\n' >>$file
}

printluafiles() {
    # Print headers
    echo ${hdr/lib/Lua${1}} >>$file
    find  $srcpath/$1/$luapath -iname "*.h" -exec echo '    '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e ')\n' >>$file
    # Print public headers
    echo ${pubhdr/lib/Lua${1}} >>$file
    find  $hdrpath/$1/$luapath -iname "*.h" -exec echo '    '$srcdir/{} \; 2>/dev/null | sort -f >>$file 2>/dev/null
    echo -e ')\n' >>$file
    # Print source files
    echo ${src/lib/Lua${1}} >>$file
    find  $srcpath/$1/$luapath -iname "*.cpp" -exec echo '    '$srcdir/{} \; 2>/dev/null | sort -f >>$file
    echo -e ')\n' >>$file
}

pushd $basedir
echo -e "# This file was auto-generated with gen_filelists.sh\n" >$file
for lib in "Core" "Controls" "Debugger"; do
    printfiles $lib
done

printfontdefaultfiles "Core"

for lib in "Core" "Controls"; do
    printluafiles $lib
done
popd

