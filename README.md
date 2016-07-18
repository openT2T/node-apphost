# TBD

```
node build.js --help

--help                     :  display available options
                           :  
--cid_node=[commit-id]     :  Checkout a particular commit from node/node-chakracore repo
--cid_jxcore=[commit-id]   :  Checkout a particular commit from jxcore repo
--dest-cpu=[cpu_type]      :  set target cpu (arm, ia32, x86, x64). i.e. --dest-cpu=ia32
--ndk-path=[path]          :  path to android ndk. This option is required for platform=android
--platform=[target]        :  set target platform. by default 'desktop'. (android, desktop, ios, windows-arm)
--force-target=[jxcore or nodejs] :  Force target framework regardless from the platform
--release                  :  Build release binary. By default Debug
--reset                    :  Clone nodejs and jxcore repos once again
--test                     :  run tests after build
```
