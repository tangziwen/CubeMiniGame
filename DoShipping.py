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


to_exclude = ["./Asset/ArtDev", ]

#ignores excluded directories and .exe files
def get_ignored(path, filenames):
    ret = []
    for filename in filenames:
        theFilePath = os.path.join(path, filename)
        filePathAbs = os.path.abspath(theFilePath)
        for excludeDir in to_exclude:
            excludeDirAbs = os.path.abspath(excludeDir)
            if excludeDirAbs in filePathAbs:
                print("exclude")
                ret.append(filename)
    return ret


from wand.image import Image
import os
import shutil
shutil.copytree("./Asset/", "./Asset_cook/", ignore = get_ignored)

#convert to dds
g = os.walk("./Asset_cook/")
for path,directories,filelist in g:
    directories[:] = [d for d in directories if d not in ['UITexture']]#ignore UI Texture
    for filename in filelist:
        theFilePath = os.path.join(path, filename)
        if os.path.splitext(theFilePath)[1] in (".png",".PNG", ".tga", ".TGA", ".jpg", ".JPG", ".bmp", ".BMP"):
            print("cook texture",theFilePath)
            os.system("texconv.exe -f DXT5 -vflip -keepcoverage 0.5 -nologo -o "+os.path.dirname(theFilePath) + " \""+ theFilePath + "\"")
            os.remove(theFilePath)
            # with Image(filename= theFilePath) as img:
            #     img.flip()
            #     img.save(filename=os.path.splitext(theFilePath)[0]+'.dds')
            #     #remove Old one
            #     os.remove(theFilePath)

print("packing..")
theZipFile = zipfile.ZipFile("./asset.pkg", 'w', compression=zipfile.ZIP_DEFLATED)
ZipDir("./Asset_cook", theZipFile, ignoreDir=["ArtDev"], ignoreExt=[".zip"])
#remove cooked file
print("clean up")
shutil.rmtree("./Asset_cook/", ignore_errors=False, onerror=None)
print ("done.")