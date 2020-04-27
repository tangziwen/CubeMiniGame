import zipfile as zipfile
import os
def IsPathValid(path, ignoreDir, ignoreExt):
    splited = None
    if os.path.isfile(path):
        if ignoreExt:
            _, ext = os.path.splitext(path)
            if ext in ignoreExt:
                return False

        splited = os.path.dirname(path).split('\\/')
    else:
        if not ignoreDir:
            return True
        splited = path.split('\\/')

    for s in splited:
        if s in ignoreDir:  # You can also use set.intersection or [x for],
            return False

    return True

def zipDirHelper(path, rootDir, zf, ignoreDir = [], ignoreExt = []):
    # zf is zipfile handle
    if os.path.isfile(path):
        if IsPathValid(path, ignoreDir, ignoreExt):
            relative = os.path.relpath(path, rootDir)
            zf.write(path, relative)
        return

    ls = os.listdir(path)
    for subFileOrDir in ls:
        if not IsPathValid(subFileOrDir, ignoreDir, ignoreExt):
            continue

        joinedPath = os.path.join(path, subFileOrDir)
        zipDirHelper(joinedPath, rootDir, zf, ignoreDir, ignoreExt)

def ZipDir(path, zf, ignoreDir = [], ignoreExt = []):
    rootDir = path if os.path.isdir(path) else os.path.dirname(path)
    zipDirHelper(path, rootDir, zf, ignoreDir, ignoreExt)
    pass
    
theZipFile = zipfile.ZipFile("./asset.pkg", 'w', compression=zipfile.ZIP_DEFLATED)
ZipDir("./Asset", theZipFile, ignoreDir=["ArtDev"], ignoreExt=[".zip"])