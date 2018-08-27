"""Submit config analysis jobs to NCG batch system."""

from __future__ import division, print_function
import os
import os.path
import optparse
import glob

SOFTDIR = "/lstore/cms/giles/LatticeQCD_IST2018/"


def make_job_file(uid, input_file, output_dir):
    """Build and submit analysis job."""
    output_file = output_dir + str(uid) + '.csv'

    cmd = "./bin/main.exe "
    cmd += "-i " + input_file
    cmd += " -o " + output_file

    job_name = "analysis_" + str(uid) + ".job"
    job_file = open(job_name, "w")
    job_file.write("echo Beginning\ job\n")
    job_file.write("module load gcc-5.4\n")
    job_file.write("export PATH=/lstore/cms/giles/programs/bin:$PATH\n")
    job_file.write("export LD_LIBRARY_PATH=/lstore/cms/giles/programs/lib64\n")
    job_file.write("cd " + SOFTDIR + "\n")
    job_file.write("echo Paths\ set\n")
    job_file.write(cmd + "\n")
    job_file.close()

    sub = "qsub " + job_name
    print("Submitting: " + sub)
    os.system(sub)


if __name__ == "__main__":
    parser = optparse.OptionParser(usage=__doc__)
    parser.add_option("-i", "--input_dir", dest="input_dir",
                      default="/lstore/cms/giles/configs/confs_b6.2_bin/",
                      action="store", help="Directory of configs")
    parser.add_option("-n", "--N", dest="n", action="store", default=-1,
                      help="Number of files to run")
    parser.add_option("-o", "--output_dir", dest="output_dir", action="store",
                      default='Output/', help="Output directory")
    opts, args = parser.parse_args()

    samples = glob.glob(opts.input_dir + '*.bin')
    print('Running over {} of {} samples found'.format(opts.n, len(samples)))
    if opts.n > 0:
        samples = samples[0:int(opts.n)]

    for i, sample in enumerate(samples):
        make_job_file(i, sample, opts.output_dir)
        # break
