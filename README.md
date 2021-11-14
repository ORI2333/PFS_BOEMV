# PFS_BOEMV
#### 介绍
基于嵌入式机器视觉的宠物喂食系统

其中：     
Pegasus目录：是对Pegasus开发板套件的控制代码补丁；     
Taurus目录：是猫狗检测分类的相关AI插件代码补丁，以及一个训练好的wk模型;        
APP目录：一个手机APP的控制程序代码  

----  

#### 1.Pegasus代码补丁的验证
步骤1：把PFS_BOEMV仓clone下来。    
步骤2：重新解压一个HiHope_WiFi-IoT_Hi3861SPC025的SDK。    
步骤3：将PFS_BOEMV/Pegasus/目录下的所有文件都复制到HiHope_WiFi-IoT_Hi3861SPC025目录下，选择全部替换。     
步骤4：打开HUAWEI LiteOS Studio，打开刚新建的HiHope_WiFi-IoT_Hi3861SPC025工程，进行编译，并烧录到Pegasus开发板中。    
步骤5：验证功能。    

#### 2、Taurus代码补丁的验证

步骤1：把PFS_BOEMV仓clone下来。    
步骤2：按照QQ群提供的资料，在code目录下，重新搭建一份hiopenais_v1.2的环境。    
步骤3：把Taurus/patch/目录下的所有.patch文件都复制到code/hiopenais/patch/目录下。    
步骤4：在code/hiopenais/目录下执行下面的命令，把patch补丁打入步骤2创建的hiopenais环境中。    
```
patch -p1 < patch/0001.hiopenais_build.patch    
patch -p1 < patch/0002.hiopenais_src.patch    
patch -p1 < patch/0003.sdk_sample.patch
patch -p1 < patch/0004.sdk_kernel_dts.patch
```
步骤5：进入code/hiopenais/build/目录下，执行下面的命令，重新编译hiopenais。   
make rebuild && make plugs_rebuild && make boards_rebuild     
步骤6：编译成功后，把code/hiopenais/output/目录下的所有文件都复制到SDK的文件系统中。    
步骤7：把PFS_BOEMV/Taurus/src/目录下的wk文件，复制到文件系统的hiopenais/plugs/目录下。    
步骤8：在code/hiopenais/目录下执行 ./build.sh ext4来制作文件系统    
步骤7：重新烧录镜像，验证功能。    
#### 3、APP代码的功能验证
步骤1：把PFS_BOEMVr仓clone下来。   
步骤2：使用Android Studio把APP目录下的工程打开。    
步骤3：重新build一个工程    
步骤4：连接手机，把APK安装到您的手机中    
