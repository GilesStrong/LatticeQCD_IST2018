import os, glob
failures = [x[:x.rfind(".")] for x in glob.glob("*.o*") if "not unitary" in open(x).read()]
print len(failures), failures
for f in failures: os.system("rm " + f + ".*")
for f in failures: os.system("qsub " + f)