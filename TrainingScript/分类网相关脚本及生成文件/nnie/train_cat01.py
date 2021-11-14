# -*- coding: utf-8 -*-
from __future__ import print_function  # do not delete this line if you want to save your log file.
import moxing as mox
import zipfile
import os
import subprocess
from naie.context import Context
os.system('pip install mmcv_full-1.3.9-cp36-cp36m-manylinux1_x86_64.whl')

def load_data():
    from naie.datasets import get_data_reference
    data_reference = get_data_reference(dataset="Default", dataset_entity="train_cat")
    file_paths = data_reference.get_files_paths()
    mox.file.copy(data_reference.get_files_paths()[0], "/cache/cats_classify_newer.zip")
    rf = zipfile.ZipFile("/cache/cats_classify_newer.zip")
    rf.extractall("/cache/")
    rf.close()
    print(file_paths)

def main():
    config = './configs/resnet/resnet18_b32x8_imagenet.py'
    gpus = 8
    workdir = './ckpt'

    cmd =  " {} {} --work-dir {} ".format(config,gpus,workdir)
    print(cmd)
    # subprocess.call("cd /hone/ma-user /work/Agoritha/algo-trash_ classify_test/mnclassification-master l&l sh . tools/dist_train.sh " + cnd, shel- True)subprocess.call("cd mmclassification_master && pip install -e . ", shell=True)
    subprocess.call("cd mmclassification_master && pip install -e . ", shell=True)
    subprocess.call("cd mmclassification_master && sh ./tools/dist_train.sh" + cmd, shell=True)

if __name__ == "__main__":
    load_data()
    main()
    mox.file.copy('/cache/user-job-dir/codes/mmclassification_master/ckpt/latest.pth' , os.path.join(Context.get_model_path(), 'latest.pth'))
    os.path.abspath(".")
    print("path:", os.path.abspath("."))
    print("dir:", os.listdir(os.path.abspath('.')))
