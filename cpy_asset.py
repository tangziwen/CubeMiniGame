import os;
import shutil;

src = "./Res"
desList = ["./build-debug/Res/","./build-release/Res/","./build-profile/Res/"]

for des in desList:
	if os.path.exists(des):
		shutil.copytree(src,des);