set -e -x

export PYTHONPATH=external:$PYTHONPATH
export DESRES_MODULE_CXXFLAGS=$CXXFLAGS
export DESRES_MODULE_CFLAGS=$CFLAGS
export DESRES_MODULE_LDFLAGS=$LDFLAGS

nprocs=`getconf _NPROCESSORS_ONLN`
python_version=$(python -c 'import sys; print("".join(map(str, sys.version_info[:2])))')
$BUILD_PREFIX/bin/scons install -j $nprocs \
    PREFIX=$PREFIX \
    -D MSYS_WITH_RAPID_JSON=1 \
    -D MSYS_WITH_BLISS=1 \
    -D MSYS_WITH_LPSOLVE=1 \
    -D MSYS_WITH_INCHI=1 \
    PYTHONVER=$python_version

sitepackage_dir=$(python -c 'import os, sys, site; print(os.path.relpath(site.getsitepackages()[0], sys.exec_prefix))')
mv $PREFIX/lib/python/msys $PREFIX/$sitepackage_dir/msys
