import os

os.chdir("tests")
for _file in os.listdir("."):
    print(os.popen(f"hampp {_file}").read())